// ui.c

ui_state UI;
static i32 DeltaX = 0;
static i32 DeltaY = 0;

#define UI_TESTING 0

static v2 UI_GetContainerSize(ui_element* E);
static void UI_Interaction(ui_element* E);
static void UI_AlignToContainer(ui_element* E, ui_element* Container, v2 P);
static void UI_InitElement(ui_element* E, u32 ID, v2 Size, i32 Type);
static void UI_Process(ui_state* State);
static void UI_AlignElement(ui_element* E);
static ui_element* UI_InitInteractable(u32 ID, i32* Prev);
static ui_element* UI_LastElement();
static ui_element* UI_PushElement();

// NOTE(lucas): Get container size based on the container size mode
v2 UI_GetContainerSize(ui_element* E) {
  v2 Size = V2(0, 0);
  v2 ParentSize = V2(Window.Width, Window.Height);
  if (E->Parent) {
    ParentSize = E->Parent->Size;
  }
  else if (UI.Container == E) {
    return V2(Window.Width, Window.Height);
  }
  switch (UI.ContainerSizeMode) {
    case CONTAINER_SIZE_MODE_DEFAULT:
      Size = UI.ContainerSize;
      break;
    case CONTAINER_SIZE_MODE_PERCENT:
      Size = V2(
        (UI.ContainerSize.X * ParentSize.W) - 2 * UIMargin,
        (UI.ContainerSize.Y * ParentSize.H) - 2 * UIMargin
      );
      break;
    default:
      Assert(0 && "Invalid container size mode");
      break;
  }
  return Size;
}

