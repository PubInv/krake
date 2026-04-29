# DFPlayer.cpp — Full Code Walkthrough

Personal notes for understanding the DFPlayer module code in GPAD_API.
File: `Firmware/GPAD_API/GPAD_API/DFPlayer.cpp`
Header: `Firmware/GPAD_API/GPAD_API/DFPlayer.h`

---

## What is the DFPlayer Mini?

The DFPlayer Mini is a small MP3 player module. It:
- Has its own SD card slot (you put audio files on it)
- Communicates with the ESP32 over UART (serial)
- Has a BUSY pin that goes LOW while a track is playing, HIGH when idle
- Is powered separately and plays audio through a small speaker

The ESP32 sends commands like "play track 3" or "set volume to 20" over serial,
and the DFPlayer executes them. It can also send back status messages (card inserted,
playback finished, errors, etc.).

There is also a known clone called the TD5580 that looks identical but behaves
differently and will block the firmware if you send it play commands.

---

## Hardware Connections (from DFPlayer.h)

```
ESP32 GPIO 17 (TXD2)  →  DFPlayer RX
ESP32 GPIO 16 (RXD2)  →  DFPlayer TX
ESP32 GPIO 4           →  DFPlayer BUSY pin (LOW = playing, HIGH = idle)
UART baud rate: 9600
```

The BUSY pin is read with `digitalRead(nDFPlayer_BUSY)`.
- `HIGH` means the module is free → safe to send a play command
- `LOW`  means it is currently playing → wait before sending another command

---

## Global Variables (top of DFPlayer.cpp)

```cpp
DFRobotDFPlayerMini dfPlayer;       // The library object. All commands go through this.
HardwareSerial mySerial1(2);        // UART2 on the ESP32. This is the serial line to the module.
const int nDFPlayer_BUSY = 4;       // GPIO pin connected to the DFPlayer BUSY output.
bool isDFPlayerDetected = false;    // Flag: true only if a real, working module was confirmed.
int volumeDFPlayer = 20;            // Starting volume (0–30). Set once during setup.
int numberFilesDF = 0;              // How many audio files are on the SD card. Read during setup.
char command;                       // Stores a single character typed in Serial Monitor.
int pausa = 0;                      // Toggle flag for pause (0 = playing, 1 = paused).
```

`isDFPlayerDetected` is the most important flag in the whole file.
Every playback function checks it first. If false, they return immediately
without touching the DFPlayer library — this prevents blocking.

---

## Function by Function

---

### serialSplashDFP()

Prints firmware version and compile date to Serial Monitor.
Purely informational — called once at startup so you can see what version is running.
Has nothing to do with audio.

---

### menu_opcoes()

Prints a help menu to Serial Monitor showing keyboard commands you can type.
Called after any serial command is handled so you always see the options again.
Also purely informational — no audio logic here.

---

### checkSerial()

**Purpose:** Let you control the DFPlayer by typing in Serial Monitor during development/testing.

**How it works:**
1. Checks if any character was typed (`Serial.available() > 0`)
2. Reads one character at a time
3. Decides what to do based on the character:

| Key | Action |
|-----|--------|
| 1–9 | Play that track number (`command - 48` converts ASCII digit to integer) |
| s   | Stop playback |
| p   | Toggle pause/resume |
| +   | Volume up |
| -   | Volume down |
| >   | Next track |
| <   | Previous track |

This function does NOT guard on `isDFPlayerDetected` — it is a debug/test utility,
not called in the main alarm flow.

---

### setupDFPlayer() — THE MOST IMPORTANT FUNCTION

Called once at boot from the main `setup()`. It initialises the UART, confirms the
module is real, identifies clones, and plays a startup sound.

**Step-by-step flow:**

#### Step 1 — Configure BUSY pin
```cpp
pinMode(nDFPlayer_BUSY, INPUT_PULLUP);
```
Sets GPIO4 as input with internal pull-up resistor.
Pull-up means the pin reads HIGH by default even when nothing is connected.
When the DFPlayer is playing it pulls this LOW.

#### Step 2 — Start UART2
```cpp
mySerial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
while (!mySerial1) { delay(100); }
```
Opens the serial connection on UART2 at 9600 baud.
The `while (!mySerial1)` loop waits until the serial port is ready.
On ESP32 with HardwareSerial this is almost instant — the loop is a safety net.

#### Step 3 — begin() handshake
```cpp
if (!dfPlayer.begin(mySerial1, true, false))
{
    isDFPlayerDetected = false;
    return;  // EXIT EARLY — no module
}
```
`dfPlayer.begin(stream, isACK, doReset)`:
- `isACK = true` — the library sends a command and waits for the module to reply (ACK).
  If no reply comes within the timeout, begin() returns false.
- `doReset = false` — do not send a hardware reset command first.

