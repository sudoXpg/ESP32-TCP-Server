#include "wifi_connect.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <string.h>


// Error check function
void err_check(esp_err_t ret, const char *msg) {
    if (ret != ESP_OK) {
        printf( "%s failed with code: 0x%x", msg, ret);
        abort();
    }
}


static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    if(event_base==WIFI_EVENT){
        switch(event_id){
            case WIFI_EVENT_STA_START:
                printf( "Wi-Fi has started connecting\n");
                esp_wifi_connect();
                break;
            
            case WIFI_EVENT_STA_DISCONNECTED:
                printf( "Wi-Fi has disconnected\nAttempting to reconnect ...\n");
                esp_wifi_connect();
                break;

            default:
                printf( "Unhandled Wi-Fi event: %lx", event_id);
                break;
        }
    }
    else if(event_base==IP_EVENT && event_id==IP_EVENT_STA_GOT_IP){
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;     // cast the event data to a ip event data
        printf("IP Address: %d.%d.%d.%d \n", IP2STR(&event->ip_info.ip));
        printf( "Wi-Fi connected !\n");
    }
}


void wifi_connect(char *wifiSSID, char *wifiPWD){

    esp_err_t ret = ESP_OK;
    const wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();

    // Initialize NVS flash storage
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    err_check(ret, "NVS Flash Initialization");

    // Initialize network interface
    ret = esp_netif_init();
    err_check(ret, "Network Interface Initialization");

    // Initialize event loop
    ret = esp_event_loop_create_default();
    err_check(ret, "Event Loop Creation");

    // Create default Wi-Fi station
    esp_netif_create_default_wifi_sta();

    
    // Register Wi-Fi and IP event handlers
    esp_event_handler_instance_t any_event;
    esp_event_handler_instance_t got_ip_event;

    ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &any_event);
    err_check(ret, "Registering Wi-Fi Event Handler");

    ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &got_ip_event);
    err_check(ret, "Registering IP Event Handler");

    // Initialize the Wi-Fi stack with default configuration
    ret = esp_wifi_init(&wifi_config);
    err_check(ret, "Wi-Fi Initialization");

    // Set Wi-Fi mode to station
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    err_check(ret, "Wi-Fi Mode Setting");

    // Set Wi-Fi credentials
    wifi_config_t wifi_nfo = {
        .sta = {
            .threshold={
                .authmode = WIFI_AUTH_OPEN
            }
        }
    };
    // Copy SSID and password into the arrays
    strncpy((char *)wifi_nfo.sta.ssid, wifiSSID, sizeof(wifi_nfo.sta.ssid) - 1);
    wifi_nfo.sta.ssid[sizeof(wifi_nfo.sta.ssid) - 1] = '\0'; // Ensure null-termination

    strncpy((char *)wifi_nfo.sta.password, wifiPWD, sizeof(wifi_nfo.sta.password) - 1);
    wifi_nfo.sta.password[sizeof(wifi_nfo.sta.password) - 1] = '\0'; // Ensure null-termination


    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_nfo);
    err_check(ret, "Wi-Fi Configuration Setting");

    // Start Wi-Fi
    ret = esp_wifi_start();
    err_check(ret, "Wi-Fi Start");
}