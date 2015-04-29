#include <pebble.h>

enum {
  CONFIG_KEY_BGCOLOR = 1,
  CONFIG_KEY_FGCOLOR = 2
};


#define DIGIT_CHANGE_ANIM_DURATION 800
#define STARTDELAY 700
#define NUMSLOTS 4


GColor bgColor, fgColor;
static char bgColorText[10] = "#000000";
static char fgColorText[10] = "#ffffff";

typedef struct {
  Layer *layer;
  int   prevDigit;
  int   curDigit;
  uint32_t normTime;
} digitSlot;

Window *window;
Layer *mainLayer;
Layer *rootLayer;

digitSlot slot[NUMSLOTS]; // 4 big digits for the hour

bool animRunning = false;
static AppTimer *timer;

AnimationImplementation animImpl;
Animation *anim = NULL;

char buffer[256] = "";

GRect rects[11][2] = {
  { { { 17, 16 }, { 34, 48 } }, { { 17, 16 }, { 34, 48 } } },  // 0
  { { {  0,  0 }, { 34, 80 } }, { { 51,  0 }, { 17, 80 } } },  // 1
  { { {  0, 16 }, { 51, 16 } }, { { 17, 48 }, { 51, 16 } } },  // 2
  { { {  0, 16 }, { 51, 16 } }, { {  0, 48 }, { 51, 16 } } },  // 3
  { { { 17,  0 }, { 34, 32 } }, { {  0, 48 }, { 51, 32 } } },  // 4
  { { { 17, 16 }, { 51, 16 } }, { {  0, 48 }, { 51, 16 } } },  // 5
  { { { 17, 16 }, { 51, 16 } }, { { 17, 48 }, { 34, 16 } } },  // 6
  { { {  0, 16 }, { 51, 32 } }, { {  0, 48 }, { 51, 32 } } },  // 7
  { { { 17, 16 }, { 34, 16 } }, { { 17, 48 }, { 34, 16 } } },  // 8
  { { { 17, 16 }, { 34, 16 } }, { {  0, 48 }, { 51, 16 } } },  // 9
  { { {  0,  0 }, { 68, 32 } }, { {  0, 32 }, { 68, 48 } } }  // 10
};


GRect slotFrame(int i) {
  return GRect(2+72*(i%2), 2+84*(i>>1), 68, 80);
}

digitSlot *findSlot(Layer *layer) {
  int i;
  for (i=0; i<NUMSLOTS; i++) {
    if (slot[i].layer == layer) {
      return &slot[i];
    }
  }

  return NULL;
}

void updateMainLayer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, bgColor);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 0, GCornerNone);
}

void updateSlot(Layer *layer, GContext *ctx) {
  int i, x1, x2, y1, y2, w1, w2, h1, h2;
  GRect r[2];
  digitSlot *slot;

  slot = findSlot(layer);

  if (slot->prevDigit != slot->curDigit) {
    if (slot->normTime == ANIMATION_NORMALIZED_MAX) {
      r[0] = rects[slot->curDigit][0];
      r[1] = rects[slot->curDigit][1];
    } else {
      for (i=0; i<2; i++) {
        x1 = rects[slot->prevDigit][i].origin.x;
        y1 = rects[slot->prevDigit][i].origin.y;
        w1 = rects[slot->prevDigit][i].size.w;
        h1 = rects[slot->prevDigit][i].size.h;

        x2 = rects[slot->curDigit][i].origin.x;
        y2 = rects[slot->curDigit][i].origin.y;
        w2 = rects[slot->curDigit][i].size.w;
        h2 = rects[slot->curDigit][i].size.h;

        r[i].origin.x = slot->normTime * (x2-x1) / ANIMATION_NORMALIZED_MAX + x1;
        r[i].origin.y = slot->normTime * (y2-y1) / ANIMATION_NORMALIZED_MAX + y1;
        r[i].size.w = slot->normTime * (w2-w1) / ANIMATION_NORMALIZED_MAX + w1;
        r[i].size.h = slot->normTime * (h2-h1) / ANIMATION_NORMALIZED_MAX + h1;
      }
    }
  } else {
    r[0] = rects[slot->curDigit][0];
    r[1] = rects[slot->curDigit][1];
  }

  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, fgColor);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 0, GCornerNone);

