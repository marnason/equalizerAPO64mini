/*
	This file is part of EqualizerAPO, a system-wide equalizer.
	Copyright (C) 2014  Jonas Thedering

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include <algorithm>
#include <cmath>
#include <cstring>

#include "PreampProcessor.h"

static_assert(std::atomic<double>::is_always_lock_free,
	"The real-time preamp path requires lock-free gain publication");

PreampProcessor::PreampProcessor()
	: inputChannelCount(0), outputChannelCount(0), rampFrameCount(1), targetGain(1.0),
	currentGain(1.0), rampTarget(1.0), rampStep(0.0), rampFramesRemaining(0)
{
}

void PreampProcessor::initialize(unsigned inputChannels, unsigned outputChannels, unsigned sampleRate, double initialGainDb)
{
	inputChannelCount = inputChannels;
	outputChannelCount = outputChannels;
	rampFrameCount = (std::max)(1u, sampleRate / 100u);
	currentGain = std::pow(10.0, initialGainDb / 20.0);
	targetGain.store(currentGain, std::memory_order_release);
	rampTarget = currentGain;
	rampStep = 0.0;
	rampFramesRemaining = 0;
}

void PreampProcessor::setGainDb(double dbGain)
{
	targetGain.store(std::pow(10.0, dbGain / 20.0), std::memory_order_release);
}

#pragma AVRT_CODE_BEGIN
void PreampProcessor::process(float* output, const float* input, unsigned frameCount)
{
	const double requestedGain = targetGain.load(std::memory_order_acquire);
	if (requestedGain != rampTarget)
	{
		rampTarget = requestedGain;
		rampFramesRemaining = rampFrameCount;
		rampStep = (rampTarget - currentGain) / rampFramesRemaining;
	}

	if (currentGain == 1.0 && rampFramesRemaining == 0 && inputChannelCount == outputChannelCount)
	{
		if (output != input)
			std::memcpy(output, input, frameCount * outputChannelCount * sizeof(float));
		return;
	}

	const unsigned copiedChannelCount = (std::min)(inputChannelCount, outputChannelCount);
	auto processFrame = [&](unsigned frame, double gainFactor)
	{
		const float* inputFrame = input + frame * inputChannelCount;
		float* outputFrame = output + frame * outputChannelCount;

		for (unsigned channel = 0; channel < copiedChannelCount; ++channel)
		{
			if (gainFactor == 1.0)
				outputFrame[channel] = inputFrame[channel];
			else
				outputFrame[channel] = static_cast<float>(static_cast<double>(inputFrame[channel]) * gainFactor);
		}

		// Preserve Windows' normal mono-to-stereo behavior when this APO changes
		// the negotiated channel topology. Any further synthetic channels remain silent.
		if (inputChannelCount == 1 && outputChannelCount >= 2)
			outputFrame[1] = outputFrame[0];
		for (unsigned channel = copiedChannelCount; channel < outputChannelCount; ++channel)
		{
			if (!(inputChannelCount == 1 && channel == 1))
				outputFrame[channel] = 0.0f;
		}
	};

	// Expanding an in-place interleaved buffer must run backwards so output from
	// one frame cannot overwrite input belonging to the next frame.
	if (output == input && outputChannelCount > inputChannelCount)
	{
		for (unsigned frame = frameCount; frame > 0; --frame)
		{
			const unsigned advance = (std::min)(frame, rampFramesRemaining);
			processFrame(frame - 1, currentGain + rampStep * advance);
		}
	}
	else
	{
		for (unsigned frame = 0; frame < frameCount; ++frame)
		{
			const unsigned advance = (std::min)(frame + 1, rampFramesRemaining);
			processFrame(frame, currentGain + rampStep * advance);
		}
	}

	const unsigned advancedFrames = (std::min)(frameCount, rampFramesRemaining);
	currentGain += rampStep * advancedFrames;
	rampFramesRemaining -= advancedFrames;
	if (rampFramesRemaining == 0)
	{
		currentGain = rampTarget;
		rampStep = 0.0;
	}
}
#pragma AVRT_CODE_END
