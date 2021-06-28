// ui.h

#ifndef _UI_H
#define _UI_H

// TODO(lucas): Temporary
#define UI_ID (__LINE__)

enum element_type {
  ELEMENT_NONE = 0,
  ELEMENT_BUTTON,
  ELEMENT_TEXT_BUTTON,
  ELEMENT_CONTAINER,
  ELEMENT_TOGGLE,
};

typedef enum element_placement_mode {
  PLACEMENT_BELOW = 0,
  PLACEMENT_NEXT,
} element_placement_mode;

struct ui_state;

typedef union element_data {
  struct {
    u8 ToggleValue;
  };
} element_data;

typedef struct ui_element {
  u32 ID;
  v3 P;
  v2 Size;
  v3 Color;
  v3 BorderColor;
  i32 Type;

  struct ui_element* Parent;
  struct ui_state* UI;

  const char* Text;
  element_data Data;

  u8 Pressed;
  u8 PressedDown;
  u8 Released;
  u8 Hover;
  u8 Movable;
  u8 Active;
  u8 Interaction;
} ui_element;

#define MAX_UI_ELEMENTS 256

typedef struct ui_state {
  ui_element Elements[MAX_UI_ELEMENTS];
  u32 ElementCount;
  ui_element* Container;
  ui_element* Prev;
  element_placement_mode PlacementMode;
} ui_state;

void UI_Init();

void UI_Begin();

i32 UI_DoContainer(u32 ID);

i32 UI_DoButton(u32 ID);

#if 0
i32 UI_DoTextButton(u32 ID, v2 P, v2 Size, v3 Color, const char* Text);

i32 UI_DoSpecialButton(u32 ID, v2 P, v2 Size, v3 Color);

i32 UI_DoToggle(u32 ID, v2 P, v2 Size, v3 Color, u8* Value);
#endif

void UI_SetPlacement(element_placement_mode Mode);

void UI_WindowResizeCallback(i32 Width, i32 Height);

void UI_Render();

#endif
