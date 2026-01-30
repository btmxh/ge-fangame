#pragma once

#include "ge-app/scenes/buzz.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/core.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace ge {

class PlayerStats {
public:
  static constexpr float MAX_FOOD = 100.0f;
  static constexpr float MAX_STAMINA = 100.0f;
  static constexpr float FOOD_THRESHOLD_FOR_HEALING =
      30.0f; // Food must be > 30% for stamina to heal
  static constexpr float STAMINA_HEAL_RATE =
      0.5f; // Stamina per second when healing
  static constexpr float FOOD_DRAIN_RATE =
      5.0f; // Food drain per second (passive)
  static constexpr float STAMINA_DRAIN_ON_TURN =
      10.0f; // Stamina drain when turning
  static constexpr u32 INITIAL_HP = 100;
  static constexpr float FOOD_PER_STAMINA = 0.2f;

  PlayerStats() : food(MAX_FOOD), stamina(MAX_STAMINA), last_angle(0.0f) {}

  void update(float dt, float current_angle, i32 boat_y, bool is_steering) {
    max_y = std::max(max_y, boat_y);
    // Passive food drain
    food = std::max(0.0f, food - FOOD_DRAIN_RATE * dt);

    // Stamina drains when steering and direction changes
    if (is_steering) {
      float angle_diff = std::abs(current_angle - last_angle);
      // Normalize angle difference to [0, PI]
      while (angle_diff > M_PI) {
        angle_diff -= 2.0f * M_PI;
      }
      angle_diff = std::abs(angle_diff);

      // Drain stamina based on how much the angle changed
      if (angle_diff > 0.1f * dt) { // Only drain if turning significantly
        float drain = STAMINA_DRAIN_ON_TURN * (angle_diff / M_PI);
        stamina = std::max(0.0f, stamina - drain);
      }
    }

    // Heal stamina if food is above threshold
    if (food > FOOD_THRESHOLD_FOR_HEALING) {
      auto old_stamina = stamina;
      stamina = std::min(MAX_STAMINA, stamina + STAMINA_HEAL_RATE * dt);
      auto healed_amount = stamina - old_stamina;
      // drain food while healing stamina, based on the amount of stamina healed
      food = std::max(0.0f, food - healed_amount * FOOD_PER_STAMINA);
    }

    last_angle = current_angle;
  }

  // Consume food to increase food bar
  void consume_fish(float fish_food_value) {
    food = std::min(MAX_FOOD, food + fish_food_value);
  }

  float get_food() const { return food; }
  float get_stamina() const { return stamina; }

  // Get food percentage (0-100)
  float get_food_percent() const { return (food / MAX_FOOD) * 100.0f; }

  // Get stamina percentage (0-100)
  float get_stamina_percent() const { return (stamina / MAX_STAMINA) * 100.0f; }

  u32 get_ship_hp() const { return hp; }
  u32 get_max_ship_hp() const { return INITIAL_HP; }

  void apply_damage(App &app, scenes::BuzzScene &scene, u32 damage) {
    if (damage == 0)
      return;
    last_taken_damage_time = app.now();
    scene.buzz_for(50);
    if (damage >= hp) {
      hp = 0;
    } else {
      hp -= damage;
    }
  }

  i64 get_last_taken_damage_time() const { return last_taken_damage_time; }

  bool is_dead() const { return hp == 0 || food <= 0; }

  i32 get_max_y() const { return max_y; }

private:
  i64 last_taken_damage_time = -1;
  u32 hp = INITIAL_HP; // TODO: change this based on
  float food;
  float stamina;
  float last_angle; // Track last angle to detect direction changes
  i32 max_y = 0;
};

} // namespace ge
