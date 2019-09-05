#pragma once

#include <app_info.h>
#include <km_common/km_defines.h>
#include <km_common/km_input.h>
#include <km_common/km_log.h>
#include <km_common/km_math.h>
#include <opengl.h>

#define MIDI_IN_QUEUE_SIZE 1024

#ifdef APP_315K
#define ARDUINO_CHANNELS 4
#define ARDUINO_ANALOG_INPUTS  6
#define ARDUINO_DIGITAL_INPUTS 1
#endif

struct ThreadContext
{
	int placeholder;
};

// ---------------------------- Platform functions ----------------------------
struct DEBUGReadFileResult
{
	uint64 size;
	void* data;
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY_FUNC(name) \
	void name(const ThreadContext* thread, DEBUGReadFileResult* file)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY_FUNC(DEBUGPlatformFreeFileMemoryFunc);

#define DEBUG_PLATFORM_READ_FILE_FUNC(name) \
	DEBUGReadFileResult name(const ThreadContext* thread, const char* fileName)
typedef DEBUG_PLATFORM_READ_FILE_FUNC(DEBUGPlatformReadFileFunc);

#define DEBUG_PLATFORM_WRITE_FILE_FUNC(name) \
	bool32 name(const ThreadContext* thread, const char* fileName, \
		uint64 memorySize, const void* memory, bool overwrite)
typedef DEBUG_PLATFORM_WRITE_FILE_FUNC(DEBUGPlatformWriteFileFunc);

#define MAX_KEYS_PER_FRAME 256

struct ScreenInfo
{
	bool32 changed;

	Vec2Int size;

	int8 colorBits;
	int8 alphaBits;
	int8 depthBits;
	int8 stencilBits;
};

struct GameButtonState
{
	int transitions;
	bool isDown;
};

struct GameControllerInput
{
	bool isConnected;

	Vec2 leftStart;
	Vec2 leftEnd;
	Vec2 rightStart;
	Vec2 rightEnd;

	union
	{
		GameButtonState buttons[6];
		struct
		{
			GameButtonState a;
			GameButtonState b;
			GameButtonState x;
			GameButtonState y;
			GameButtonState lShoulder;
			GameButtonState rShoulder;
		};
	};
};

struct MidiMessage
{
	uint8 status;
	uint8 dataByte1;
	uint8 dataByte2;
};

struct MidiInput
{
	int numMessages;
	MidiMessage messages[MIDI_IN_QUEUE_SIZE];
};

#ifdef APP_315K
struct ArduinoInput
{
	bool connected;

	uint8 activeChannel;
	GameButtonState pedal;
	float32 analogValues[ARDUINO_CHANNELS][ARDUINO_ANALOG_INPUTS];
};
#endif

struct GameInput
{
	GameButtonState mouseButtons[5];
	Vec2Int mousePos;
	Vec2Int mouseDelta;
	int mouseWheel;
	int mouseWheelDelta;

	GameButtonState keyboard[KM_KEY_LAST];
	char keyboardString[MAX_KEYS_PER_FRAME];
	uint32 keyboardStringLen;

	GameControllerInput controllers[4];

	MidiInput midiIn;
#ifdef APP_315K
	ArduinoInput arduinoIn;
#endif
};

struct GameAudio
{
	uint32 sampleRate;
	uint8 channels;
	uint64 bufferSizeSamples;
	float32* buffer;

	uint64 sampleDelta; // Samples elapsed/written since last update
	uint64 fillLength;
};

struct PlatformFunctions
{
	PlatformFlushLogsFunc* flushLogs;
	OpenGLFunctions glFunctions;

	DEBUGPlatformFreeFileMemoryFunc* DEBUGPlatformFreeFileMemory;
	DEBUGPlatformReadFileFunc*       DEBUGPlatformReadFile;
	DEBUGPlatformWriteFileFunc*      DEBUGPlatformWriteFile;
};

struct MemoryBlock
{
	uint64 size;
	// Required to be cleared to zero at startup    TODO... why??
	void* memory;
};

struct GameMemory
{
	bool32 isInitialized;

	MemoryBlock permanent;
	MemoryBlock transient;

	bool32 shouldInitGlobalVariables;
};

// ------------------------------ Game functions ------------------------------
#define GAME_UPDATE_AND_RENDER_FUNC(name) void name( \
	const ThreadContext* thread, \
	const PlatformFunctions* platformFuncs, \
	const GameInput* input, ScreenInfo screenInfo, float32 deltaTime, \
	GameMemory* memory, GameAudio* audio, LogState* logState)
typedef GAME_UPDATE_AND_RENDER_FUNC(GameUpdateAndRenderFunc);
