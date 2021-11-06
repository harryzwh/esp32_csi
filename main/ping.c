#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "ping/ping_sock.h"

static const char *TAG = "[ping]";
esp_ping_handle_t ping_session_handle;

static void ping_success_callback(esp_ping_handle_t hdl, void *args)
{
    // optionally, get callback arguments
    // const char* str = (const char*) args;
    // printf("%s\r\n", str); // "foo"
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    ESP_LOGI(TAG, "%d bytes from %s icmp_seq=%d ttl=%d time=%d ms",
             recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
}

static void ping_timeout_callback(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ESP_LOGI(TAG, "From %s icmp_seq=%d timeout", inet_ntoa(target_addr.u_addr.ip4), seqno);
}

static void ping_end_callback(esp_ping_handle_t hdl, void *args)
{
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    ESP_LOGI(TAG, "%d packets transmitted, %d received, time %dms", transmitted, received, total_time_ms);
}

void ping_init(void)
{
    /* convert URL to IP address */
    ip_addr_t target_addr;
    inet_pton(AF_INET, "192.168.76.201", (void *)&target_addr);

    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ping_config.target_addr = target_addr;       // target IP address
    ping_config.count = ESP_PING_COUNT_INFINITE; // ping in infinite mode, esp_ping_stop can stop it
    ping_config.interval_ms = 1000;
    ping_config.data_size = 1024;
    ping_config.timeout_ms = ping_config.interval_ms / 2;

    /* set callback functions */
    esp_ping_callbacks_t cbs;
    cbs.on_ping_success = ping_success_callback;
    cbs.on_ping_timeout = ping_timeout_callback;
    cbs.on_ping_end = ping_end_callback;
    cbs.cb_args = NULL; // arguments that will feed to all callback functions, can be NULL

    esp_ping_new_session(&ping_config, &cbs, &ping_session_handle);
    ESP_LOGI(TAG, "ping session inited");
}

void ping_start(void)
{
    esp_ping_start(ping_session_handle);
    ESP_LOGI(TAG, "ping session started");
}

void ping_stop(void)
{
    esp_ping_stop(ping_session_handle);
    ESP_LOGI(TAG, "ping session stoped");
}