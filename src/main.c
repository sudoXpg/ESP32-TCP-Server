#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "wifi_connect.h"
#include "server.h"

#define WIFI_SSID "abc"             // network name
#define WIFI_PWD  ""                // network password


void app_main(){

    wifi_connect(WIFI_SSID,WIFI_PWD);
    tcp_server();
        
}

