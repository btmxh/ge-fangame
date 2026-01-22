#pragma once

#include "ge-hal/core.hpp"
#include <cstring>

namespace ge {

enum class FishRarity { Common, Uncommon, Rare, Legendary };

struct FishItem {
  const char *name;
  FishRarity rarity;
  i64 caught_time; // Timestamp when caught (in milliseconds)

  FishItem() : name(nullptr), rarity(FishRarity::Common), caught_time(0) {}

  FishItem(const char *name, FishRarity rarity, i64 caught_time)
      : name(name), rarity(rarity), caught_time(caught_time) {}

  bool is_empty() const { return name == nullptr; }
};

class Inventory {
public:
  static constexpr u32 MAX_ITEMS = 50; // Maximum inventory size

  Inventory() : item_count(0) {}

  // Add a fish to the inventory
  bool add_fish(const char *name, FishRarity rarity, i64 caught_time) {
    if (item_count >= MAX_ITEMS) {
      return false; // Inventory full
    }

    items[item_count] = FishItem(name, rarity, caught_time);
    item_count++;
    return true;
  }

  // Get the number of items in inventory
  u32 get_item_count() const { return item_count; }

  // Get item at index
  const FishItem &get_item(u32 index) const {
    static const FishItem empty_item;
    if (index >= item_count) {
      return empty_item;
    }
    return items[index];
  }

  // Clear all items from inventory
  void clear() { item_count = 0; }

  // Check if inventory is full
  bool is_full() const { return item_count >= MAX_ITEMS; }

  // Get total count by rarity
  u32 get_count_by_rarity(FishRarity rarity) const {
    u32 count = 0;
    for (u32 i = 0; i < item_count; i++) {
      if (items[i].rarity == rarity) {
        count++;
      }
    }
    return count;
  }

private:
  FishItem items[MAX_ITEMS];
  u32 item_count;
};

} // namespace ge
