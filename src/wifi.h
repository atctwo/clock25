#ifndef CLOCK_WIFI_H
#define CLOCK_WIFI_H

typedef void(*wifi_connect_cb_t)();

void setup_wifi();
bool connect_wifi(wifi_connect_cb_t callback);
bool is_wifi_connected();

#endif /* CLOCK_WIFI_H */