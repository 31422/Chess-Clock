#include "pebble.h"
#include "time.h"
#include "resource_ids.auto.h"

#define WHITE_ON_BLACK

#ifndef WHITE_ON_BLACK
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

#define LONG_CLICK_MS 700
#define MULTI_CLICK_INTERVAL 10

#define TOTAL_SECONDS_KEY 1
#define TOTAL_SECONDS_DEFAULT 5 * 60
  

enum State_1 {
  DONE_1,
  SETTING_1,
  PAUSED_1,
  COUNTING_DOWN_1
};
enum State_2 {
  DONE_2,
  SETTING_2,
  PAUSED_2,
  COUNTING_DOWN_2
};



// Main window stuff
Window *window;

TextLayer *title;
TextLayer *count_down_1;
char count_down_1_text[10];
TextLayer *count_down_2;
char count_down_2_text[10];
TextLayer *time_text;
TextLayer *Blanc;
TextLayer *Noir;
InverterLayer *inverter_layer;

Layer *unit_marker;

enum State_1 current_state_1 = SETTING_1;
enum State_2 current_state_2 = SETTING_2;

int total_seconds_1;
int total_seconds_2;
int current_seconds_1;
int last_set_time_1 = -1;
int current_seconds_2;
int last_set_time_2 = -1;

const VibePattern alarm_finished = {
  .durations = (uint32_t []) {300, 150, 150, 150,  300, 150, 300},
  .num_segments = 7
};

// Setting state

enum SettingUnit {
  SETTING_SECOND = 2,
  SETTING_MINUTE = 1,
  SETTING_HOUR = 0,
};

enum SettingUnit setting_unit = SETTING_MINUTE;

void update_countdown1() {
  if (current_seconds_1 == last_set_time_1) {
    return;
  }

  static char time_fmt_1[] = " %02d:%02d:%02d";
  static char count_down_1_text[] = " 00:00:00";

  int hour_1 = current_seconds_1 / (60 * 60);
  int min_1  = (current_seconds_1 - hour_1 * 60 * 60) / 60;
  int sec_1  = current_seconds_1 - hour_1 * 60 * 60 - min_1 * 60;

  snprintf(count_down_1_text, sizeof(count_down_1_text), time_fmt_1, hour_1, min_1, sec_1);
  text_layer_set_text(count_down_1, count_down_1_text);
  last_set_time_1 = current_seconds_1;
}

void update_countdown2() {
  if (current_seconds_2 == last_set_time_2) {
    return;
  }

  static char time_fmt_2[] = " %02d:%02d:%02d";
  static char count_down_2_text[] = " 00:00:00";

  int hour_2 = current_seconds_2 / (60 * 60);
  int min_2  = (current_seconds_2 - hour_2 * 60 * 60) / 60;
  int sec_2  = current_seconds_2 - hour_2 * 60 * 60 - min_2 * 60;

  snprintf(count_down_2_text, sizeof(count_down_2_text), time_fmt_2, hour_2, min_2, sec_2);
  text_layer_set_text(count_down_2, count_down_2_text);
  last_set_time_2 = current_seconds_2;
}


bool setting_blink_state = true;

void draw_setting_unit() {
  layer_mark_dirty(unit_marker);  
}

void toggle_setting_mode(ClickRecognizerRef recognizer, void *window) {
  if (current_state_1 == SETTING_1 && current_state_2 == SETTING_2) {
   /* current_state_1 = COUNTING_DOWN_1;
    current_state_2 = PAUSED_2; */
  }
  else {
    current_seconds_1 = total_seconds_1;
    current_seconds_2 = total_seconds_2;
    update_countdown1();
    update_countdown2();
    current_state_1 = SETTING_1;
    current_state_2 = SETTING_2;
    setting_unit = SETTING_MINUTE;
    draw_setting_unit();
    //current_seconds_2 = total_seconds_2;
 
  }
  /*else {
    current_seconds_1 = total_seconds_1;
    current_seconds_2 = total_seconds_2;
    update_countdown1();
    update_countdown2();
    current_state_1 = SETTING_1;
    current_state_2 = SETTING_2;
    setting_unit = SETTING_MINUTE;
    draw_setting_unit();
    //current_seconds_2 = total_seconds_2; 
 //SOMETHING 
  } 
  */
}

void unit_marker_update_callback(Layer *me, GContext* ctx) {
  (void)me;

  int width = 20;
  int start = 61 + (width + 7) * setting_unit;

  if (current_state_1 == SETTING_1 && setting_blink_state) {
    graphics_context_set_stroke_color(ctx, COLOR_BACKGROUND);

    for (int i = 0; i < 4; i++) {
      graphics_draw_line(ctx, GPoint(start, 65 + i), GPoint(start + width, 65 + i));
    }
  }
  if (current_state_2 == SETTING_2 && setting_blink_state) {
    graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);

  for (int j = 0; j < 4; j++) {
      graphics_draw_line(ctx, GPoint(start, 109 + j), GPoint(start + width, 109 + j));
    }
  }
}

  //This section below controls the starting and stopping of the timer(s)
