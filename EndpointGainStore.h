/*
	This file is part of EqualizerAPO, a system-wide equalizer.
	Copyright (C) 2026

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
*/

#pragma once

#include <string>

class EndpointGainStore
{
public:
	static constexpr int MIN_GAIN_MILLIDB = -60000;
	static constexpr int MAX_GAIN_MILLIDB = 30000;
	static constexpr int DEFAULT_GAIN_MILLIDB = 0;
	static constexpr const wchar_t* REGISTRY_PATH = L"SOFTWARE\\EqualizerAPO\\Gains";

	static int readGainMilliDb(const std::wstring& endpointGuid);
	static void writeGainMilliDb(const std::wstring& endpointGuid, int gainMilliDb);
	static double toDb(int gainMilliDb);
	static bool isValid(int gainMilliDb);
};
