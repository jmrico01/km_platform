#pragma once

#include "main_platform.h"

#define NOMINMAX
#include <Windows.h>

struct Win32ReplayBuffer
{
	char filePath[MAX_PATH];
	HANDLE fileHandle;
	HANDLE memoryMap;
	void* gameMemoryBlock;
};

struct Win32State
{
	uint64 gameMemorySize;
	void* gameMemoryBlock;
	Win32ReplayBuffer replayBuffers[4];

	HANDLE recordingHandle;
	int inputRecordingIndex;

	HANDLE playbackHandle;
	int inputPlayingIndex;

	char exeFilePath[MAX_PATH];
	char* exeOnePastLastSlash;
};
