#pragma once

#include "ge-hal/core.hpp"

namespace ge {

enum class FishRarity { Common, Uncommon, Rare, Legendary };

struct FishItem {
  const char *name;
  FishRarity rarity;
  i64 caught_time; // Timestamp when caught (in milliseconds)
  float weight;    // Weight in kg for cargo calculation

  FishItem()
      : name(nullptr), rarity(FishRarity::Common), caught_time(0),
        weight(0.0f) {}

  FishItem(const char *name, FishRarity rarity, i64 caught_time, float weight)
      : name(name), rarity(rarity), caught_time(caught_time), weight(weight) {}

  bool is_empty() const { return name == nullptr; }
};

class Inventory {
public:
  static constexpr u32 MAX_ITEMS = 50; // Maximum inventory size

  Inventory() : item_count(0) {}
  Inventory(const Inventory &other) = delete;
  Inventory &operator=(const Inventory &other) = delete;

  // Add a fish to the inventory
  bool add_fish(const char *name, FishRarity rarity, i64 caught_time,
                float weight) {
    if (item_count >= MAX_ITEMS) {
      return false; // Inventory full
    }

    items[item_count] = FishItem(name, rarity, caught_time, weight);
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

  // Get total cargo weight in kg
  float get_total_weight() const {
    float total = 0.0f;
    for (u32 i = 0; i < item_count; i++) {
      total += items[i].weight;
    }
    return total;
  }

  // Remove fish at index (for consuming)
  bool remove_fish(u32 index) {
    if (index >= item_count) {
      return false;
    }
    // Shift items down
    for (u32 i = index; i < item_count - 1; i++) {
      items[i] = items[i + 1];
    }
    item_count--;
    return true;
  }

private:
  FishItem items[MAX_ITEMS];
  u32 item_count;
};

} // namespace ge
