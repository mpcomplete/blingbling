#ifndef _MAIN_H
#define _MAIN_H

#ifndef DEBUG
#define NDEBUG
#endif

#include "../config.h"

using namespace std;

#include <assert.h>
#include "pglog.h"

/* Global Defines
 */
// keypress events
#define KEY_DOWN 0
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_ROTATE_LEFT 3
#define KEY_ROTATE_RIGHT 4
#define KEY_DROP 5
#define NUM_KEYS 6

// you need at least this many points before a score floater appears
#define MIN_FLOATSCORE 30

// config files
#define BLINGBLING_CONF "blingbling.conf"
#define PREFS_CONF ".blingprefs"
#define HIGHSCORES_CONF ".blingscores"

class App;
extern App* app;

typedef void BasicCallback(void* data);

#endif  // header
