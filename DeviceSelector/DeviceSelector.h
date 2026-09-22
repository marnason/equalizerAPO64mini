/*
	This file is part of EqualizerAPO, a system-wide equalizer.
	Copyright (C) 2026

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
*/

#pragma once

#include <memory>
#include <vector>
#include <DeviceAPOInfo.h>
#include <QtWidgets/QDialog>

class QDialogButtonBox;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class DeviceSelector : public QDialog
{
	Q_OBJECT

public:
	DeviceSelector(QWidget* parent = nullptr);

private:
	void addDevices(const std::vector<std::shared_ptr<DeviceAPOInfo>>& devices, QTreeWidgetItem* parentNode);
	void onDeviceToggled(QTreeWidgetItem* item, int column);
	void applyInstallationChanges();
	void updateApplyButton();
	bool hasInstallationChanges() const;
	QString getStateText(const std::shared_ptr<DeviceAPOInfo>& info, bool checked) const;

	QTreeWidget* deviceTreeWidget;
	QDialogButtonBox* buttonBox;
	QPushButton* applyButton;
	bool populating = false;
};

Q_DECLARE_METATYPE(std::shared_ptr<DeviceAPOInfo>)
