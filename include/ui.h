// ui.h

#ifndef _UI_H
#define _UI_H

#define FILE_ID Hash(__FILE__)
#define FUNCTION_ID Hash((char*)__FUNCTION__)

#define UI_ID ((u32)(__LINE__ * (FILE_ID + FUNCTION_ID)))

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
  float BorderThickness;
  i32 Type;

  struct ui_element* Parent;
  struct ui_state* UI;

  const char* Text;
  string String;
  element_data Data;

  u8 Focus;
  u8 DrawText;

  u8 Pressed;
  u8 PressedDown;
  u8 Released;
  u8 Hover;
  u8 Movable;
  u8 Active;
  u8 Interaction;
} ui_element;

typedef struct ui_state {
  ui_element* Elements;
  u32 ElementCount;
  u32 ElementAllocCount;
  hash_table ElementLocations;  // ID to element index
  ui_element* Container;  // Master
  ui_element* CurrentContainer;
  ui_element* PrevContainer;
  ui_element* Prev;
  element_placement_mode PlacementMode;
  v2 ContainerSize;
  container_size_mode ContainerSizeMode;
  u8 CurrentDepth;
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

i32 UI_DoStringButton(u32 ID, const char* Format, ...);

i32 UI_DoBox(u32 ID, v2 Size, v3 Color);

i32 UI_DoTextToggle(u32 ID, const char* Text, u8* Value);

void UI_SetPlacement(element_placement_mode Mode);

void UI_WindowResizeCallback(i32 Width, i32 Height);

void UI_Render();

#endif