void UI_Interaction(ui_element* E) {
  if (!E->Movable) {
    // Only align non-movable elements
    UI_AlignElement(E);
  }
  E->Focus = 0;
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
        E->Focus = 1;
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
        E->Focus = 1;
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

  E->ID = ID;
  E->P = V3(0, 0, 0);
  E->Size = Size;
#if UI_TESTING
  E->Color = RandomColor();
  E->BorderColor = RandomColor();
#else
  E->Color = V3(0.9f, 0.9f, 0.9f);  // Temp
  E->BorderColor = UIColorBorder;
#endif
  E->BorderThickness = UIBorderThickness;
  E->Type = Type;

  if (E != UI.Container && UI.Container != NULL) {
    if (E != UI.CurrentContainer && UI.CurrentContainer) {
      E->Parent = UI.CurrentContainer;
    }
    else {
      E->Parent = UI.Container;
    }
  }

  if (E->Parent) {
    E->P.Z = E->Parent->P.Z + 0.01f;
  }

  E->UI = NULL;

  E->Text = NULL;
  E->Data = (element_data) { .ToggleValue = 1, };

  E->FillMode = 0;
  E->Fill = V2(0, 0);

  E->Focus = 0;
  E->DrawText = 0;

  E->Pressed = 0;
  E->PressedDown = 0;
  E->Hover = 0;
  E->Movable = 0;
  E->Active = 1;

  switch (E->Type) {
    case ELEMENT_CONTAINER: {
      float ColorFactor = Clamp(0.1f * UI.CurrentDepth, 0.0f, 1.0f);
      E->Color = LerpV3t(UIColorContainer, UIColorContainerBright, ColorFactor);
      if (!UI.Container) {
        UI.Container = E; // Ok, this element is the master container
      }
      else {
        UI.PrevContainer = UI.CurrentContainer;
        UI.CurrentContainer = E;
        E->Size = UI_GetContainerSize(E);
      }
      break;
    }
    case ELEMENT_TEXT_BUTTON:
    case ELEMENT_BUTTON: {
      E->Color = UIColorButton;
      break;
    }
    default:
      break;
  }
#if 0
  // NOTE(lucas): There are probably some alignment issues when it comes to margin placements and such
  if (E->Parent != UI.Prev && UI.Prev != NULL && E->Parent != NULL) {
    switch (UI.PlacementMode) {
      case PLACEMENT_HORIZONTAL: {
Horizontal:
        if (UI.Prev->P.X + UI.Prev->Size.W + E->Size.W - UIMargin < E->Parent->P.X + E->Parent->Size.W) {
          E->P.X = UI.Prev->P.X + UI.Prev->Size.W + UIMargin;
          E->P.Y = UI.Prev->P.Y;
          break;
        }
        E->P.X = E->Parent->P.X;
        goto Vertical;
      }
      case PLACEMENT_VERTICAL: {
Vertical:
        E->P.X = UI.Prev->P.X;
        // E->P.X += UIMargin;
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
#endif
  UI.Prev = E;
}

void UI_Process(ui_state* State) {
  TIMER_START();
  if (!State)
    return;

  for (u32 ElementIndex = 0; ElementIndex < State->ElementCount; ++ElementIndex) {
    ui_element* E = &State->Elements[ElementIndex];
    if (!E->Interaction) {
      ht_key Key = E->ID;
      HtRemoveElement(&UI.ElementLocations, Key);
      --State->ElementCount;
      if (State->ElementCount > 0) {
        *E = State->Elements[State->ElementCount];
        if (E->ID != Key) {
          ht_value Location = E - &State->Elements[0];
          Key = E->ID;
          HtInsertElement(&State->ElementLocations, Key, Location); // The swapped element now has a new location, therefore we update the hash table accordingly
        }
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

void UI_AlignElement(ui_element* E) {
  if (E->Type == ELEMENT_CONTAINER) {
    E->Size = UI_GetContainerSize(E);
  }
  if (E->Parent == NULL) {
    if (E == UI.Container) {
      return;
    }
  }
  else if (E->Parent && E->Parent != E) {
    if (!UI.Prev) {
      E->P = V3(
        E->Parent->P.X + UIMargin,
        E->Parent->P.Y + UIMargin,
        E->P.Z
      );
    }
    else {
      switch (UI.PlacementMode) {
        case PLACEMENT_VERTICAL: {
          break;
        }
        case PLACEMENT_HORIZONTAL: {
          if (UI.Prev->P.X + UI.Prev->Size.W + E->Size.W + (2 * UIMargin) < E->Parent->P.X + E->Parent->Size.W) {
            E->P.X = UI.Prev->P.X + UI.Prev->Size.W + UIMargin;
            E->P.Y = UI.Prev->P.Y;
          }
          else {
            E->P.X = E->Parent->P.X + UIMargin;
            E->P.Y = UI.Prev->P.Y + UI.Prev->Size.H + UIMargin;
          }
          if (E->Type == ELEMENT_CONTAINER) {
            if (E->Parent) {
              float Delta = (E->P.Y + E->Size.H) - (E->Parent->P.Y + E->Parent->Size.H);
              if (Delta > 0) {
                E->Size.H -= Delta + UIMargin;
              }
            }
          }
          break;
        }
        default:
          break;
      }
    }
  }
  UI.Prev = E;
}

ui_element* UI_InitInteractable(u32 ID, i32* Prev) {
  ui_element* E = NULL;

  ht_key Key = ID;
  const ht_value* Value = HtLookup(&UI.ElementLocations, Key);
  if (Value) {
    E = &UI.Elements[*Value];
  }

  if (E) {
    *Prev = 1;
  }
  else {
    *Prev = 0;
    u32 Location = UI.ElementCount;
    E = UI_PushElement();
    if (E) {
      HtInsertElement(&UI.ElementLocations, ID, Location);
    }
  }

  Assert(E);
  return E;
}

ui_element* UI_LastElement() {
  if (UI.ElementCount > 0) {
    return &UI.Elements[UI.ElementCount - 1];
  }
  return NULL;
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
  UI.ElementLocations = HtCreateEmpty();
  UI.Container = NULL;
  UI.CurrentContainer = NULL;
  UI.PrevContainer = NULL;
  UI.Prev = NULL;
  UI.PlacementMode = PLACEMENT_HORIZONTAL;
  UI.ContainerSize = V2(0, 0);
  UI.ContainerSizeMode = CONTAINER_SIZE_MODE_DEFAULT;
  UI.CurrentDepth = 0;
}

void UI_Free() {
  HtFree(&UI.ElementLocations);
}

void UI_Begin() {
  UI.CurrentDepth = 0;
  UI.CurrentContainer = NULL;
  UI.PrevContainer = NULL;
  UI.Prev = NULL;

  UI_Process(&UI);
}

i32 UI_DoContainer(u32 ID) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI.PrevContainer = UI.CurrentContainer; // To be able to go back to the parent container
    UI_InitElement(E, ID, V2(0, 0), ELEMENT_CONTAINER);
  }
  UI_Interaction(E);

  UI.Prev = NULL;
  UI.PrevContainer = UI.CurrentContainer;
  UI.CurrentContainer = E;

  ++UI.CurrentDepth;
  return E->Active;
}

i32 UI_EndContainer() {
  UI.Prev = UI.CurrentContainer;
  UI.CurrentContainer = UI.CurrentContainer->Parent;  // NOTE(lucas): Change from UI.PrevContainer to UI.CurrentContainer->Parent
  UI.PrevContainer = NULL;
  --UI.CurrentDepth;
  return NoError;
}

i32 UI_SetContainerSizeMode(container_size_mode Mode) {
  UI.ContainerSizeMode = Mode;
  return NoError;
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
    Size.W = (1 + strlen(Text)) * UITextSize * UITextKerning;
    UI_InitElement(E, ID, Size, ELEMENT_TEXT_BUTTON);
    E->DrawText = 1;
    E->Color = UIColorButton;
  }
  E->Text = Text;
  UI_Interaction(E);
  return E->Released;
}

i32 UI_DoBufferButton(u32 ID, const char* Format, ...) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    v2 Size = UIButtonSize;
    UI_InitElement(E, ID, Size, ELEMENT_TEXT_BUTTON);
  }
  E->Text = Format;
  UI_Interaction(E);
  return E->Released;
}

i32 UI_DoBox(u32 ID, v2 Size, v3 Color) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, Size, ELEMENT_BUTTON);
  }
  E->Color = Color;
  UI_Interaction(E);
  return E->Released;
}

