// ui.c

ui_state UI;
static i32 DeltaX = 0;
static i32 DeltaY = 0;

static float UIMargin = 4.0f;
static i32 UITextSize = 14;
static float UITextKerning = 0.6f;
static float UITextLeading = 1.5f;

static void UI_Interaction(ui_element* E);
static void UI_AlignToContainer(ui_element* E, ui_element* Container, v2 P);
static void UI_InitElement(ui_element* E, u32 ID, v2 Size, i32 Type);
static void UI_Process(ui_state* State);
static ui_element* UI_InitInteractable(u32 ID, i32* Prev);
static ui_element* UI_PushElement();

void UI_Interaction(ui_element* E) {
  if (MouseOver(MouseX, MouseY, E->P.X, E->P.Y, E->Size.W, E->Size.H)) {
    E->Hover = 1;
  }
  else {
    E->Hover = 0;
  }
  if (E->Hover) {
    if (LeftMousePressed) {
      E->Pressed = 1;
      if (E->Movable) {
        DeltaX = E->P.X - MouseX;
        DeltaY = E->P.Y - MouseY;
      }
    }
    else {
      E->Pressed = 0;
    }
    if (LeftMouseDown) {
      E->PressedDown = 1;
      if (E->Movable) {
        E->P.X = MouseX + DeltaX;
        E->P.Y = MouseY + DeltaY;
      }
    }
    else {
      if (E->PressedDown) {
        E->Released = 1;
      }
      else {
        E->Released = 0;
      }
      E->PressedDown = 0;
    }
  }
  else {
    E->Pressed = 0;
    E->PressedDown = 0;
    E->Released = 0;
  }
  E->Interaction = 1;
}

void UI_AlignToContainer(ui_element* E, ui_element* Container, v2 P) {
  if (!Container)
    return;

  E->P = V3(
    P.X + Container->P.X,
    P.Y + Container->P.Y,
    E->P.Z
  );
}

void UI_InitElement(ui_element* E, u32 ID, v2 Size, i32 Type) {
  Assert(E);

  if (!UI.Container) {
    Size = V2(
      WindowWidth(),
      WindowHeight()
    );
  }

  E->ID = ID;
  E->P = V3(0, 0, 0);
  E->Size = Size;
  E->Color = V3(0.9f, 0.9f, 0.9f);  // Temp
  E->BorderColor = V3(0, 0, 0); // Temp
  E->Type = Type;

  E->Parent = UI.Container != E ? UI.Container : NULL;

  if (E->Parent) {
    E->P.Z = E->Parent->P.Z + 0.01f;
  }

  E->UI = NULL;

  E->Text = NULL;
  E->Data = (element_data) { .ToggleValue = 1, };

  E->Pressed = 0;
  E->PressedDown = 0;
  E->Hover = 0;
  E->Movable = 0;
  E->Active = 1;

  switch (E->Type) {
    case ELEMENT_CONTAINER: {
      E->Color = V3(0.3f, 0.3f, 0.3f);  // Temp
      UI.Container = E;
      break;
    }
    default:
      break;
  }

  if (E->Parent != UI.Prev && UI.Prev != NULL && E->Parent != NULL) {
    switch (UI.PlacementMode) {
      case PLACEMENT_HORIZONTAL: {
Horizontal:
        if (UI.Prev->P.X + UI.Prev->Size.W + UIMargin + E->Size.W < E->Parent->P.X + E->Parent->Size.W) {
          E->P.X = UI.Prev->P.X + UI.Prev->Size.W + UIMargin;
          E->P.Y = UI.Prev->P.Y;
          break;
        }
        E->P.X = E->Parent->P.X;
        goto Vertical;
      }
      case PLACEMENT_VERTICAL: {
Vertical:
        E->P.X += UIMargin;
        E->P.Y = UI.Prev->P.Y + UI.Prev->Size.H + UIMargin;
        break;
      }
      default:
        break;
    }
  }
  else if (E->Parent && E->Parent != NULL && UI.Prev == NULL) {
    E->P = V3(
      E->Parent->P.X + UIMargin,
      E->Parent->P.Y + UIMargin,
      E->P.Z
    );
  }
  UI.Prev = E;
}

void UI_Process(ui_state* State) {
  TIMER_START();
  for (u32 ElementIndex = 0; ElementIndex < State->ElementCount; ++ElementIndex) {
    ui_element* E = &State->Elements[ElementIndex];
    if (!E->Interaction) {
      --State->ElementCount;
      if (State->ElementCount > 0) {
        *E = State->Elements[State->ElementCount];
      }
      continue;
    }
    if (E->Type == ELEMENT_CONTAINER && E->UI && E->Active) {
      UI_Process(E->UI);
    }
    E->Interaction = 0;
  }
  TIMER_END();
}

ui_element* UI_InitInteractable(u32 ID, i32* Prev) {
  ui_element* E = NULL;

  // This is quite inefficient as we are doing this for each element. Maybe use hash table or other solution?
  for (u32 ElementIndex = 0; ElementIndex < UI.ElementCount; ++ElementIndex) {
    ui_element* Elem = &UI.Elements[ElementIndex];
    if (Elem->ID == ID) {
      E = Elem;
      break;
    }
  }

  if (E) {
    *Prev = 1;
  }
  else {
    *Prev = 0;
    E = UI_PushElement();
  }

  Assert(E);
  return E;
}

