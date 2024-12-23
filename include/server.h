#ifndef SERVER_H
#define SERVER_H

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include <socket.h>
#include <unistd.h>

#define PORT 22160
#define BACKLOGS 10



void tcp_server();

#endif