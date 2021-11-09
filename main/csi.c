#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#define NUM_SAMPLE_BLOCK 16
#define SAMPLE_SIZE (1 + 4 + 128)

static const char *TAG = "[csi]";
static uint8_t csi_buffer[SAMPLE_SIZE * NUM_SAMPLE_BLOCK];
static uint8_t *csi_buffer_idx = csi_buffer;

void receive_csi_cb(void *ctx, wifi_csi_info_t *Rx_Data)
{
	// ESP_LOGI(TAG, "%02X:%02X:%02X:%02X:%02X:%02X  %d", Rx_Data->mac[0], Rx_Data->mac[1], Rx_Data->mac[2], Rx_Data->mac[3], Rx_Data->mac[4], Rx_Data->mac[5], Rx_Data->rx_ctrl.timestamp);
	// int csi_len = Rx_Data->len;
	// int8_t *data = Rx_Data->buf;
	// while (csi_len--)
	// {
	// 	printf("%d ", *(data++));
	// }
	// printf("\n");
	printf("CSI:%d ", Rx_Data->rx_ctrl.timestamp);
	if (Rx_Data->len == 128)
	{
		int8_t *data = Rx_Data->buf;
		int csi_len = Rx_Data->len;
		while (csi_len--)
		{
			printf("%d ", *(data++));
		}
		printf("\n");
	}
	// if (Rx_Data->len == 128)
	// {
	// 	int8_t rssi = Rx_Data->rx_ctrl.rssi;
	// 	uint32_t timestamp = Rx_Data->rx_ctrl.timestamp;
	// 	memcpy(csi_buffer_idx, &rssi, 1);
	// 	csi_buffer_idx += 1;
	// 	memcpy(csi_buffer_idx, &timestamp, 4);
	// 	csi_buffer_idx += 1;
	// 	memcpy(csi_buffer_idx, Rx_Data->buf, 128);
	// 	csi_buffer_idx += 128;
	// }
}

// void receive_promiscuous_cb(void *buf, wifi_promiscuous_pkt_type_t type)
// {
// 	switch (type)
// 	{
// 	case WIFI_PKT_MGMT:
// 	case WIFI_PKT_CTRL:
// 	case WIFI_PKT_DATA:
// 	{
// 		wifi_promiscuous_pkt_t *data = (wifi_promiscuous_pkt_t *)buf;
// 		// ESP_LOGI(TAG, "type:%d, channel:%d, bandwidth:%d, len:%d, mode:%d", type, data->rx_ctrl.channel, data->rx_ctrl.cwb, data->rx_ctrl.sig_len, data->rx_ctrl.sig_mode);
// 		// ESP_LOG_BUFFER_HEX(TAG, data->payload+10, 6);
// 		break;
// 	}
// 	case WIFI_PKT_MISC:
// 		break;
// 	}
// }

void csi_init(void)
{
	wifi_csi_config_t CSI_Config;	  // CSI = Channel State Information
	CSI_Config.lltf_en = 1;			  //1
	CSI_Config.htltf_en = 0;		  //1
	CSI_Config.stbc_htltf2_en = 0;	  //1
	CSI_Config.ltf_merge_en = 0;	  //1
	CSI_Config.channel_filter_en = 1; //1
	CSI_Config.manu_scale = 0;		  // Automatic scalling
	// CSI_Config.shift=2; // 0->15
	ESP_ERROR_CHECK(esp_wifi_set_csi_config(&CSI_Config));
	ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&receive_csi_cb, NULL));
	// wifi_promiscuous_filter_t filter = {
	// 	.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT};
	// esp_wifi_set_promiscuous_filter(&filter);
	// esp_wifi_set_promiscuous(true);
	// esp_wifi_set_promiscuous_rx_cb(&receive_promiscuous_cb);
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