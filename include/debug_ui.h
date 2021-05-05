// debug_ui.h

// TODO(lucas): Temporary
#define UI_ID (__LINE__)

enum element_type {
  ELEMENT_NONE = 0,
  ELEMENT_BUTTON,
  ELEMENT_CONTAINER,
};

typedef struct ui_element {
  u32 ID;
  v3 P;
  v2 Size;
  v3 Color;
  i32 Type;

  struct ui_element* Parent;

  u8 Pressed;
  u8 PressedDown;
  u8 Released;
  u8 Hover;
  u8 Movable;
  u8 Active;
  u8 Interaction;
} ui_element;

#define MAX_UI_ELEMENTS 128

typedef struct ui_state {
  ui_element Elements[MAX_UI_ELEMENTS];
  u32 ElementCount;
  u32 ElementIter;
  ui_element* Container;
} ui_state;

void UI_Init();

void UI_Begin();

i32 UI_DoContainer(u32 ID, v2 P, v2 Size, v3 Color, u8 Movable);

i32 UI_DoButton(u32 ID, v2 P, v2 Size, v3 Color);

void UI_Render();
