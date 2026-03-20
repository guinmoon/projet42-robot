# 🤖 Project 42 - A Cute ESP32 Robot Companion

![Project 42](https://img.shields.io/badge/Project-42-blue.svg)
![Cute Robot](https://img.shields.io/badge/Type-Cute%20Robot-green.svg)
![ESP32 Powered](https://img.shields.io/badge/Powered%20by-ESP32-orange.svg)

> **Meet Project 42** - A charming, interactive robotic companion powered by ESP32, featuring expressive animated eyes, gesture recognition, and a heart full of digital warmth. 💕

---

## 🌟 About Project 42

**Project 42** is not just another embedded system - it's a **cute robot friend** designed to bring joy and companionship through technology! 

This adorable ESP32-powered robot features:
- ✨ **Expressive animated eyes** that can fall asleep, wake up, and show emotions
- 👋 **Gesture recognition** - responds to your hand movements
- 🎭 **Interactive personality** - reacts to your presence
- 🔄 **Servo-controlled movements** - physical expressions
- 📱 **Web interface** - connect and interact remotely

---

## 🎨 What Makes Project 42 Special?

### 💜 Personality & Behavior

Project 42 has been designed with **personality in mind**:

- **Falling Asleep Animation**: Watch Project 42's eyes slowly get heavy and close in a realistic 5-second sequence with progressive blinking
- **Wake Up Sequence**: Gently open its eyes when it's time to interact again
- **Attention Detection**: Knows when you're nearby and responds accordingly
- **Idle Behaviors**: Natural eye movements when not actively engaged

### 🧠 Smart Features

| Feature | Description |
|---------|-------------|
| **Dual Distance Sensors** | VL53L0X sensors detect your presence from both sides |
| **Gesture Recognition** | Short & long attention detection for different interactions |
| **Power Modes** | Sleeps when needed, wakes up when you're around |
| **Memory Management** | Smart use of ESP32's PSRAM for smooth animations |

---

## 🎭 Animation Showcase

### The Falling Asleep Animation 😴

One of Project 42's most endearing features is its **falling asleep animation**:

```
👀 0ms    → Eyes wide open, alert and happy
👁️ 800ms  → First slow, heavy blink...
👁️ 2200ms → Second blink, eyelids getting heavier...
👁️ 4300ms → Final slow blink...
😴 5000ms  → Eyes gently close, sweet dreams!
```

**Watch the progression:**
- **0-30%**: Normal bright eyes ✨
- **30-60%**: 90% size - getting sleepy... 💤
- **60-85%**: 70% size - so heavy... 🌙
- **85-100%**: 50% size - almost there... 😴

---

## 🔧 Hardware Components

### What's Inside Project 42?

```
┌─────────────────────────────────┐
│        🤖 PROJECT 42 🤖         │
│                                 │
│   👁️     [TFT Display]     👁️  │  ← Expressive eyes
│                                 │
│   [VL53L0X]      [VL53L0X]      │  ← Distance sensors
│                                 │
│   [ESP32 Board]                 │  ← The brain!
│                                 │
│   [Servo Motors]                │  ← Movement & expression
│                                 │
│   [Web Server]                  │  ← Remote connection
└─────────────────────────────────┘
```

### Required Parts

- 🧠 **ESP32 DevKit V1** - The smart brain
- 👁️ **240x240 TFT Display** - For the cute face
- 📏 **2x VL53L0X Sensors** - To sense your presence
- 🎪 **Servo Motors** - For physical movements
- 🔌 **5V Power Supply** - Energy for cuteness

---

## 🚀 Getting Started with Project 42

### Step 1: Install PlatformIO

```bash
# Install VS Code + PlatformIO Extension
# Then clone Project 42
git clone <your-repository-url>
cd proj42
```

### Step 2: Build & Upload

```bash
# Let's bring Project 42 to life!
platformio run -t upload
```

### Step 3: Watch It Come Alive!

Connect the serial monitor and watch Project 42 say hello:

```bash
platformio device monitor
```

You'll see:
```
alive
[Project 42 initializes...]
[Display starts up...]
[Sensors calibrating...]
[Ready to interact!]
```

---

## 💡 How to Interact with Project 42

### Hand Gestures

| Action | Project 42's Response |
|--------|----------------------|
| **Wave nearby** (short attention) | 👋 Acknowledges your presence |
| **Stay nearby** (long attention) | 💕 Shows affection, may display hearts |
| **Move away** | 😢 Eyes follow you, waiting |
| **Return** | 😊 Happy to see you again! |

### Power Modes

Project 42 can adjust its energy levels:

```cpp
// When you want Project 42 to rest
proj42.LowPowMode();      // 80MHz - Nap time 😴

// Normal interaction mode
proj42.NormalPowMode();   // 160MHz - Ready to play! 🎮

// Maximum excitement
proj42.HighPowMode();     // 240MHz - Super active! ⚡
```

---

## 🎯 Project 42's Abilities

### Display & Animation
- ✅ Animated eye rendering with smooth transitions
- ✅ Natural blinking behavior
- ✅ Falling asleep animation (5 seconds of cuteness)
- ✅ Wake up sequence
- ✅ Mood expressions (tired, happy, excited)

### Sensing & Interaction
- ✅ Dual-side presence detection
- ✅ Gesture recognition (short/long attention)
- ✅ Sliding window averaging for stable readings
- ✅ Automatic response to hand movements

### Movement & Control
- ✅ Servo motor control for physical expression
- ✅ Multi-core processing for smooth operation
- ✅ Web server for remote interaction

---

## 📖 Technical Documentation

For developers who want to understand Project 42's inner workings:

- **[Main README](README.md)** - Complete technical documentation
- **[Falling Asleep Animation](FALLING_ASLEEP_ANIMATION.md)** - How the cute sleep animation works
- **[Sensor Refactoring](SENSOR_REFACTORING.md)** - Architecture of gesture recognition

---

## 🛠️ Customization

### Make Project 42 Your Own!

You can customize various aspects:

```cpp
// In eyes_drawer.hpp, adjust the sleep duration:
int fallingAsleepDuration = 5000;  // Change how long it takes to fall asleep

// Adjust animation timing in drawEyes() method
// Modify blink patterns and eyelid behavior
```

### Display Configuration

To enable the TFT display, edit `platformio.ini`:

```ini
lib_deps = 
    bodmer/TFT_eSPI@^2.5.31

build_flags = 
    -D USER_SETUP_LOADED=1
    -D ST7789_2_DRIVER=1
    -D TFT_WIDTH=240
    -D TFT_HEIGHT=240
```

---

## 🌈 Future Features (Coming Soon!)

We're working on making Project 42 even cuter:

- 🎵 **Audio responses** - Woofs and happy sounds
- 🧭 **Gyro support** - Head tilting and nodding
- 👆 **Touch interaction** - Pet Project 42!
- 🌐 **Enhanced web interface** - More ways to interact
- 🎭 **More emotions** - Sad, excited, curious...

---

## 🤝 Community & Support

### Join the Project 42 Family!

- 📝 **Report bugs**: Open an issue
- 💡 **Suggest features**: We love ideas!
- 🎨 **Share your builds**: Show us your Project 42!
- ❤️ **Spread the love**: Star this repository!

---

## 📸 Photo Gallery

*Coming soon - Share your Project 42 photos in the issues!*

---

## ⚠️ Important Notes

- Project 42 is still in development - some features may be experimental
- Always handle electronics with care
- Ensure proper power supply to avoid brownouts
- Some features require uncommenting code sections

---

## 📝 License

This project is brought to you with ❤️ - see the [LICENSE](LICENSE) file for details.

---

## 💌 A Note from the Creator

> "Project 42 was created with the belief that technology can be warm, friendly, and even cute. This robot isn't just about code and hardware - it's about creating a connection between humans and machines through expressive, natural behaviors. Whether it's falling asleep with heavy eyelids or waking up to greet you, every animation is designed to feel authentic and endearing. Thank you for being part of Project 42's journey!" 🌟

---

## 🔗 Quick Links

- **[Project Repository](#)** - Source code and updates
- **[PlatformIO Docs](https://docs.platformio.org/)** - Build system documentation
- **[ESP32 Resources](https://www.espressif.com/)** - Hardware information
- **[TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)** - Display driver

---

<div align="center">

### 🤖 Made with ❤️ and ESP32 🤖

**Project 42** - Because the world needs more cute robots!

⭐ **Star this repo if Project 42 made you smile!** ⭐

</div>
