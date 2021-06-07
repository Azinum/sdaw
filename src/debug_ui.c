// debug_ui.c

ui_state UI;
static i32 DeltaX = 0;
static i32 DeltaY = 0;

static void UI_Interaction(ui_element* E);
static void UI_AlignToContainer(ui_element* E, ui_element* Container, v2 P);
static void UI_InitElement(ui_element* E, u32 ID, v2 P, v2 Size, v3 Color, i32 Type);
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

void UI_InitElement(ui_element* E, u32 ID, v2 P, v2 Size, v3 Color, i32 Type) {
  Assert(E);
  E->ID = ID;
  E->P = V3(P.X, P.Y, 0);
  E->Size = Size;
  E->Color = Color;
  E->Type = Type;

  E->Parent = UI.Container != E ? UI.Container : NULL;

  if (E->Parent) {
    E->P.Z = E->Parent->P.Z + 0.01f;
  }

  E->UI = NULL;

  E->Text = NULL;

  E->Pressed = 0;
  E->PressedDown = 0;
  E->Hover = 0;
  E->Movable = 0;
  E->Active = 1;
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
}

void UI_Begin() {
  UI_Process(&UI);
}

i32 UI_DoContainer(u32 ID, v2 P, v2 Size, v3 Color, u8 Movable) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, P, Size, Color, ELEMENT_CONTAINER);
    E->Movable = Movable;
    UI.Container = E;
  }
  UI_AlignToContainer(E, E->Parent, P);
  UI_Interaction(E);
  return E->Active;
}

i32 UI_DoButton(u32 ID, v2 P, v2 Size, v3 Color) {
  i32 Prev = 0;
  ui_element* E = UI_InitInteractable(ID, &Prev);
  if (!Prev) {
    UI_InitElement(E, ID, P, Size, Color, ELEMENT_BUTTON);
  }
  UI_AlignToContainer(E, E->Parent, P);
  UI_Interaction(E);
  return E->Released;
}

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

void UI_Render() {
  for (u32 ElementIndex = 0; ElementIndex < UI.ElementCount; ++ElementIndex) {
    ui_element* E = &UI.Elements[ElementIndex];
    if (E->Active) {
      v3 Color = E->Color;
      if (E->Type != ELEMENT_CONTAINER) {
        if (E->Pressed || E->PressedDown) {
          Color = LerpV3t(E->Color, V3(0, 0, 0), 0.2f);
        }
        else if (E->Hover) {
          Color = LerpV3t(E->Color, V3(1, 1, 1), 0.2f);
        }
      }
      DrawRect(E->P, E->Size, Color);
      if (E->Type == ELEMENT_TEXT_BUTTON) {
        v3 TextP = E->P;
        TextP.Z += 0.01f;
        DrawText(TextP, E->Size, V3(1, 1, 1), E->Text);
      }
    }
  }
}
