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

struct PlatformReadFileResult
{
	uint64 size;
	void* data;
};

template <typename Allocator>
PlatformReadFileResult PlatformReadFile(const ThreadContext* thread, Allocator* allocator,
	const char* filePath);
template <typename Allocator>
void PlatformFreeFile(const ThreadContext* thread, Allocator* allocator,
	PlatformReadFileResult* file);
bool PlatformWriteFile(const ThreadContext* thread, const char* filePath,
	uint64 memorySize, const void* memory, bool overwrite);
bool PlatformFileExists(const ThreadContext* thread, const char* filePath);
bool PlatformFileChanged(const ThreadContext* thread, const char* filePath);

#define MAX_KEYS_PER_FRAME 256

struct ScreenInfo
{
	bool changed;

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
	OpenGLFunctions glFunctions;
	PlatformFlushLogsFunc* flushLogs;
};

struct MemoryBlock
{
	uint64 size;
	// Required to be cleared to zero at startup    TODO... why??
	void* memory;
};

struct GameMemory
{
	bool isInitialized;

	MemoryBlock permanent;
	MemoryBlock transient;

	bool shouldInitGlobalVariables;
};

// ------------------------------ Game functions ------------------------------
void GameUpdateAndRender(const ThreadContext* thread, const PlatformFunctions* platformFuncs,
	const GameInput* input, ScreenInfo screenInfo, float32 deltaTime,
	GameMemory* memory, GameAudio* audio, LogState* logState);
