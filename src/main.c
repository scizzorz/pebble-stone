#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

// config.h defines the SERVER_URL constant
#include "config.h"
#include "constants.h"
#include "http.h"
#include "util.h"

PBL_APP_INFO(UUID, NAME, AUTHOR, MAJOR_VERSION, MINOR_VERSION, RESOURCE_ID_ICON_MENU, APP_INFO_WATCH_FACE);

Window window;
Layer layer;
BmpContainer icon_bmp;

bool has_icon, has_weather, has_location;
int status, cur_lat, cur_long, cur_icon;
char time_str[BUFSIZE], date_str[BUFSIZE], weather_str[BUFSIZE];

static uint8_t WEATHER_ICONS[] = {
	RESOURCE_ID_ICON_CLEAR_DAY,
	RESOURCE_ID_ICON_CLEAR_NIGHT,
	RESOURCE_ID_ICON_RAIN,
	RESOURCE_ID_ICON_SNOW,
	RESOURCE_ID_ICON_SLEET,
	RESOURCE_ID_ICON_WIND,
	RESOURCE_ID_ICON_FOG,
	RESOURCE_ID_ICON_CLOUDY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_DAY,
	RESOURCE_ID_ICON_PARTLY_CLOUDY_NIGHT,
};
char* STATUS_MESSAGES[] = {
	" ",
	"...",
	"x",
	"!",
	"!"
};

void set_timer(AppContextRef ctx) {
	app_timer_send_event(ctx, REFRESH_DELAY, 1);
}

// UI functions
void update_time(PblTm* time) {
	string_format_time(time_str, BUFSIZE, "%l:%M%P", time);
	string_format_time(date_str, BUFSIZE, "%a, %b %d", time);
	trim(time_str);
	layer_mark_dirty(&layer);
}
void update_weather(char* msg, bool is_weather) {
	// only write non-weather messages if we don't
	// already have the weather
	if((!is_weather && !has_weather) || is_weather) {
		strcpy(weather_str, msg);
		layer_mark_dirty(&layer);
	}
}
void update_icon(uint8_t new_icon) {
	// free the last icon
	if(has_icon) {
		layer_remove_from_parent(&icon_bmp.layer.layer);
		bmp_deinit_container(&icon_bmp);
		has_icon = false;
	}

	// make a new icon
	bmp_init_container(WEATHER_ICONS[new_icon], &icon_bmp);
	layer_add_child(&layer, &icon_bmp.layer.layer);
	layer_set_frame(&icon_bmp.layer.layer, GRect(44, ICON_Y, 24, 24));
	has_icon = true;
	cur_icon = new_icon;
}
void update_status(int to) {
	status = to;
	layer_mark_dirty(&layer);
}

void request_weather() {
	if(!has_location) {
		update_weather("fetching gps...", false);
		http_location_request();
		return;
	}

	update_weather("making http...", false);
	DictionaryIterator *body;
	HTTPResult result = http_out_get(SERVER_URL, 1, &body);

	if(result == HTTP_NOT_CONNECTED) {
		update_weather("disconnected", false);
		update_status(STATUS_DISC);
		return;
	}

	if(result != HTTP_OK) {
		update_weather("make fail", false);
		update_status(STATUS_ERR);
		return;
	}

	dict_write_int32(body, POST_KEY_LAT, cur_lat);
	dict_write_int32(body, POST_KEY_LONG, cur_long);
	dict_write_cstring(body, POST_KEY_UNITS, "auto");

	update_weather("sending http...", false);
	if(http_out_send() != HTTP_OK) {
		update_weather("send fail", false);
		update_status(STATUS_ERR);
		return;
	}
	update_status(STATUS_LOAD);
	update_weather("waiting...", false);
}

// httpebble callbacks
void http_success(int32_t cookie, int http_status, DictionaryIterator* datas, void* context) {
	if(!cookie) return;

	Tuple* data = dict_find(datas, HTTP_KEY_WEATHER);
	if(data) {
		uint16_t value = data->value->int16;
		uint8_t icon_id = value >> 11;
		int16_t temp = value & 0x3FF;
		if(value & 0x400) {
			temp = -temp;
		}

		has_weather = true;
		update_weather(itoa(temp), true);
		update_icon(icon_id);
	}
	update_status(STATUS_IDLE);
}
void http_failure(int32_t cookie, int http_status, void* context) {
	update_weather("receive fail", false);
	update_status(STATUS_FAIL);
}
void http_reconnect(void* context) {
	request_weather();
}
void http_location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	cur_lat = latitude * 10000;
	cur_long = longitude * 10000;
	has_location = true;
	update_weather("refreshing...", false);
	request_weather();
	set_timer((AppContextRef)context);
}

// text layer callback
void update_layer_callback(Layer *self, GContext* ctx) {
	graphics_context_set_text_color(ctx, GColorBlack);

	// Time
	graphics_text_draw(ctx,
			time_str,
			fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK),
			GRect(5, TIME_Y, 134, 100),
			GTextOverflowModeWordWrap,
			GTextAlignmentCenter,
			NULL);

	// Date
	graphics_text_draw(ctx,
			date_str,
			fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
			GRect(5, DATE_Y, 134, 100),
			GTextOverflowModeWordWrap,
			GTextAlignmentCenter,
			NULL);

	// Weather
	if(has_weather) {
		graphics_text_draw(ctx,
				weather_str,
				fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
				GRect(72, WEATHER_Y, 67, 30),
				GTextOverflowModeWordWrap,
				GTextAlignmentLeft,
				NULL);
	} else {
		graphics_text_draw(ctx,
				weather_str,
				fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
				GRect(5, WEATHER_Y, 134, 30),
				GTextOverflowModeWordWrap,
				GTextAlignmentCenter,
				NULL);
	}

	// Loading status
	if(status) {
		graphics_text_draw(ctx,
				STATUS_MESSAGES[status],
				fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
				GRect(0, 144, 144, 24),
				GTextOverflowModeWordWrap,
				GTextAlignmentCenter,
				NULL);
	}
}

// OS callbacks
void handle_init(AppContextRef ctx) {
	window_init(&window, "Two");
	window_set_background_color(&window, GColorWhite);
	window_stack_push(&window, true);

	resource_init_current_app(&APP_RESOURCES);

	layer_init(&layer, window.layer.frame);
	layer.update_proc = update_layer_callback;
	layer_add_child(&window.layer, &layer);

	has_location = false;
	has_weather = false;
	has_icon = false;
	update_status(STATUS_IDLE);

	PblTm temp;
	get_time(&temp);
	update_time(&temp);
	update_weather("--", false);

	http_register_callbacks((HTTPCallbacks){
			.failure = http_failure,
			.success = http_success,
			.reconnect = http_reconnect,
			.location = http_location
			}, (void*)ctx);

	request_weather();
}
void handle_deinit(AppContextRef ctx) {
	if(has_icon) {
		bmp_deinit_container(&icon_bmp);
	}
}
void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
	request_weather();
	if(cookie) set_timer(ctx);
}
void handle_tick(AppContextRef ctx, PebbleTickEvent *event) {
	update_time(event->tick_time);
}

// OS init
void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.deinit_handler = &handle_deinit,
		.tick_info = {
			.tick_handler = &handle_tick,
			.tick_units = MINUTE_UNIT
		},
		.timer_handler = handle_timer,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 124,
				.outbound = 256,
			}
		}
	};
	app_event_loop(params, &handlers);
}
