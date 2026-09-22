/*
	This file is part of EqualizerAPO, a system-wide equalizer.
	Copyright (C) 2026

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
*/

#include "stdafx.h"
#include <DeviceAPOInfo.h>
#include <EndpointGainStore.h>
#include <helpers/RegistryHelper.h>
#include <helpers/ServiceHelper.h>
#include "../version.h"
#include "DeviceSelector.h"

namespace
{
	constexpr int INFO_ROLE = Qt::UserRole;
}

DeviceSelector::DeviceSelector(QWidget* parent)
	: QDialog(parent)
{
	setWindowFlags(windowFlags().setFlag(Qt::WindowContextHelpButtonHint, false));
	setWindowIcon(QIcon(":/icons/preferences-system.ico"));
	resize(820, 480);

	QString version = QString("%1.%2").arg(MAJOR).arg(MINOR);
	if (REVISION != 0)
		version += QString(".%1").arg(REVISION);
	setWindowTitle(tr("Equalizer APO %1").arg(version));

	auto* layout = new QVBoxLayout(this);
	auto* description = new QLabel(tr("Select the endpoints that should use Equalizer APO and set an independent preamp gain for each device."), this);
	description->setWordWrap(true);
	layout->addWidget(description);

	deviceTreeWidget = new QTreeWidget(this);
	deviceTreeWidget->setColumnCount(4);
	deviceTreeWidget->setHeaderLabels({tr("Connector"), tr("Device"), tr("Status"), tr("Gain")});
	deviceTreeWidget->setRootIsDecorated(true);
	deviceTreeWidget->setAlternatingRowColors(true);
	layout->addWidget(deviceTreeWidget);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
	applyButton = buttonBox->addButton(tr("Apply installation changes"), QDialogButtonBox::ApplyRole);
	layout->addWidget(buttonBox);

	populating = true;
	try
	{
		auto* playback = new QTreeWidgetItem(deviceTreeWidget, {tr("Playback devices")});
		playback->setFirstColumnSpanned(true);
		playback->setExpanded(true);
		addDevices(DeviceAPOInfo::loadAllInfos(false), playback);

		auto* capture = new QTreeWidgetItem(deviceTreeWidget, {tr("Capture devices")});
		capture->setFirstColumnSpanned(true);
		capture->setExpanded(true);
		addDevices(DeviceAPOInfo::loadAllInfos(true), capture);
	}
	catch (RegistryException& e)
	{
		QMessageBox::critical(this, tr("Registry error"), QString::fromStdWString(e.getMessage()));
	}
	populating = false;

	for (int column = 0; column < deviceTreeWidget->columnCount(); ++column)
		deviceTreeWidget->resizeColumnToContents(column);

	connect(deviceTreeWidget, &QTreeWidget::itemChanged, this, &DeviceSelector::onDeviceToggled);
	connect(applyButton, &QPushButton::clicked, this, &DeviceSelector::applyInstallationChanges);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::accept);
	updateApplyButton();

	const bool fixedAudioDG = !DeviceAPOInfo::checkProtectedAudioDG(true);
	const bool fixedRegistration = !DeviceAPOInfo::checkAPORegistration(true);
	if (fixedAudioDG || fixedRegistration)
		QMessageBox::information(this, tr("Audio registration repaired"),
			tr("Required Windows audio registration was repaired. Apply an installation change or restart Windows before using the affected endpoint."));
}

void DeviceSelector::addDevices(const std::vector<std::shared_ptr<DeviceAPOInfo>>& devices, QTreeWidgetItem* parentNode)
{
	for (const auto& info : devices)
	{
		const bool checked = info->isInstalled();
		auto* item = new QTreeWidgetItem(parentNode, {
			QString::fromStdWString(info->getConnectionName()),
			QString::fromStdWString(info->getDeviceName()),
			getStateText(info, checked)
		});
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
		item->setData(0, INFO_ROLE, QVariant::fromValue(info));

		auto* gain = new QDoubleSpinBox(deviceTreeWidget);
		gain->setRange(EndpointGainStore::MIN_GAIN_MILLIDB / 1000.0, EndpointGainStore::MAX_GAIN_MILLIDB / 1000.0);
		gain->setSingleStep(0.1);
		gain->setDecimals(1);
		gain->setSuffix(tr(" dB"));
		try
		{
			gain->setValue(EndpointGainStore::toDb(EndpointGainStore::readGainMilliDb(info->getDeviceGuid())));
		}
		catch (const std::exception&)
		{
			gain->setValue(0.0);
		}
		connect(gain, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this, info](double value) {
			try
			{
				EndpointGainStore::writeGainMilliDb(info->getDeviceGuid(), qRound(value * 1000.0));
			}
			catch (const std::exception&)
			{
				QMessageBox::critical(this, tr("Registry error"), tr("The gain could not be saved."));
			}
		});
		deviceTreeWidget->setItemWidget(item, 3, gain);
	}
}

