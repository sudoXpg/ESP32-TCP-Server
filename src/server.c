#include "server.h"

static void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family==AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


void tcp_server() {
    int sockfd, new_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char *msg = "ESP32 - Welcome !\n";  // Message to be sent to clients
    char client_ip[INET_ADDRSTRLEN];

    // Set up TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("TCP_SERVER Unable to create socket");
        return;
    }

    // Set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("TCP_SERVER Unable to bind socket");
        return;
    }

    // Listen for incoming connections
    if (listen(sockfd, BACKLOGS) < 0) {
        printf("TCP_SERVER Listen failed");
        return;
    }

    printf("TCP_SERVER Server listening on port %d", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        if (new_fd < 0) {
            printf("TCP_SERVER Unable to accept connection");
            continue;
        }

        // Convert IP address to string
        inet_ntoa_r(client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("TCP_SERVER Client connected: %s", client_ip);

        // Send message to the client
        if (send(new_fd, msg, strlen(msg), 0) < 0) {
            printf("TCP_SERVER Unable to send message");
        }

        // Close connection
        close(new_fd);
    }

    // Cleanup
    close(sockfd);
}



// HTTP GET handler
esp_err_t hello_get_handler(httpd_req_t *req) {
    const char *resp_str = "Hello There!\n";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}