#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, bgColor);
#else
  graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  for (i=0; i<2; i++) {
    graphics_fill_rect(ctx, r[i], 0, GCornerNone);
  }
}

void initSlot(int i, Layer *parent) {
  slot[i].normTime = ANIMATION_NORMALIZED_MAX;
  slot[i].prevDigit = 0;
  slot[i].curDigit = 10;

  slot[i].layer = layer_create(slotFrame(i));
  layer_set_update_proc(slot[i].layer, updateSlot);
  layer_add_child(parent, slot[i].layer);
}

void deinitSlot(int i) {
  layer_destroy(slot[i].layer);
}

void redrawAllSlots() {
  int i;

  layer_mark_dirty(mainLayer);

  for (i=0; i<NUMSLOTS; i++) {
    layer_mark_dirty(slot[i].layer);
  }
}

void destroyAnim(Animation *animation) {
  if (anim != NULL) {
    animation_destroy(anim);
  }
  anim = NULL;
}

void createAnim() {
  anim = animation_create();
  animation_set_delay(anim, 0);
  animation_set_duration(anim, DIGIT_CHANGE_ANIM_DURATION);
  animation_set_implementation(anim, &animImpl);
}

#ifdef PBL_PLATFORM_APLITE
void animateDigits(struct Animation *anim, const uint32_t normTime)
#else
  void animateDigits(Animation *anim, const AnimationProgress normTime)
#endif
{
  int i;

  for (i=0; i<NUMSLOTS; i++) {
    if (slot[i].curDigit != slot[i].prevDigit) {
      slot[i].normTime = normTime;
      layer_mark_dirty(slot[i].layer);
    }
  }

  if (normTime == ANIMATION_NORMALIZED_MAX) {
    animRunning = false;
  }
}

void handle_tick(struct tm *now, TimeUnits units_changed) {
  int h, m;
  int i;

  //
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tick: start");
  //

  if (!animation_is_scheduled(anim)) {
    //
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tick: setting digits");
    //

    h = now->tm_hour;
    m = now->tm_min;

    if (!clock_is_24h_style()) {
      h = h%12;
      if (h == 0) {
        h = 12;
      }
    }

    for (i=0; i<NUMSLOTS; i++) {
      slot[i].prevDigit = slot[i].curDigit;
    }

    // Hour slots
    slot[0].curDigit = h/10;
    slot[1].curDigit = h%10;
    slot[2].curDigit = m/10;
    slot[3].curDigit = m%10;

    //
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tick: rescheduling anim");
    //
    createAnim();
    animation_schedule(anim);
  }

  //
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tick: end");
  //

}

void do_update() {
  time_t curTime;
  struct tm *now;

  curTime = time(NULL);
  now = localtime(&curTime);
  handle_tick(now, 0);
}

void handle_timer(void *data) {
  timer=NULL;
  do_update();
}

bool checkAndSaveInt(int *var, int val, int key) {
  int ret;

  if (*var != val) {
    *var = val;
    ret = persist_write_int(key, val);
    if (ret < 0) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "checkAndSaveInt() : persist_write_int(%d, %d) returned %d",
              key, val, ret);
    }
    return true;
  } else {
    return false;
  }
}

bool checkAndSaveString(char *var, char *val, int key) {
  int ret;

  if (strcmp(var, val) != 0) {
    strcpy(var, val);
    ret = persist_write_string(key, val);
    if (ret < (int)strlen(val)) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "checkAndSaveString() : persist_write_string(%d, %s) returned %d",
              key, val, ret);
    }
    return true;
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "checkAndSaveString() : value has not changed (was : %s, is : %s)",
            var, val);
    return false;
  }
}