i32 UI_DoTextToggle(u32 ID, const char* Text, u8* Value) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    v2 Size = UIButtonSize;
    Size.W = (1 + strlen(Text)) * UITextSize * UITextKerning;
    UI_InitElement(E, ID, Size, ELEMENT_TOGGLE);
    E->DrawText = 1;
    E->Color = UIColorButton;
  }
  E->Text = Text;
  if (Value) {
    E->Data.ToggleValue = *Value;
  }
  UI_Interaction(E);

  if (E->Released) {
    E->Data.ToggleValue = !E->Data.ToggleValue;
    if (Value) {
      *Value = E->Data.ToggleValue;
    }
  }
  return E->Released;
}

void UI_SetPlacement(element_placement_mode Mode) {
  UI.PlacementMode = Mode;
}

void UI_WindowResizeCallback(i32 Width, i32 Height) {
}

// TODO(lucas): Implement "scissoring"/clipping of 2d elements
void UI_Render() {
  v4 DefaultClipping = Clip;
  v4 Clipping = Clip;
  for (u32 ElementIndex = 0; ElementIndex < UI.ElementCount; ++ElementIndex) {
    ui_element* E = &UI.Elements[ElementIndex];
    if (E->Active) {
      if (E->Parent) {
        v3 P = E->Parent->P;
        v2 Size = E->Parent->Size;
        Clipping = V4(
          P.X, P.Y,
          P.X + Size.W, P.Y + Size.H
        );
        SetClipping(Clipping);
      }
      else {
        SetClipping(DefaultClipping);
      }
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
#define HighContrastMode 0
      if (HighContrastMode) {
        BorderColor = ColorInvert(Color);
      }
      v3 P = E->P;
      DrawRectangle(P, E->Size, Color, BorderColor, E->BorderThickness);
      if (E->DrawText && E->Text) {
        v3 TextP = P;
        TextP.Y += E->Size.H / 2.0f - UITextSize / 2.0f;
        TextP.Z += 0.01f;
        v3 TextColor = ColorGray(ColorInvert(ColorGain(E->Color, 2.0f)));
        DrawText(TextP, E->Size, TextColor, UITextKerning, UITextLeading, UITextSize, E->Text);
      }
    }
  }
  SetClipping(DefaultClipping); // Go back to the original clipping value
}