void select_pressed(ClickRecognizerRef recognizer, void *window) {
  if (current_state_1 == SETTING_1 && current_state_2 == SETTING_2) {
    //FLAG GOES HERE
    setting_unit = (setting_unit + 1) % 3;
    setting_blink_state = true;
    draw_setting_unit();
    //FLAG GOES HERE
    // }
  }
  else 
  {
    current_state_1 = SETTING_1;
    current_state_2 = SETTING_2;
  }

}

void select_long_release_handler(ClickRecognizerRef recognizer, void *window) {
  if (current_state_1 == SETTING_1 && current_state_2 == SETTING_2)
    {
    //Flag goes here
    current_state_1 = COUNTING_DOWN_1;
    current_state_2 = PAUSED_2;
    //flag goes here
    // }
  }
   else if (current_state_1 == PAUSED_1 || current_state_2 == PAUSED_2) 
   {
    current_state_1 = SETTING_1;
    current_state_2 = SETTING_2;
   }
  
  
   else
   {
    current_state_1 = SETTING_1;
    current_state_2 = SETTING_2;
     //Flag goes here
   }
  // This is needed to avoid missing clicks. Seems to be a bug in the SDK.
  //Below appears to be The button cicks for settings
}

void increment_time(int direction) {
  if (current_state_1 == SETTING_1 && current_state_2 == SETTING_2) {
    switch (setting_unit) {
    case SETTING_HOUR: direction *= 60;
    case SETTING_MINUTE: direction *= 60;
    default: break;
    }

    int new_seconds_1 = total_seconds_1 + direction;
    if (new_seconds_1 >= 0 && new_seconds_1 < 100 * 60 * 60) {
      total_seconds_1 = new_seconds_1;
      current_seconds_1 = total_seconds_1;
      update_countdown1();
    }
    
    int new_seconds_2 = total_seconds_2 + direction;
    if (new_seconds_2 >= 0 && new_seconds_2 < 100 * 60 * 60) {
      total_seconds_2 = new_seconds_2;
      current_seconds_2 = total_seconds_2;
      update_countdown2();
    }
  }
}
// Not very certain what these are for but this is also required for button clicks above
    //This is for adding time
void button_pressed_up(ClickRecognizerRef recognizer, void *window) {
  if (current_state_1 == SETTING_1 && current_state_2 == SETTING_2) {
  increment_time(1);
  }
  else if (current_state_1 == COUNTING_DOWN_1 && current_state_2 == PAUSED_2){
    current_state_1 = PAUSED_1;
    current_state_2 = COUNTING_DOWN_2;
  }
  else {
    current_state_1 = COUNTING_DOWN_1;
    current_state_2 = PAUSED_2;
  }
}
    //This is for taking time away
void button_pressed_down(ClickRecognizerRef recognizer, void *window) {
  if (current_state_1 == SETTING_1 && current_state_2 == SETTING_2) {
  increment_time(-1);
  }
   else if (current_state_2 == COUNTING_DOWN_2 && current_state_1 == PAUSED_1){
    current_state_1 = COUNTING_DOWN_1;
    current_state_2 = PAUSED_2;
  
   }
  else {
    current_state_1 = PAUSED_1;
    current_state_2 = COUNTING_DOWN_2;
  }
}
    //This is for saving changes to the timer
void reset_timer(ClickRecognizerRef recognizer, void *window) {
    if (current_state_1 != SETTING_1) {
    current_state_1 = PAUSED_1;
    current_seconds_1 = total_seconds_1;
    update_countdown1();
  }
  
    if (current_state_2 != SETTING_2) {
    current_state_2 = PAUSED_2;
    current_seconds_2 = total_seconds_2;
    update_countdown2();
  }
}
// Here we have the button clicks regarding the timer actually running
void main_click_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_pressed);

  window_long_click_subscribe(BUTTON_ID_SELECT, LONG_CLICK_MS,
                              toggle_setting_mode, select_long_release_handler);

  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 2, 10, true, reset_timer);

  window_single_repeating_click_subscribe(BUTTON_ID_UP, 300, button_pressed_up);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 300, button_pressed_down);  

}

void handle_second_counting_down_1() {
  current_seconds_1--;

  update_countdown1();

  if (current_seconds_1 == 0) {
    vibes_enqueue_custom_pattern(alarm_finished);
    current_state_1 = DONE_1;
  }
}

void handle_second_counting_down_2() {
  current_seconds_2--;

  update_countdown2();

  if (current_seconds_2 == 0) {
    vibes_enqueue_custom_pattern(alarm_finished);
    current_state_2 = DONE_2;
  }
}
  
void handle_second_waiting_1() {
  //current_seconds_1 = total_seconds_1;
  update_countdown1();
}
void handle_second_waiting_2() {
  
  //current_seconds_2 = total_seconds_2;
  update_countdown2();
}


void handle_second_setting() {
  setting_blink_state = !setting_blink_state;
  layer_mark_dirty(unit_marker);
}

