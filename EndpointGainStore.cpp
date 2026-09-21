/*
	This file is part of EqualizerAPO, a system-wide equalizer.
	Copyright (C) 2026

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
*/

#include "stdafx.h"
#include <cstdint>
#include <stdexcept>
#include "EndpointGainStore.h"

namespace
{
	std::wstring endpointPath(const std::wstring& endpointGuid)
	{
		return std::wstring(EndpointGainStore::REGISTRY_PATH) + L"\\" + endpointGuid;
	}
}

bool EndpointGainStore::isValid(int gainMilliDb)
{
	return gainMilliDb >= MIN_GAIN_MILLIDB && gainMilliDb <= MAX_GAIN_MILLIDB;
}

double EndpointGainStore::toDb(int gainMilliDb)
{
	return static_cast<double>(gainMilliDb) / 1000.0;
}

int EndpointGainStore::readGainMilliDb(const std::wstring& endpointGuid)
{
	DWORD value = 0;
	DWORD size = sizeof(value);
	const LSTATUS result = RegGetValueW(HKEY_LOCAL_MACHINE, endpointPath(endpointGuid).c_str(), L"GainMilliDb",
		RRF_RT_REG_DWORD | RRF_SUBKEY_WOW6464KEY, nullptr, &value, &size);
	if (result == ERROR_FILE_NOT_FOUND)
		return DEFAULT_GAIN_MILLIDB;
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("Unable to read endpoint gain");

	const int signedValue = static_cast<int>(static_cast<int32_t>(value));
	if (!isValid(signedValue))
		throw std::out_of_range("Stored endpoint gain is outside the supported range");
	return signedValue;
}

void EndpointGainStore::writeGainMilliDb(const std::wstring& endpointGuid, int gainMilliDb)
{
	if (!isValid(gainMilliDb))
		throw std::out_of_range("Endpoint gain is outside the supported range");

	HKEY key = nullptr;
	const LSTATUS openResult = RegCreateKeyExW(HKEY_LOCAL_MACHINE, endpointPath(endpointGuid).c_str(), 0, nullptr,
		REG_OPTION_NON_VOLATILE, KEY_SET_VALUE | KEY_WOW64_64KEY, nullptr, &key, nullptr);
	if (openResult != ERROR_SUCCESS)
		throw std::runtime_error("Unable to open endpoint gain registry key");

	const DWORD value = static_cast<DWORD>(static_cast<int32_t>(gainMilliDb));
	const LSTATUS writeResult = RegSetValueExW(key, L"GainMilliDb", 0, REG_DWORD,
		reinterpret_cast<const BYTE*>(&value), sizeof(value));
	RegCloseKey(key);
	if (writeResult != ERROR_SUCCESS)
		throw std::runtime_error("Unable to write endpoint gain");
}
