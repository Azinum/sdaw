// ui.h

#ifndef _UI_H
#define _UI_H

#define FILE_ID Hash(__FILE__)
#define FUNCTION_ID Hash((char*)__FUNCTION__)

#define UI_ID (FILE_ID + FUNCTION_ID + __LINE__)

enum element_type {
  ELEMENT_NONE = 0,
  ELEMENT_BUTTON,
  ELEMENT_TEXT_BUTTON,
  ELEMENT_CONTAINER,
  ELEMENT_TOGGLE,
};

typedef enum element_placement_mode {
  PLACEMENT_VERTICAL = 0,
  PLACEMENT_HORIZONTAL,
  PLACEMENT_FILL,
} element_placement_mode;

typedef enum container_size_mode {
  CONTAINER_SIZE_MODE_DEFAULT,  // Which is in pixels
  CONTAINER_SIZE_MODE_PERCENT,  // Percent defined in decimal from 0-1 (in relation to the element's parent)
} container_size_mode;

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

  u8 FillMode;  // TODO(lucas): Implement
  v2 Fill;      // TODO(lucas): Implement

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
  hash_table ElementLocations;  // ID to element index
  ui_element* Container;  // Master
  ui_element* CurrentContainer;
  ui_element* PrevContainer;
  ui_element* Prev;
  element_placement_mode PlacementMode;
  v2 ContainerSize;
  container_size_mode ContainerSizeMode;
  u32 CurrentDepth;
} ui_state;

void UI_Init();

void UI_Free();

void UI_Refresh();

void UI_Begin();

i32 UI_DoContainer(u32 ID);

i32 UI_EndContainer();

i32 UI_SetContainerSizeMode(container_size_mode Mode);

i32 UI_SetContainerSize(v2 Size);

i32 UI_DoButton(u32 ID);

i32 UI_DoTextButton(u32 ID, const char* Text);

i32 UI_DoBox(u32 ID, v2 Size, v3 Color);

#if 0
i32 UI_DoTextButton(u32 ID, v2 P, v2 Size, v3 Color, const char* Text);

i32 UI_DoSpecialButton(u32 ID, v2 P, v2 Size, v3 Color);

i32 UI_DoToggle(u32 ID, v2 P, v2 Size, v3 Color, u8* Value);
#endif

void UI_SetPlacement(element_placement_mode Mode);

void UI_WindowResizeCallback(i32 Width, i32 Height);

void UI_Render();

#endif
