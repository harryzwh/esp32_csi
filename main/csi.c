#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

static const char *TAG = "[csi]";

void receive_csi_cb(void *ctx, wifi_csi_info_t *Rx_Data)
{
    ESP_LOGI(TAG, "%02X:%02X:%02X:%02X:%02X:%02X", Rx_Data->mac[0], Rx_Data->mac[1], Rx_Data->mac[2], Rx_Data->mac[3], Rx_Data->mac[4], Rx_Data->mac[5]);
}

void csi_init(void)
{
    wifi_csi_config_t CSI_Config; // CSI = Channel State Information
	CSI_Config.lltf_en = 1;//1
	CSI_Config.htltf_en = 1;//1
	CSI_Config.stbc_htltf2_en = 1;//1
	CSI_Config.ltf_merge_en = 1;//1
	CSI_Config.channel_filter_en = 0;//1
	CSI_Config.manu_scale = 0; // Automatic scalling
	//CSI_Config.shift=15; // 0->15
	ESP_ERROR_CHECK(esp_wifi_set_csi_config(&CSI_Config));
	ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&receive_csi_cb, NULL));
    ESP_LOGI(TAG, "wifi csi inited");
}

void csi_start(void)
{
    esp_wifi_set_csi(true);
    ESP_LOGI(TAG, "wifi csi started");
}

void csi_stop(void)
{
    esp_wifi_set_csi(false);
    ESP_LOGI(TAG, "wifi csi stoped");
}