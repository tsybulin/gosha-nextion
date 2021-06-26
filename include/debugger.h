#ifndef DEBUGGER_H
#define DEBUGGER_H

// #define NEXX_DEBUG_ENABLE 1
#undef NEXX_DEBUG_ENABLE

#ifdef NEXX_DEBUG_ENABLE
#define Debug_begin(a) Serial.begin(a)
#define Debug_println(a) Serial.println(a)
#define Debug_print(a) Serial.print(a)
#define Debug_printf(a) Serial.printf(a)
#else
#define Debug_begin(a) do{}while(0)
#define Debug_println(a) do{}while(0)
#define Debug_print(a) do{}while(0)
#define Debug_printf(a) do{}while(0)
#endif


#endif
