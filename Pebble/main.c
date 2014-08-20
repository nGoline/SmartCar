/**
* SmartCar Pebble Watch App
* Author: nGoline
* Date: 11th July 2013
*
* The MIT License (MIT)
*
*Copyright (c) 2014 Níckolas Goline
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in
*all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*THE SOFTWARE.
*/

#include "pebble.h"
#include "pebble_fonts.h"

//Globals
static Window *window;
static TextLayer *textLayer;

enum {
  SELECT_KEY = 0x0, // TUPLE_INTEGER
  UP_KEY = 0x01,
  DOWN_KEY = 0x02,
  DATA_KEY = 0x0
};

void out_sent_handler(DictionaryIterator *sent, void *context) {
  text_layer_set_text(textLayer, "Command sent!");
}

static void out_failed_handler(DictionaryIterator* failed, AppMessageResult reason, void* context) {
  static char buf[] = "123456";
  snprintf(buf, sizeof(buf), "not sent:%d", reason);
  text_layer_set_text(textLayer, buf);
}

static void in_received_handler(DictionaryIterator* iter, void* context) {
  Tuple *in_tuple = dict_find(iter, DATA_KEY);
  if (in_tuple)
    text_layer_set_text(textLayer, in_tuple->value->cstring);
}

void in_dropped_handler(AppMessageResult reason, void *context) {
	text_layer_set_text(textLayer, "Message dropped!");
}

static void send_cmd(uint8_t cmd) {
  Tuplet value = TupletInteger(DATA_KEY, cmd);
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  if (iter == NULL)
    return;
  
  dict_write_tuplet(iter, &value);
  dict_write_end(iter);
  
  app_message_outbox_send();
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  
  send_cmd(UP_KEY);
  
  text_layer_set_text(textLayer, "Up button sent");
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  
  send_cmd(DOWN_KEY);
  
  text_layer_set_text(textLayer, "Down button sent");
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
  
  send_cmd(SELECT_KEY);
  
  text_layer_set_text(textLayer, "Select button sent");
}

void click_config_provider(Window *window) {
  (void)window;

	window_set_click_context(BUTTON_ID_UP, window);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_single_click_handler);
	window_set_click_context(BUTTON_ID_SELECT, window);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)select_single_click_handler);
	window_set_click_context(BUTTON_ID_DOWN, window);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_single_click_handler);
}

void handle_init() {

	//Init window
	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, GColorWhite);
	
	//messages
	const uint32_t inbound_size = 64;
	const uint32_t outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	
	//Init TextLayers
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	textLayer = text_layer_create(bounds);
	text_layer_set_background_color(textLayer, GColorClear);
	text_layer_set_text_color(textLayer, GColorBlack);
	text_layer_set_text_alignment(textLayer, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(textLayer));	
	text_layer_set_text(textLayer, "Ready");
		
	app_message_register_inbox_received(in_received_handler);
   	app_message_register_inbox_dropped(in_dropped_handler);
   	app_message_register_outbox_sent(out_sent_handler);
   	app_message_register_outbox_failed(out_failed_handler);
	
	//Setup button click handlers
	window_set_click_config_provider_with_context(window, (ClickConfigProvider)click_config_provider, (void*)window);
	
	vibes_double_pulse();
}

int main(void) {
    handle_init();
		
	app_event_loop();
	
	text_layer_destroy(textLayer);
	window_destroy(window);
}
