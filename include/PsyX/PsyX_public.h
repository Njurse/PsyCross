#ifndef EMULATOR_PUBLIC_H
#define EMULATOR_PUBLIC_H

#define CONTROLLER_MAP_FLAG_AXIS		0x4000
#define CONTROLLER_MAP_FLAG_INVERSE		0x8000

typedef struct
{
	int id;

	int kc_square, kc_circle, kc_triangle, kc_cross;

	int kc_l1, kc_l2, kc_l3;
	int kc_r1, kc_r2, kc_r3;

	int kc_start, kc_select;

	int kc_dpad_left, kc_dpad_right, kc_dpad_up, kc_dpad_down;
} PsyXKeyboardMapping;

typedef struct
{
	int id;

	// you can bind axis by adding CONTROLLER_MAP_AXIS_FLAG
	int gc_square, gc_circle, gc_triangle, gc_cross;

	int gc_l1, gc_l2, gc_l3;
	int gc_r1, gc_r2, gc_r3;

	int gc_start, gc_select;

	int gc_dpad_left, gc_dpad_right, gc_dpad_up, gc_dpad_down;

	int gc_axis_left_x, gc_axis_left_y;
	int gc_axis_right_x, gc_axis_right_y;
} PsyXControllerMapping;

typedef void(*GameDebugKeysHandlerFunc)(int nKey, char down);
typedef void(*GameDebugMouseHandlerFunc)(int x, int y, int dx, int dy);
typedef void(*GameOnTextInputHandler)(const char* buf);

//------------------------------------------------------------------------

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/* Mapped inputs */
extern PsyXControllerMapping		g_cfg_controllerMapping;
extern PsyXKeyboardMapping			g_cfg_keyboardMapping;
extern int							g_cfg_controllerToSlotMapping[2];

/* Game inputs */
extern GameOnTextInputHandler		g_cfg_gameOnTextInput;

/* Graphics configuration */
extern int							g_cfg_swapInterval;
extern int							g_cfg_pgxpZBuffer;
extern int							g_cfg_bilinearFiltering;
extern int							g_cfg_pgxpTextureCorrection;

/* Debug inputs */
extern GameDebugKeysHandlerFunc		g_dbg_gameDebugKeys;
extern GameDebugMouseHandlerFunc	g_dbg_gameDebugMouse;

/* Usually called at the beginning of main function */
extern void PsyX_Initialise(char* windowName, int screenWidth, int screenHeight, int fullscreen);

/* Cleans all resources and closes open instances */
extern void PsyX_Shutdown(void);

/* Returns the screen size dimensions */
extern void PsyX_GetScreenSize(int* screenWidth, int* screenHeight);

/* Sets mouse cursor position */
extern void PsyX_SetCursorPosition(int x, int y);

/* Sets mouse relative movement */
extern void PsyX_SetCursorRelative(int enable);

/* Usually called after ClearOTag/ClearOTagR */
extern char PsyX_BeginScene(void);

/* Usually called after DrawOTag/DrawOTagEnv */
extern void PsyX_EndScene(void);

/* Explicitly updates emulator input loop */
extern void PsyX_UpdateInput(void);

/* Returns keyboard mapping index */
extern int PsyX_LookupKeyboardMapping(const char* str, int default_value);

/* Returns controller mapping index */
extern int PsyX_LookupGameControllerMapping(const char* str, int default_value);

/* Screen size of emulated PSX viewport with widescreen offsets */
extern void PsyX_GetPSXWidescreenMappedViewport(struct _RECT16* rect);

/* Waits for timer */
extern void PsyX_WaitForTimestep(int count);

/* Changes swap interval state */
extern void PsyX_EnableSwapInterval(int enable);

/* Changes swap interval interval interval */
extern void PsyX_SetSwapInterval(int interval);

/* JERICHO: is the frame-skip (BACKSPACE debug fast-forward) currently ON?
 *
 * This is the latch behind the BACKSPACE hold: set on key-down, cleared by the
 * key-up, by a lost focus (which eats the key-up) and by
 * PsyX_SetFastForwardLocked. While on, PsyX stops swapping with vsync.
 *
 * It no longer raises the sim rate by itself: the step gate runs on wall-clock
 * milliseconds (FilterFrameTime), so a skip can only make the game DRAW less.
 * The engine reads the same latch through PsyX_GetFastForwardHeld and re-asserts
 * its own `FastForward` (7 steps per frame) while the key is down. Exposed so a
 * diagnostic can see it; a latched skip (JERICHO_FF_LATCH) remains the test
 * lever for a key-up that never arrives.
 */
extern int PsyX_GetFrameSkip(void);

/* JERICHO: is the BACKSPACE fast-forward key HELD?
 *
 * Same latch as PsyX_GetFrameSkip (key-down sets it; key-up, focus loss and
 * PsyX_SetFastForwardLocked clear it), but read by the engine as "the fast-forward
 * key is down". BACKSPACE is a hold: State_GameLoop re-asserts FastForward from
 * this every gated frame, because DisplayOverlays clears FastForward at the end
 * of a frame and a key-down alone would only buy that one frame. */
extern int PsyX_GetFastForwardHeld(void);

/* JERICHO: gate that debug fast-forward off (a session owns the clock).
 *
 * While locked, BACKSPACE no longer turns the skip on, and a skip that was
 * already latched is cleared immediately -- which is the point: a client running
 * the sim fast desyncs, and the lock is the module's way of saying it owns the
 * rate. Mirrors the engine's FastForwardLocked (director.h). */
extern void PsyX_SetFastForwardLocked(int locked);

/* JERICHO: wall-clock milliseconds (SDL_GetTicks). Use THIS, not the vblank
 * counter, to compute a sim rate: while the frame skip is on the vblank counter
 * advances extra times per frame, so a rate derived from it under-reports. */
extern unsigned int PsyX_GetTicks(void);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif