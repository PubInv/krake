#ifndef GPAD_DEBUG_H
#define GPAD_DEBUG_H

// Runtime debug level — can be changed via LCD menu or serial command (d0-d3).
//   0 = off
//   1 = basic   (setup info, key events)
//   2 = verbose (detailed message flow)
//   3 = trace   (inside functions, SPI, per-loop)
extern int gpad_debug_level;

#endif
