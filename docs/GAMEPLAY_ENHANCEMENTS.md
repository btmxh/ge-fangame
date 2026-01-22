# Gameplay Enhancement Updates

This document describes the additional gameplay mechanics implemented based on user feedback.

## Overview

Three major enhancements were added to improve gameplay depth and challenge:
1. Ultra-rare Golden Fish
2. Ship acceleration system with weight mechanics
3. Food and stamina resource management

---

## 1. Golden Fish Rarity Enhancement

### Changes
- **Previous**: Rare rarity, weight 5 out of ~127 total
- **Current**: Legendary rarity, weight 1 out of ~130,001 total

### Probability
- **Catch rate**: Approximately **1 in 130,000 catches**
- Makes the Golden Fish an extremely valuable and memorable catch
- Other fish weights scaled up proportionally to maintain balance

### Fish Distribution Table

| Fish | Rarity | Weight | Probability | Physical Weight |
|------|--------|--------|-------------|-----------------|
| Tropical Fish | Common | 30,000 | ~23% | 0.5 kg |
| Sea Bass | Common | 25,000 | ~19% | 2.0 kg |
| Sardine | Common | 20,000 | ~15% | 0.3 kg |
| Tuna | Uncommon | 15,000 | ~11.5% | 10.0 kg |
| Salmon | Uncommon | 15,000 | ~11.5% | 5.0 kg |
| Pufferfish | Uncommon | 10,000 | ~7.7% | 1.5 kg |
| Clownfish | Rare | 8,000 | ~6.2% | 0.2 kg |
| Old Boot | Uncommon | 7,000 | ~5.4% | 3.0 kg |
| Treasure Chest | Legendary | 2,000 | ~1.5% | 25.0 kg |
| **Golden Fish** | **Legendary** | **1** | **~0.0008%** | **0.1 kg** |

**Total Weight**: ~130,001

---

## 2. Ship Acceleration System

### Controls
- **Button A (Hold)**: Accelerate in Steering mode
- **Button A (Release)**: Decelerate back to base speed

### Mechanics

#### Speed Calculation
```
effective_base_speed = base_speed * (ship_weight / total_weight)
current_speed = effective_base_speed * acceleration_multiplier

where:
  base_speed = 30 m/s
  ship_weight = 100 kg (constant)
  total_weight = ship_weight + cargo_weight
  acceleration_multiplier = 1.0 to 2.5 (when accelerating)
```

#### Weight Impact
- **Light cargo** (< 50 kg): Minimal speed reduction, fast acceleration
- **Medium cargo** (50-200 kg): Moderate speed reduction
- **Heavy cargo** (> 200 kg): Significant speed reduction, slower max speed

#### Example Speeds

| Cargo Weight | Base Speed | Max Accel Speed | Speed Reduction |
|--------------|------------|-----------------|-----------------|
| 0 kg | 30.0 m/s | 75.0 m/s | 0% |
| 50 kg | 20.0 m/s | 50.0 m/s | 33% |
| 100 kg | 15.0 m/s | 37.5 m/s | 50% |
| 200 kg | 10.0 m/s | 25.0 m/s | 67% |
| 400 kg | 6.0 m/s | 15.0 m/s | 80% |

#### Acceleration Parameters
- **Acceleration rate**: 60 m/s² (speed increases by 60 m/s per second)
- **Deceleration rate**: 40 m/s² (speed decreases by 40 m/s per second)
- **Max multiplier**: 2.5x (when fully accelerated)
- **Transition**: Smooth, gradual speed changes

### Gameplay Impact
- **Risk/Reward**: Carrying heavy cargo (treasure chests) significantly reduces mobility
- **Strategic choices**: Players must balance cargo value vs. maneuverability
- **Fuel efficiency**: (Future) Could tie acceleration to stamina drain
- **Exploration**: Faster travel when inventory is light

---

## 3. Food and Stamina System

### Resource Bars

#### Food Bar
- **Maximum**: 100 units
- **Color**: Yellow (0xFBE0)
- **Passive drain**: -1 unit/second
- **Restoration**: Consume fish (+weight × 20 food)
- **Purpose**: Required to maintain stamina healing

#### Stamina Bar
- **Maximum**: 100 units
- **Color**: Cyan (0x07FF)
- **Drain**: When steering and turning (proportional to angle change)
- **Heal rate**: +5 units/second (only when food > 30)
- **Purpose**: Resource consumed during active steering

### Mechanics

#### Food Consumption
Press **Button A** in inventory to eat selected fish:

**Edible Fish:**
| Fish | Weight | Food Value |
|------|--------|------------|
| Tropical Fish | 0.5 kg | 10 food |
| Sea Bass | 2.0 kg | 40 food |
| Sardine | 0.3 kg | 6 food |
| Tuna | 10.0 kg | 200 food |
| Salmon | 5.0 kg | 100 food |
| Clownfish | 0.2 kg | 4 food |
| Golden Fish | 0.1 kg | 2 food |

**Non-Edible Items:**
- **Pufferfish**: Poisonous (1.5 kg)
- **Old Boot**: Not food (3.0 kg)
- **Treasure Chest**: Not food (25.0 kg)

#### Stamina Drain Formula
```
stamina_drain = STAMINA_DRAIN_ON_TURN * (angle_change / π) * dt

where:
  STAMINA_DRAIN_ON_TURN = 2.0
  angle_change = absolute difference from previous angle
  dt = delta time (seconds)
```

