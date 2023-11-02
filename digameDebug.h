#ifndef __DIGAME_DEBUG_H__
#define __DIGAME_DEBUG_H__


// Show debug messages.
#define SHOW_DEBUG

// Define where debug output will be printed.
#define DEBUG_UART Serial

// Setup debug printing macros.
#ifdef SHOW_DEBUG
  #define DEBUG_PRINT(...) { DEBUG_UART.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_UART.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif


#endif  // __DIGAME_DEBUG_H__