void update_time(struct tm *tick_time) {
  static char time[] = "Xxxxxxxxx - 00 00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%B %e   %R";
  } else {
    time_format = "%B %e   %I:%M";
  }

  strftime(time, sizeof(time), time_format, tick_time);

  text_layer_set_text(time_text, time);
}

void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {
  switch(current_state_1) {
  case PAUSED_1:
    handle_second_waiting_1();
    break;
  case COUNTING_DOWN_1:
    handle_second_counting_down_1();
    break;
  case SETTING_1:
    handle_second_setting();
    break;
  default:
    break;
  }
    
  switch(current_state_2) {
  case PAUSED_2:
    handle_second_waiting_2();
    break;
  case COUNTING_DOWN_2:
    handle_second_counting_down_2();
    break;
  case SETTING_2:
    handle_second_setting();
    break;
  default:
    break;
  }

  if (units_changed & MINUTE_UNIT) {
    update_time(tick_time);
  }
}

void handle_init(void) {
  total_seconds_1 = persist_exists(TOTAL_SECONDS_KEY) ?
    persist_read_int(TOTAL_SECONDS_KEY) : TOTAL_SECONDS_DEFAULT;
  
  total_seconds_2 = persist_exists(TOTAL_SECONDS_KEY) ?
    persist_read_int(TOTAL_SECONDS_KEY) : TOTAL_SECONDS_DEFAULT;


  window = window_create();
  window_set_fullscreen(window, true);
  window_set_background_color(window, COLOR_BACKGROUND);
  window_stack_push(window, true /* Animated */);

  Layer *window_layer = window_get_root_layer(window);
  
  window_set_click_config_provider(window, (ClickConfigProvider) main_click_provider);
// Timer One: Player 1 with White chess peices
  count_down_1 = text_layer_create(GRect(55, 35, 89, 40));
  text_layer_set_font(count_down_1, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(count_down_1, COLOR_BACKGROUND);
  text_layer_set_background_color(count_down_1, COLOR_FOREGROUND);
  text_layer_set_text(count_down_1, count_down_1_text);
  update_countdown1();
  layer_add_child(window_layer, text_layer_get_layer(count_down_1));
// Timer Two: Player 2 with Black chess peices
  count_down_2 = text_layer_create(GRect(55, 79, 89, 40));
  text_layer_set_font(count_down_2, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(count_down_2, COLOR_BACKGROUND);
  text_layer_set_background_color(count_down_2, COLOR_FOREGROUND);
  text_layer_set_text(count_down_2, count_down_2_text);
  update_countdown2();
  layer_add_child(window_layer, text_layer_get_layer(count_down_2));
  
  inverter_layer = inverter_layer_create(GRect(55, 79, 89, 40));
	layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
  
    unit_marker = layer_create(
    layer_get_frame(window_layer));
  layer_set_update_proc(unit_marker, unit_marker_update_callback);
  layer_add_child(window_layer, unit_marker);

// The top text layer for name of the app
  title = text_layer_create(GRect(30, 5, 100, 24));
  text_layer_set_font(title, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_color(title, COLOR_FOREGROUND);
  text_layer_set_background_color(title, GColorClear);
  text_layer_set_text(title, "Chess Clock");
  layer_add_child(window_layer, text_layer_get_layer(title));
  // Blanc
  Blanc = text_layer_create(GRect(0, 35, 55, 40));
  text_layer_set_font(Blanc, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_color(Blanc, COLOR_FOREGROUND);
  text_layer_set_background_color(Blanc, GColorClear);
  text_layer_set_text(Blanc, "White");
  layer_add_child(window_layer, text_layer_get_layer(Blanc));
  
  inverter_layer = inverter_layer_create(GRect(0, 35, 55, 40));
	layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
  // Noir
  Noir = text_layer_create(GRect(0, 79, 55, 40));
  text_layer_set_font(Noir, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_color(Noir, COLOR_FOREGROUND);
  text_layer_set_background_color(Noir, GColorClear);
  text_layer_set_text(Noir, "Black");
  layer_add_child(window_layer, text_layer_get_layer(Noir));
// This is the bootom text layer for the time and date going under the timers
  time_text = text_layer_create(GRect(20, 120, 110, 30));
  text_layer_set_font(time_text,
                      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(time_text, COLOR_FOREGROUND);
  text_layer_set_background_color(time_text, GColorClear);
  
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  update_time(current_time);
  layer_add_child(window_layer, text_layer_get_layer(time_text));

  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}


void handle_deinit() {
  persist_write_int(TOTAL_SECONDS_KEY, total_seconds_1);
  persist_write_int(TOTAL_SECONDS_KEY, total_seconds_2);
  text_layer_destroy(title);
  text_layer_destroy(count_down_1);
  text_layer_destroy(count_down_2);
  text_layer_destroy(Blanc);
  text_layer_destroy(Noir);
  text_layer_destroy(time_text);
  inverter_layer_destroy(inverter_layer);
  layer_destroy(unit_marker);
  window_destroy(window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
  