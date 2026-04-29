#ifndef GPAD_DEBUG_H
#define GPAD_DEBUG_H

// -----------------------------------------------------------------------------
// Runtime Debug Level System
// -----------------------------------------------------------------------------
// Previously, debug output was controlled by #define DEBUG scattered across
// multiple files with different values, requiring a recompile to change.
// This module replaces all of that with a single global variable that can be
// changed while the device is running — no recompile, no reflash needed.
//
// HOW TO CHANGE THE DEBUG LEVEL:
//   Via LCD menu  : open Krake Menu, navigate to "Debug Lvl", rotate to pick
//                   a value (0-3), press the encoder button to confirm.
//   Via Serial    : type d0, d1, d2, or d3 and press enter in Serial Monitor.
//
// DEBUG LEVELS:
//   0 = off      — no debug output, normal production mode
//   1 = basic    — setup info, MAC address, WiFi start, MQTT payload echo
//   2 = verbose  — RSSI values, MQTT topic verification, HAL setup trace
//   3 = trace    — SPI raw packets, per-message level and content details
//
// Higher levels always include everything from the levels below them.
// Default is 0 (off) on every boot.
// -----------------------------------------------------------------------------

extern int gpad_debug_level;

#endif
