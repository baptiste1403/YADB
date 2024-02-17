#ifndef __DEFER__
#define __DEFER__

#define return_defer(value) do { defer_return_value = (value); goto defer; } while(0)

#endif // __DEFER__