ui_element* UI_PushElement() {
  ui_element* E = NULL;

  if (UI.ElementCount < MAX_UI_ELEMENTS) {
    E = &UI.Elements[UI.ElementCount++];
  }

  return E;
}

void UI_Init() {
  UI.ElementCount = 0;
  UI.Container = NULL;
  UI.Prev = NULL;
  UI.PlacementMode = PLACEMENT_VERTICAL;
  UI.ShouldRefresh = 0;
  UI.ContainerSize = V2(0, 0);
}

// Refresh the ui upon removing elements. The way this works is subject to change.
void UI_Refresh() {
  UI.ShouldRefresh = 1;
}

void UI_Begin() {
  if (UI.ShouldRefresh) {
    UI_Init();
  }
  UI_Process(&UI);
}

i32 UI_DoContainer(u32 ID) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, UI.ContainerSize, ELEMENT_CONTAINER);
    UI.Prev = NULL;
  }
  UI_Interaction(E);
  return E->Active;
}

i32 UI_SetContainerSize(v2 Size) {
  UI.ContainerSize = Size;
  return NoError;
}

i32 UI_DoButton(u32 ID) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    v2 Size = UIButtonSize;
    UI_InitElement(E, ID, Size, ELEMENT_BUTTON);
  }
  UI_Interaction(E);
  return E->Released;
}

i32 UI_DoTextButton(u32 ID, const char* Text) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    v2 Size = UIButtonSize;
    UI_InitElement(E, ID, Size, ELEMENT_TEXT_BUTTON);
  }
  E->Text = Text;
  UI_Interaction(E);
  return E->Released;
}

#if 0
i32 UI_DoTextButton(u32 ID, v2 P, v2 Size, v3 Color, const char* Text) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, P, Size, Color, ELEMENT_TEXT_BUTTON);
  }
  E->Text = Text;
  UI_AlignToContainer(E, E->Parent, P);
  UI_Interaction(E);
  return E->Released;
}

i32 UI_DoSpecialButton(u32 ID, v2 P, v2 Size, v3 Color) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, P, Size, Color, ELEMENT_BUTTON);
  }
  UI_AlignToContainer(E, E->Parent, P);
  UI_Interaction(E);
  return E->PressedDown;
}

i32 UI_DoToggle(u32 ID, v2 P, v2 Size, v3 Color, u8* Value) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, P, Size, Color, ELEMENT_TOGGLE);
  }
  UI_AlignToContainer(E, E->Parent, P);
  UI_Interaction(E);
  if (E->Released) {
    E->Data.ToggleValue = !E->Data.ToggleValue;
    if (Value)
      *Value = E->Data.ToggleValue;
  }
  return E->Released;
}
#endif

void UI_SetPlacement(element_placement_mode Mode) {
  UI.PlacementMode = Mode;
}

void UI_WindowResizeCallback(i32 Width, i32 Height) {
  UI_Init();
}

void UI_Render() {
  v4 DefaultClipping = Clip;
  v4 Clipping = Clip;
  for (u32 ElementIndex = 0; ElementIndex < UI.ElementCount; ++ElementIndex) {
    ui_element* E = &UI.Elements[ElementIndex];
    if (E->Active) {
      if (E->Parent) {
        v3 P = E->Parent->P;
        v2 Size = E->Parent->Size;
        // Clipping = V4(
        //   P.X, P.Y,
        //   P.X + Size.W, P.Y + Size.H
        // );
        Clipping = V4(
          0, 0,
          Size.W, Size.H
        );
        // SetClipping(Clipping);
      }
      else {
        SetClipping(DefaultClipping);
      }
      float BorderThickness = 1.0f;
      v3 Color = E->Color;
      v3 BorderColor = E->BorderColor;
      if (E->Type != ELEMENT_CONTAINER) {
        if (E->Type == ELEMENT_TOGGLE) {
          if (!E->Data.ToggleValue) {
            Color = UIColorInactive;
          }
        }
        if (E->Pressed || E->PressedDown) {
          Color = LerpV3t(E->Color, V3(0, 0, 0), 0.35f);
        }
        else if (E->Hover) {
          Color = LerpV3t(Color, V3(0, 0, 0), 0.15f);
          BorderColor = LerpV3t(BorderColor, V3(0, 0, 0), 0.2f);
        }
      }
      DrawRectangle(E->P, E->Size, Color, BorderColor, BorderThickness);
      if (E->Type == ELEMENT_TEXT_BUTTON) {
        v3 TextP = E->P;
        TextP.Y += E->Size.H / 2.0f - UITextSize / 2.0f;
        TextP.Z += 0.01f;
        v3 TextColor = ColorInvert(E->Color);
        DrawText(TextP, E->Size, TextColor, UITextKerning, UITextLeading, UITextSize, E->Text);
      }
    }
  }
  SetClipping(DefaultClipping); // Go back to the original clipping value
}
