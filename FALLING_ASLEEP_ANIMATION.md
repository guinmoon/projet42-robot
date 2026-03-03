# Falling Asleep Animation Implementation

## Overview
A realistic falling asleep animation has been added to the LuLuEyes class that simulates the natural process of eyes getting heavy and slowly closing.

## Features

### Animation Behavior
- **Duration**: 5 seconds total animation time (configurable via `fallingAsleepDuration`)
- **Progressive blinking**: Three slow, heavy blinks that get progressively slower
  - First blink: occurs at 800ms
  - Second blink: occurs at 1200ms after first blink
  - Third blink: occurs at 1800ms after second blink
- **Heavy eyelids effect**: Eye height gradually decreases to simulate drooping eyelids
  - 0-30%: Normal eye size
  - 30-60%: 90% of normal size
  - 60-85%: 70% of normal size
  - 85-100%: 50% of normal size
- **Final state**: Eyes remain fully closed after animation completes

### New Methods

#### `void anim_fallingAsleep()`
Initiates the falling asleep animation sequence.
- Automatically disables other animations (autoblinker, idle mode, flicker effects)
- Sets internal state to track animation progress
- Resets blink counters and timers

#### `void anim_wakeUp()`
Wakes up the eyes after falling asleep.
- Opens both eyes to their default size
- Resets the falling asleep state
- Can be called to interrupt the sleep animation

### New Member Variables (in eyes_drawer.hpp)

```cpp
bool fallingAsleep = 0;                    // Is the falling asleep animation active?
unsigned long fallingAsleepTimer = 0;      // Timer for animation start
int fallingAsleepDuration = 5000;          // Total animation duration in ms (5 seconds)
int fallingAsleepStage = 0;                // 0 = not started, 1 = animating, 2 = complete
int sleepBlinkCounter = 0;                 // Tracks which blink stage we're on
unsigned long sleepBlinkTimer = 0;         // Timer for individual blink timing
```

## Usage Example

```cpp
// Initialize the eyes
luluEyes.begin(128, 64, sprite);

// ... normal operation ...

// Start falling asleep animation
luluEyes.anim_fallingAsleep();

// ... after some time, wake up ...
luluEyes.anim_wakeUp();
```

## Animation Timeline

```
Time    | Progress | Action
--------|----------|----------------------------------
0ms     | 0%       | Animation starts, eyes open
800ms   | 16%      | First slow blink (close)
1000ms  | 20%      | Eyes open again
2200ms  | 44%      | Second slow blink (close)
2500ms  | 50%      | Eyes open again
4300ms  | 86%      | Third slow blink (close)
4500ms  | 90%      | Eyes open briefly
5000ms  | 100%     | Final close, animation complete
```

## Integration Notes

1. **Priority**: The falling asleep animation takes priority over other animations
   - Auto-blinker is disabled
   - Idle mode is disabled
   - Horizontal/vertical flicker effects are disabled
   - Mood effects (tired, angry, happy) are suppressed

2. **Hearts Animation**: The hearts animation and falling asleep animation are mutually exclusive

3. **State Management**: The animation uses a state machine approach:
   - Stage 0: Not sleeping
   - Stage 1: In the process of falling asleep
   - Stage 2: Asleep (eyes closed)

4. **Smooth Transitions**: The existing eye height smoothing algorithm (`(current + next) / 2`) ensures smooth transitions during the animation

## Customization

You can customize the animation by modifying these parameters in the header file:

```cpp
int fallingAsleepDuration = 5000;  // Change animation duration
```

For more control, you can modify the timing thresholds in the `drawEyes()` method in `eyes_drawer.cpp`.

## Technical Implementation

The animation is implemented in the `drawEyes()` method and uses:
- Progress-based timing (0.0 to 1.0)
- State tracking via `sleepBlinkCounter`
- Gradual height reduction to simulate heavy eyelids
- Integration with existing eye drawing infrastructure

## Files Modified

1. **include/eyes_drawer.hpp**: Added new member variables and method declarations
2. **src/eyes_drawer.cpp**: Added implementation of animation methods and animation logic in drawEyes()