int hexStringToInt(const char *hexString) {
  if (hexString[0] == '#') {
    hexString++;
  }

  int l = strlen(hexString);
  int m = 1;
  int retVal = 0;
  for (int c=l-1; c>=0; c--, m *= 16) {
    char cur = hexString[c];
    if (( cur >= '0') && (cur <= '9')) {
      retVal += (int)(cur - '0') * m;
    } else if (( cur >= 'a') && (cur <= 'f')) {
      retVal += (10 + (int)(cur - 'a')) * m;
    }
  }

  return retVal;
}

GColor setColorFromText(const char *colorText) {
#ifdef PBL_PLATFORM_APLITE
  if ((hexStringToInt(colorText) & 0x63) == 0) {
    return GColorBlack;
  } else {
    return GColorWhite;
  }
#else
  return GColorFromHEX(hexStringToInt(colorText));
#endif
}

void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "ERR: Message dropped");
}

void in_received_handler(DictionaryIterator *received, void *context) {
  bool bgColorChanged = false;
  bool fgColorChanged = false;

  Tuple *bgColorTuple = dict_find(received, CONFIG_KEY_BGCOLOR);
  Tuple *fgColorTuple = dict_find(received, CONFIG_KEY_FGCOLOR);

  if (bgColorTuple && fgColorTuple) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received config:");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "  bgColor=%s, fgColor=%s",
            (char *)bgColorTuple->value->cstring, (char *)fgColorTuple->value->cstring);

    bgColorChanged = checkAndSaveString(bgColorText, bgColorTuple->value->cstring, CONFIG_KEY_BGCOLOR);
    fgColorChanged = checkAndSaveString(fgColorText, fgColorTuple->value->cstring, CONFIG_KEY_FGCOLOR);

    if (bgColorChanged) {
      bgColor = setColorFromText(bgColorText);
    }

    if (fgColorChanged) {
      fgColor = setColorFromText(fgColorText);
    }

    if (bgColorChanged || fgColorChanged) {
      redrawAllSlots();
    }
  }
}

void readConfig() {
  if (persist_exists(CONFIG_KEY_BGCOLOR)) {
    persist_read_string(CONFIG_KEY_BGCOLOR, bgColorText, sizeof(bgColorText));
  } else {
    strcpy(bgColorText, "#000000");
    persist_write_string(CONFIG_KEY_BGCOLOR, bgColorText);
  }
  bgColor = setColorFromText(bgColorText);

  if (persist_exists(CONFIG_KEY_FGCOLOR)) {
    persist_read_string(CONFIG_KEY_FGCOLOR, fgColorText, sizeof(fgColorText));
  } else {
    strcpy(fgColorText, "#ffffff");
    persist_write_string(CONFIG_KEY_FGCOLOR, fgColorText);
  }
  fgColor = setColorFromText(fgColorText);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Stored config :");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "  bgcolor=%s, fgColor=%s", bgColorText, fgColorText);
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_open(255, 255);
}

void handle_init() {
  int i;

  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true);

  readConfig();
  app_message_init();
  
  rootLayer = window_get_root_layer(window);
  mainLayer = layer_create(layer_get_bounds(rootLayer));
  layer_add_child(rootLayer, mainLayer);
  layer_set_update_proc(mainLayer, updateMainLayer);
  
  for (i=0; i<NUMSLOTS; i++) {
    initSlot(i, mainLayer);
  }
  
  animImpl.setup = NULL;
  animImpl.update = animateDigits;
#ifdef PBL_PLATFORM_APLITE
  animImpl.teardown = destroyAnim;
#else
  animImpl.teardown = NULL;
#endif
  createAnim();
  
  timer = app_timer_register(STARTDELAY, handle_timer, NULL);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void handle_deinit() {
  int i;
  if (timer != NULL) {
    app_timer_cancel(timer);
    timer=NULL;
  }
  
  destroyAnim(anim);
  
  tick_timer_service_unsubscribe();

  for (i=0; i<NUMSLOTS; i++) {
    deinitSlot(i);
  }
  
  layer_destroy(mainLayer);
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

