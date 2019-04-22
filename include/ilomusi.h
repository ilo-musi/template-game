/*
Ilo Musi Game Template - A starter template for creating new games for ilo musi

Written in 2019 by Wong Cho Ching <https://sadale.net>

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#include <stdbool.h>

//System API
extern unsigned int systemGetApiVersion(void);
extern unsigned int systemGetTick(void);
extern void systemSleep(unsigned int milliseconds, bool wakeUpToBoostMode);
extern bool systemIsBoostMode(void);

//Keys API
#define KEYS_UP		1<<0
#define KEYS_LEFT	1<<1
#define KEYS_RIGHT	1<<2
#define KEYS_DOWN	1<<3
#define KEYS_1		1<<4
#define KEYS_2		1<<5

extern void keysSetDebouncePeriod(unsigned int milliseconds);
extern uint8_t keysGetPressedState();
extern uint8_t keysGetJustChangedState();

//Storage API
extern void storageFlash(uint8_t resNum);
extern unsigned int storageRead(uint8_t resNum, uint32_t offset, void *buf, unsigned int len);
extern unsigned int storageWrite(uint8_t resNum, uint32_t offset, const void *buf, unsigned int len);

//Synth API
#define SYNTH_FREQ_SWEEP_SAW (0U<<0)
#define SYNTH_FREQ_SWEEP_TRIANGLE (1U<<0)
#define SYNTH_FREQ_SWEEP_BIPOLAR (2U<<0)
#define SYNTH_FREQ_SWEEP_NOISE (3U<<0)
#define SYNTH_FREQ_SWEEP_OFFSET (0U)
#define SYNTH_FREQ_SWEEP_MASK (0x03U)

#define SYNTH_FREQ_SWEEP_REPEAT_DISABLE (0U<<2)
#define SYNTH_FREQ_SWEEP_REPEAT_ENABLE (1U<<2)

#define SYNTH_FREQ_SWEEP_MULTIPLER_1 (0U<<3)
#define SYNTH_FREQ_SWEEP_MULTIPLER_256 (1U<<3)


#define SYNTH_DUTY_SWEEP_SAW (0U<<4)
#define SYNTH_DUTY_SWEEP_TRIANGLE (1U<<4)
#define SYNTH_DUTY_SWEEP_BIPOLAR (2U<<4)
#define SYNTH_DUTY_SWEEP_NOISE (3U<<4)
#define SYNTH_DUTY_SWEEP_OFFSET (4U)
#define SYNTH_DUTY_SWEEP_MASK (0x03U<<SYNTH_DUTY_SWEEP_OFFSET)

#define SYNTH_DUTY_SWEEP_REPEAT_DISABLE (0U<<6)
#define SYNTH_DUTY_SWEEP_REPEAT_ENABLE (1U<<6)

#define SYNTH_FAST_STEP_MODE_DISABLE (0U<<7)
#define SYNTH_FAST_STEP_MODE_ENABLE (1U<<7)
#define SYNTH_FAST_STEP_MODE_MAX_COUNTER_VALUE (4U-1U)

#define SYNTH_NUM_TICKS_FOR_A_STEP (20000U) //Equivalent to 10ms for each recalculation step.

#define SYNTH_STATE_BG_MASK (1U<<0)
#define SYNTH_STATE_FG_MASK (1U<<1)

enum SynthCommandMode {
	SYNTH_COMMAND_MODE_ONCE,
	SYNTH_COMMAND_MODE_DURATION_FREQUENCY,
	SYNTH_COMMAND_MODE_DURATION_COMPLETE_FREQUENCY,
	SYNTH_COMMAND_MODE_FULL,
	SYNTH_COMMAND_MODE_FULL_ONCE_RETURN_TO_DF,
	SYNTH_COMMAND_MODE_FULL_ONCE_RETURN_TO_DCF,
};

enum SynthMode{
	SYNTH_MODE_ADDITIVE,
	SYNTH_MODE_FG_REPLACES_BG,
	SYNTH_MODE_FG_FREEZE_BG,
	SYNTH_MODE_NUM, //Do not use
};

struct __attribute__ ((__packed__)) SynthData{
	uint8_t durationRemaining;
	uint8_t initialFreq;
	uint8_t finalFreq;
	uint8_t freqSweepPeriod;
	uint8_t initialDutyCycle:4;
	uint8_t finalDutyCycle:4;
	uint8_t dutySweepPeriod;
	uint8_t controlFlags;
};

extern void synthPlayOne(bool foreground, const struct SynthData *synthData);
extern void synthPlayCommand(bool foreground, const uint8_t *command); //To stop, send in SYNTH_COMMAND_MODE_ONCE or command==NULL
extern void synthSetMode(enum SynthMode mode);
extern unsigned int synthGetStatus(void); //bit 0: bg. bit 1: fg


//Graphic API
#define GRAPHIC_HEIGHT (8)
#define GRAPHIC_WIDTH (128)

#define GRAPHIC_SIZE (GRAPHIC_HEIGHT*GRAPHIC_WIDTH)

#define GRAPHIC_PIXEL_HEIGHT (GRAPHIC_HEIGHT*8)
#define GRAPHIC_PIXEL_WIDTH GRAPHIC_WIDTH

#define GRAPHIC_SHAPE_TRIANGLE_BOTTOM_LEFT ((void*)0xFFFFFFFF)
#define GRAPHIC_SHAPE_TRIANGLE_BOTTOM_RIGHT ((void*)0xFFFFFFFE)
#define GRAPHIC_SHAPE_TRIANGLE_TOP_LEFT ((void*)0xFFFFFFFD)
#define GRAPHIC_SHAPE_TRIANGLE_TOP_RIGHT ((void*)0xFFFFFFFC)
#define GRAPHIC_SHAPE_IS_TRIANGLE(SHAPE) ((uint32_t)SHAPE >= GRAPHIC_SHAPE_TRIANGLE_TOP_RIGHT && (uint32_t)SHAPE <= GRAPHIC_SHAPE_TRIANGLE_BOTTOM_LEFT)
#define GRAPHIC_SHAPE_RECTANGLE ((void*)0xFFFFFFFB)
#define GRAPHIC_SHAPE_ELLIPSE ((void*)0xFFFFFFFA) //Limitation: Only support even number width/height. If it's not the filled one, it doesn't work well on small circles

#define GRAPHIC_MODE_BACKGROUND_NONE	(0U)
#define GRAPHIC_MODE_BACKGROUND_AND_NOT	(1U)
#define GRAPHIC_MODE_BACKGROUND_OR		(2U)
#define GRAPHIC_MODE_BACKGROUND_XOR		(3U)
#define GRAPHIC_MODE_BACKGROUND_MASK	(0x3U)

#define GRAPHIC_MODE_FOREGROUND_MASK_OFFSET (2)
#define GRAPHIC_MODE_FOREGROUND_NONE	(0U<<GRAPHIC_MODE_FOREGROUND_MASK_OFFSET)
#define GRAPHIC_MODE_FOREGROUND_AND_NOT		(1U<<GRAPHIC_MODE_FOREGROUND_MASK_OFFSET)
#define GRAPHIC_MODE_FOREGROUND_OR		(2U<<GRAPHIC_MODE_FOREGROUND_MASK_OFFSET)
#define GRAPHIC_MODE_FOREGROUND_XOR		(3U<<GRAPHIC_MODE_FOREGROUND_MASK_OFFSET)
#define GRAPHIC_MODE_FOREGROUND_MASK	(0x3U<<GRAPHIC_MODE_FOREGROUND_MASK_OFFSET)

#define GRAPHIC_MODE_SHAPE_FILL_NONE	(0U<<4)
#define GRAPHIC_MODE_SHAPE_FILL_FULL	(1U<<4)
#define GRAPHIC_MODE_SHAPE_FILL_MASK	(0x1U<<4)

#define GRAPHIC_MODE_TEXT_WORD_WRAP_ENABLE	(0U<<5)
#define GRAPHIC_MODE_TEXT_WORD_WRAP_DISABLE	(1U<<5)
#define GRAPHIC_MODE_TEXT_WORD_WRAP_MASK	(0x1U<<5)

#define GRAPHIC_MODE_OPAQUE			(GRAPHIC_MODE_BACKGROUND_AND_NOT|GRAPHIC_MODE_FOREGROUND_OR)
#define GRAPHIC_MODE_TRANSPARENT	(GRAPHIC_MODE_BACKGROUND_NONE|GRAPHIC_MODE_FOREGROUND_OR)

#define GRAPHIC_FONT_WIDTH ((uint8_t)5)
#define GRAPHIC_FONT_HEIGHT ((uint8_t)8)

enum GraphicDisplayMode{
	GRAPHIC_DISPLAY_NORMAL,
	GRAPHIC_DISPLAY_INVERTED,
	GRAPHIC_DISPLAY_ALL_ON,
	GRAPHIC_DISPLAY_ALL_OFF,
	GRAPHIC_DISPLAY_NUM,
};

struct __attribute__((packed)) GraphicImage{
	void* image;
	uint8_t width;
	uint8_t height; //Max height: 64
};
extern void graphicClearDisplay(bool inverted);
extern void graphicSetFont(uint8_t (*font)[GRAPHIC_FONT_WIDTH]);
extern void graphicDrawText(const char *text, unsigned int length, int x, int y, unsigned int width, unsigned int height, unsigned int mode);
extern void graphicDrawImage(const struct GraphicImage *image, int x, int y, unsigned int mode);
extern void graphicDrawToColumn(uint64_t *displayColumn, uint64_t newColumn, unsigned int mode);


extern void graphicLoadImage(const void *buf, struct GraphicImage *image);
extern void graphicSetDrawBuffer(const void *buf);
extern void graphicReadColumnFromDrawBuffer(uint64_t *displayColumn, unsigned int columnId);
extern void graphicWriteColumnToDrawBuffer(const uint64_t *displayColumn, unsigned int columnId);
extern void graphicSetDisplayMode(enum GraphicDisplayMode displayMode);
extern bool graphicDisplay(const void *buf);
extern bool graphicIsDisplayReady(void);