void DeviceSelector::onDeviceToggled(QTreeWidgetItem* item, int column)
{
	if (populating || column != 0 || item->childCount() != 0)
		return;
	const auto info = item->data(0, INFO_ROLE).value<std::shared_ptr<DeviceAPOInfo>>();
	item->setText(2, getStateText(info, item->checkState(0) == Qt::Checked));
	updateApplyButton();
}

void DeviceSelector::applyInstallationChanges()
{
	bool changed = false;
	for (int groupIndex = 0; groupIndex < deviceTreeWidget->topLevelItemCount(); ++groupIndex)
	{
		QTreeWidgetItem* group = deviceTreeWidget->topLevelItem(groupIndex);
		for (int index = 0; index < group->childCount(); ++index)
		{
			QTreeWidgetItem* item = group->child(index);
			const auto info = item->data(0, INFO_ROLE).value<std::shared_ptr<DeviceAPOInfo>>();
			const bool checked = item->checkState(0) == Qt::Checked;
			try
			{
				if (checked && !info->isInstalled())
					info->install();
				else if (!checked && info->isInstalled())
					info->uninstall();
				else if (checked && (info->canBeUpgraded() || info->isEnhancementsDisabled()))
					info->reinstall();
				else
					continue;
				changed = true;
			}
			catch (RegistryException& e)
			{
				QMessageBox::critical(this, tr("Registry error"), QString::fromStdWString(e.getMessage()));
				return;
			}
		}
	}

	if (changed)
	{
		try
		{
			ServiceHelper::restartService(L"AudioSrv");
		}
		catch (ServiceException& e)
		{
			QMessageBox::warning(this, tr("Restart required"),
				tr("The Windows audio service could not be restarted. Restart Windows to apply the installation changes.\n\n%1")
				.arg(QString::fromStdWString(e.getMessage())));
		}
	}

	accept();
}

bool DeviceSelector::hasInstallationChanges() const
{
	for (int groupIndex = 0; groupIndex < deviceTreeWidget->topLevelItemCount(); ++groupIndex)
	{
		QTreeWidgetItem* group = deviceTreeWidget->topLevelItem(groupIndex);
		for (int index = 0; index < group->childCount(); ++index)
		{
			QTreeWidgetItem* item = group->child(index);
			const auto info = item->data(0, INFO_ROLE).value<std::shared_ptr<DeviceAPOInfo>>();
			const bool checked = item->checkState(0) == Qt::Checked;
			if (checked != info->isInstalled()
				|| checked && (info->canBeUpgraded() || info->isEnhancementsDisabled()))
				return true;
		}
	}
	return false;
}

void DeviceSelector::updateApplyButton()
{
	applyButton->setEnabled(hasInstallationChanges());
}

QString DeviceSelector::getStateText(const std::shared_ptr<DeviceAPOInfo>& info, bool checked) const
{
	QString state;
	if (checked && !info->isInstalled())
		state = tr("Will be installed");
	else if (!checked && info->isInstalled())
		state = tr("Will be removed");
	else if (info->isInstalled() && info->canBeUpgraded())
		state = tr("Update available");
	else if (info->isInstalled() && info->isEnhancementsDisabled())
		state = tr("Enhancements disabled");
	else if (info->isInstalled())
		state = tr("Active");
	else
		state = tr("Not installed");

	if (info->isDefaultDevice())
		state += tr(", default");
	if (info->isDisabled())
		state += tr(", disabled");
	if (info->isUnplugged())
		state += tr(", disconnected");
	return state;
}