If `begin()` returns false → no module → set flag → return immediately.
Without the `return`, the code would fall into play commands on a missing module
and block for seconds (this was bug #460).

If `begin()` returns true → set `isDFPlayerDetected = true` and continue.
BUT: on floating UART lines (module physically absent but ESP32 boot noise on the line),
`begin()` can incorrectly return true. This is a "false positive". The next step handles it.

#### Step 4 — Secondary validation with readState()
```cpp
dfPlayer.setTimeOut(500);
delay(100);
int moduleState = dfPlayer.readState();
```
`setTimeOut(500)` — any read command that gets no reply within 500ms returns -1.

`readState()` asks the module "what are you currently doing?".
A real DFPlayer Mini returns 0 when stopped/idle.

Three outcomes:

| moduleState | Meaning | Action |
|-------------|---------|--------|
| < 0 | No reply (timeout) — begin() was a false positive, UART line noise | `isDFPlayerDetected = false` → return |
| = 0 | Genuine DFPlayer Mini, stopped and ready | Continue with setup |
| > 0 | TD5580 clone — returns a non-zero state at idle | `isDFPlayerDetected = false` → return |

This two-step detection (begin + readState) is necessary because begin() alone is not reliable.

#### Step 5 — Set volume and play startup sound
Only reached if a genuine module is confirmed.

```cpp
dfPlayer.volume(20);     // Set volume to 20 out of 30
dfPlayer.start();        // Resume from wherever it was (safety call)
delay(1000);
dfPlayer.play(9);        // Play track 9 (startup sound)
delay(100);
dfPlayer.stop();
delay(1000);
dfPlayer.previous();     // Go back to track before 9
delay(1500);
dfPlayer.play();         // Play that track (the actual splash)
displayDFPlayerStats();  // Print SD card info to serial
```

The delays here are intentional — the DFPlayer needs time between commands.
If you fire commands too fast, it ignores them.

---

### displayDFPlayerStats()

Queries the module over UART and prints to Serial Monitor:
- Current state (playing/stopped)
- Current volume level
- EQ setting
- Total number of files on SD card (stored in `numberFilesDF`)
- Current track number
- Number of files in subfolder 03

This is diagnostic only. Called once after setup and at any time you want to inspect the module.
Note: `dfPlayer.EQ(0)` is commented out because it causes a lockup — left as a warning.

---

### setVolume(int zeroToThirty)

Simple wrapper. Lets other parts of the firmware set the volume without directly
calling the library. Range 0–30.

---

### printDetail(uint8_t type, int value)

The DFPlayer module sends back event notifications spontaneously over UART:
- SD card inserted or removed
- Playback finished on a track number
- USB inserted/removed
- Various errors

`dfPlayer.readType()` tells you what kind of event it was.
`dfPlayer.read()` gives you the associated value (e.g. the track number).

This function is a decoder — it takes those two values and prints a human-readable
message. It is called anywhere `dfPlayer.available()` returns true (meaning the
module sent something).

---

### dfPlayerUpdate()

**Purpose:** Called repeatedly from the main loop. Handles ongoing DFPlayer activity.

```cpp
void dfPlayerUpdate(void)
{
    // Every 3 seconds, play the next track (demo/test cycling behaviour)
    static unsigned long timer = millis();
    if (millis() - timer > 3000)
    {
        timer = millis();
        dfPlayer.next();
    }

    // Check if the module sent any event (finished playing, error, card event)
    if (dfPlayer.available())
    {
        printDetail(dfPlayer.readType(), dfPlayer.read());
    }
}
```

`static unsigned long timer` — the `static` keyword means this variable is created
once and retains its value between calls. It is not reset every time the function runs.
This is how you do non-blocking timing in Arduino/ESP32 — compare against `millis()`
instead of using `delay()`.

Note: On the `bugfix/460` branch this function does NOT have an `isDFPlayerDetected` guard yet.
On the `bugfix/43` branch it does. This is a known gap.

---

### playNotBusy()

Plays the next track but only if the BUSY pin is HIGH (module is not currently playing).

```cpp
if (HIGH == digitalRead(nDFPlayer_BUSY))
{
    dfPlayer.next();
}
```

After issuing the command it checks for any incoming events from the module.
Then `delay(1000)` — this is acknowledged in the code comments as something that
should be replaced with interrupt-based detection of the BUSY pin going HIGH.
Right now it just blindly waits 1 second.

---

### playNotBusyLevel(int level)

Same pattern as playNotBusy but plays a specific track by number (`level + 1`).
The +1 is because alarm levels start at 0 but SD card tracks start at 1.

Comment in code notes that using track numbers directly is fragile — ideally
you would map alarm names to file names. Left as future work.

---

### playAlarmLevel(int alarmNumberToPlay)

**Purpose:** The main function called when Krake needs to play an alarm sound.
Returns `true` if a play command was sent, `false` if it skipped (busy or too soon).

```cpp
static unsigned long timer = millis();
const unsigned long delayPlayLevel = 20;

if (millis() - timer > delayPlayLevel)   // Only act if 20ms have passed since last call
{
    timer = millis();

    // Validate track number is in range
    if ((0 > tracNumber < 0) || (numberFilesDF < tracNumber))
        return false;

    // Only play if module is not busy
    if (HIGH == digitalRead(nDFPlayer_BUSY))
        dfPlayer.play(tracNumber);
    else
        Serial.println("Not done playing previous file");

    // Check for incoming events
    if (dfPlayer.available())
        printDetail(dfPlayer.readType(), dfPlayer.read());

    return true;
}
return false;
```

The 20ms rate limit prevents hammering the DFPlayer with commands if the main loop
calls this function very frequently.

Note: `(0 > tracNumber < 0)` is a C++ bug — this expression is always false due to
how chained comparisons work in C. It does not actually validate the lower bound.
Left as-is since it does not cause harm, just doesn't protect.

---

## Event Flow Diagrams

### Boot with genuine DFPlayer connected

```
main setup()
    └── setupDFPlayer()
            ├── pinMode(BUSY, INPUT_PULLUP)
            ├── mySerial1.begin(9600)         → UART2 open
            ├── dfPlayer.begin()              → sends command, waits for ACK
            │       └── module replies        → returns true
            ├── isDFPlayerDetected = true
            ├── dfPlayer.setTimeOut(500)
            ├── delay(100)
            ├── dfPlayer.readState()          → module returns 0 (stopped)
            │       moduleState == 0          → genuine module, continue
            ├── dfPlayer.volume(20)
            ├── dfPlayer.start()
            ├── delay(1000)
            ├── dfPlayer.play(9)              → startup sound plays
            ├── delay(100) → stop → delay(1000) → previous → delay(1500) → play
            └── displayDFPlayerStats()        → prints SD card info

main loop() repeating forever
    └── dfPlayerUpdate()
            ├── every 3s: dfPlayer.next()
            └── if module sent event: printDetail()
```

### Boot with NO DFPlayer connected (path 1 — begin() correctly returns false)

```
setupDFPlayer()
    ├── mySerial1.begin(9600)
    ├── dfPlayer.begin()       → no reply from module → returns false
    ├── isDFPlayerDetected = false
    └── return                 → exits immediately, no blocking
```

### Boot with NO DFPlayer connected (path 2 — begin() false positive from UART noise)

```
setupDFPlayer()
    ├── mySerial1.begin(9600)
    ├── dfPlayer.begin()       → noise on floating UART line → returns true (false positive!)
    ├── isDFPlayerDetected = true   ← wrongly set
    ├── dfPlayer.setTimeOut(500)
    ├── delay(100)
    ├── dfPlayer.readState()   → no real module → times out → returns -1
    │       moduleState < 0    → treat as absent
    ├── isDFPlayerDetected = false
    └── return                 → exits cleanly, no blocking
```

### Boot with TD5580 clone connected

```
setupDFPlayer()
    ├── dfPlayer.begin()       → clone responds → returns true
    ├── isDFPlayerDetected = true
    ├── dfPlayer.readState()   → clone returns non-zero at idle → moduleState > 0
    ├── prints: "TD5580 clone detected"
    ├── isDFPlayerDetected = false
    └── return                 → exits cleanly, clone never receives play commands
```

### Alarm triggered during operation

```
alarm received (MQTT or button)
    └── playAlarmLevel(alarmNumber)
            ├── isDFPlayerDetected == false?  → return false immediately (no module)
            ├── 20ms since last call?         → no → return false
            ├── track number valid?           → no → return false
            ├── BUSY pin HIGH?                → yes → dfPlayer.play(alarmNumber)
            │                                 → no  → "Not done playing previous file"
            └── return true
```

### SD card inserted during operation

```
DFPlayer module detects card insertion
    → sends spontaneous event over UART to ESP32

dfPlayerUpdate() running in main loop
    ├── dfPlayer.available() returns true   ← module sent something
    ├── dfPlayer.readType()                 ← what kind of event? DFPlayerCardInserted
    ├── dfPlayer.read()                     ← associated value
    └── printDetail()                       ← prints "Card Inserted!" to Serial Monitor
```

---

## Key Concepts to Remember

**isDFPlayerDetected is the gatekeeper.**
Every function that sends a command to the module should check this first.
If false, return immediately. This is what prevents the firmware from blocking
when the module is absent or broken.

**UART can lie.**
When the DFPlayer is physically absent, the UART2 RX line floats.
ESP32 boot activity or electrical noise can look like valid serial data.
`dfPlayer.begin()` can return true even with nothing connected.
Always do a secondary check (readState) to confirm.

**Blocking is the enemy.**
The ESP32 runs WiFi, MQTT, the rotary encoder, the LCD, and audio all in one loop.
Any `delay()` or library call that waits for a reply without a timeout can freeze everything.
This is the root cause of both #460 (boot block) and #43 (runtime block).

**The BUSY pin is hardware, not software.**
It is a physical signal from the DFPlayer chip directly.
More reliable than asking the module over UART if it is playing.
If BUSY is LOW, do not send a play command — the module will ignore it or queue badly.

**Track numbers on SD card start at 1, not 0.**
Alarm levels in the firmware start at 0. Hence `dfPlayer.play(level + 1)` in playNotBusyLevel.