#### Healing Conditions
Stamina heals **only when**:
1. Food > 30 units (30% threshold)
2. Not actively draining (minimal turning)
3. Heal rate: 5 units/second

### Visual Representation

Status bars displayed on **Status Screen**:
```
Status:
  Food: 75%
  [████████████████░░░░] (yellow bar)

  Stamina: 60%
  [████████████░░░░░░░░] (cyan bar)
```

### Gameplay Loop

1. **Catch fish** → Gain cargo weight + potential food
2. **Navigate/steer** → Drain stamina (more when turning)
3. **Low stamina?** → Need food > 30% to heal
4. **Low food?** → Consume fish from inventory
5. **Strategic decision**: Eat valuable fish vs. keep for sale/collection

### Strategic Depth

**Resource Management:**
- Keep food above 30% for stamina regen
- Save heavy fish (tuna, salmon) for emergency food
- Balance collection goals vs. survival needs

**Inventory Decisions:**
- Eat common fish (sardines, tropical) first
- Save rare fish unless desperate
- Pufferfish is trap (poisonous, can't eat)

**Weight Trade-offs:**
- Heavy food = better restoration but slower ship
- Light snacks = less food but maintain speed
- Treasure chests add weight without food value

---

## Implementation Details

### New Components

**PlayerStats Class** (`player_stats.hpp`)
- Tracks food and stamina values
- Update method called each frame
- Consumption method for eating fish
- Percentage getters for UI display

**Modified Components:**
1. **Boat** - Acceleration system, weight-based speed
2. **Inventory** - Weight tracking, fish removal
3. **GameScene** - PlayerStats integration, acceleration controls
4. **StatusScene** - Visual status bars
5. **InventoryScene** - Fish consumption interface

### Constants Reference

```cpp
// PlayerStats
MAX_FOOD = 100.0f
MAX_STAMINA = 100.0f
FOOD_THRESHOLD_FOR_HEALING = 30.0f
STAMINA_HEAL_RATE = 5.0f
FOOD_DRAIN_RATE = 1.0f
STAMINA_DRAIN_ON_TURN = 2.0f

// Boat
ship_weight = 100.0f kg
boat_speed = 30.0f m/s
acceleration_rate = 60.0f m/s²
deceleration_rate = 40.0f m/s²
max_accel_multiplier = 2.5x
```

---

## Testing Recommendations

### Golden Fish Rarity
- [ ] Catch 1000+ fish, verify Golden Fish appears ~0-1 times
- [ ] Verify it shows as Legendary rarity in inventory
- [ ] Check weight is 0.1 kg

### Ship Acceleration
- [ ] Empty inventory: verify max speed ~75 m/s
- [ ] Catch 50kg cargo: verify speed reduction
- [ ] Catch treasure chest: verify significant slowdown
- [ ] Hold/release Button A: verify smooth acceleration/deceleration

### Food & Stamina
- [ ] Verify food drains over time
- [ ] Steer with turns: verify stamina drains
- [ ] Keep food > 30%: verify stamina heals
- [ ] Food < 30%: verify stamina doesn't heal
- [ ] Eat fish: verify food increases by (weight × 20)
- [ ] Try eating pufferfish: verify it doesn't work
- [ ] Status bars: verify visual feedback matches values

### Edge Cases
- [ ] Inventory full of treasure chests: verify severe speed reduction
- [ ] Zero food: verify no stamina healing
- [ ] Zero stamina: (currently no penalty, future: can't steer?)
- [ ] Eat last fish: verify inventory adjusts correctly

---

## Future Enhancements

1. **Stamina Penalties**: Unable to steer at 0 stamina
2. **Food Spoilage**: Fish decay over time
3. **Cooking System**: Improve food value by processing
4. **Fuel System**: Acceleration consumes fuel/stamina
5. **Fishing Energy**: Cost stamina to cast/reel
6. **Weather Effects**: Storms drain more stamina
7. **Resting Mechanic**: Sleep to restore both bars fully
8. **Rare Fish Events**: Golden Fish provides buff/bonus

---

## Balance Notes

### Current Balance
- **Food drain**: 100 food lasts ~100 seconds (1:40)
- **Stamina heal**: 100 stamina in 20 seconds (if food > 30)
- **Stamina drain**: Heavy turning can drain ~10/second
- **Fish values**: Small fish = snacks, large fish = meals

### Tuning Suggestions
If gameplay feels too harsh:
- Reduce FOOD_DRAIN_RATE to 0.5
- Reduce STAMINA_DRAIN_ON_TURN to 1.5
- Increase STAMINA_HEAL_RATE to 7.5

If gameplay feels too easy:
- Increase FOOD_DRAIN_RATE to 1.5
- Increase STAMINA_DRAIN_ON_TURN to 3.0
- Decrease STAMINA_HEAL_RATE to 3.0

---

## Conclusion

These enhancements add significant depth to the gameplay:
- **Golden Fish** provides long-term collection goal
- **Acceleration** creates weight-based tactical decisions
- **Food/Stamina** introduces resource management layer

The systems interact naturally:
- Catching fish adds weight (slows ship)
- Eating fish removes weight (speeds up ship)
- Heavy cargo requires more careful navigation (stamina management)

Players must now balance exploration, collection, and resource management for a richer gameplay experience.
