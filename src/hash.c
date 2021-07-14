// hash.c

#define UNUSED_SLOT 0
#define USED_SLOT 1
#define HASH_TABLE_INIT_SIZE 13

struct item {
  ht_key Key;
  ht_value Value;
  i32 UsedSlot;
};

static u32 HashKey(const ht_key Key, const u32 Size);
static u32 LinearProbe(const hash_table* Table, const ht_key Key, u32* CollisionCount);
static i32 KeyCompare(const ht_key A, const ht_key B);
static hash_table ResizeTable(hash_table* Table, u32 NewSize);

u32 HashKey(const ht_key Key, const u32 Size) {
  return Key % Size;
}

u32 LinearProbe(const hash_table* Table, const ht_key Key, u32* CollisionCount) {
  u32 Index = HashKey(Key, Table->Size);
  u32 Counter = 0;
  for (; Index < HtGetSize(Table); Index++, Counter++) {
    if (KeyCompare(Table->Items[Index].Key, Key) || Table->Items[Index].UsedSlot == UNUSED_SLOT) {
      return Index;
    }

    if (Counter >= HtGetSize(Table)) {
      return -1;
    }

    if (Index + 1 >= HtGetSize(Table)) {
      ++(*CollisionCount);
      Index = 0;
    }
    ++(*CollisionCount);
  }
  return Index;
}

i32 KeyCompare(const ht_key A, const ht_key B) {
  return A == B;
  // return strncmp(A, B, HTABLE_KEY_SIZE) == 0;
}

hash_table ResizeTable(hash_table* Table, u32 NewSize) {
  Assert(Table != NULL);

  if (HtNumElements(Table) > NewSize)
    return *Table;

  hash_table NewTable = HtCreate(NewSize);
  if (HtGetSize(&NewTable) != NewSize) {
    // Allocation failed
    return *Table;
  }

  struct item Item;
  for (u32 Index = 0; Index < HtGetSize(Table); ++Index) {
    Item = Table->Items[Index];
    if (Item.UsedSlot != UNUSED_SLOT) {
      HtInsertElement(&NewTable, Item.Key, Item.Value);
    }
  }
  HtFree(Table);
  return NewTable;
}

hash_table HtCreate(u32 Size) {
  if (!Size)
    Size = 1;

  hash_table Table = {
    .Items = M_Calloc(sizeof(struct item), Size),
    .Count = 0,
    .Size = Size
  };
  if (!Table.Items) {
    // Allocation failed
    Table.Size = 0;
  }
  return Table;
}

hash_table HtCreateEmpty() {
  hash_table Table = {
    .Items = NULL,
    .Count = 0,
    .Size = 0
  };
  return Table;
}

i32 HtIsEmpty(const hash_table* Table) {
  Assert(Table != NULL);
  return Table->Items == NULL;
}

u32 HtInsertElement(hash_table* Table, const ht_key Key, const ht_value Value) {
  Assert(Table != NULL);
  if (HtIsEmpty(Table)) {
    hash_table NewTable = HtCreate(HASH_TABLE_INIT_SIZE);
    if (HtGetSize(&NewTable) == HASH_TABLE_INIT_SIZE) {
      *Table = NewTable;
    }
    else {
      return 0;
    }
  }
  if (HtNumElements(Table) > (HtGetSize(Table) / 2)) {
    *Table = ResizeTable(Table, Table->Size * 2);
  }

  u32 Collisions = 0;
  u32 Index = LinearProbe(Table, Key, &Collisions);
  if (Index < HtGetSize(Table)) {
    struct item Item = { .Value = Value, .Key = Key, .UsedSlot = USED_SLOT };
    // strncpy(Item.Key, Key, HTABLE_KEY_SIZE);
    Table->Items[Index] = Item;
    Table->Count++;
  }
  Assert(HtLookup(Table, Key) != NULL);
  return Collisions;
}

const ht_value* HtLookup(const hash_table* Table, const ht_key Key) {
  Assert(Table != NULL);
  if (HtGetSize(Table) == 0) return NULL;
  u32 Collisions = 0;
  u32 Index = LinearProbe(Table, Key, &Collisions);
  if (Index < HtGetSize(Table)) {
    struct item* Item = &Table->Items[Index];
    if (Item->UsedSlot == UNUSED_SLOT) {
      return NULL;
    }
    if (KeyCompare(Item->Key, Key)) {
      return &Item->Value;
    }
  }
  return NULL;
}

const ht_value* HtLookupByIndex(const hash_table* Table, const u32 Index) {
  Assert(Table != NULL);
  if (Index < HtGetSize(Table)) {
    if (Table->Items[Index].UsedSlot != UNUSED_SLOT)
      return &Table->Items[Index].Value;
  }
  return NULL;
}

const ht_key* HtLookupKey(const hash_table* Table, const u32 Index) {
  Assert(Table != NULL);
  if (Index < HtGetSize(Table)) {
    if (Table->Items[Index].UsedSlot != UNUSED_SLOT)
      return &Table->Items[Index].Key;
  }
  return NULL;
}

int HtElementExists(const hash_table* Table, const ht_key Key) {
  Assert(Table != NULL);
  return HtLookup(Table, Key) != NULL;
}

void HtRemoveElement(hash_table* Table, const ht_key Key) {
  Assert(Table != NULL);
  if (HtNumElements(Table) < (HtGetSize(Table) / 4)) {
    *Table = ResizeTable(Table, Table->Size / 2);
  }

  u32 Collisions = 0;
  u32 Index = LinearProbe(Table, Key, &Collisions);
  if (Index < HtGetSize(Table)) {
    Table->Items[Index].UsedSlot = UNUSED_SLOT;
    Table->Count--;

    struct item Item;
    for (; Index < HtGetSize(Table);) {
      if (Index + 1 >= HtGetSize(Table)) {
        Index = 0;
      }

      Item = Table->Items[++Index];
      if (Item.UsedSlot != UNUSED_SLOT) {
        Table->Items[Index].UsedSlot = UNUSED_SLOT;
        --Table->Count;	// We are just moving this slot, not adding a new one, so decrement the count
        HtInsertElement(Table, Item.Key, Item.Value);	// This function will increment count by 1
      }
      if (Item.UsedSlot == UNUSED_SLOT)
        break;
    }
    Assert(!HtElementExists(Table, Key));
  }
}

u32 HtGetSize(const hash_table* Table) {
  Assert(Table != NULL);
  return Table->Size;
}

u32 HtNumElements(const hash_table* Table) {
  Assert(Table != NULL);
  return Table->Count;
}

void HtFree(hash_table* Table) {
  Assert(Table != NULL);
  if (Table->Items) {
    M_Free(Table->Items, Table->Size * sizeof(struct item));
    Table->Items = NULL;
    Table->Size = 0;
    Table->Count = 0;
  }
}
