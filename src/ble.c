#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

#include "ble.h"
#include "hog.h"

LOG_MODULE_REGISTER(ble, LOG_LEVEL_DBG);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
			  BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL),
			  BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static void connected(struct bt_conn* conn, uint8_t err) {
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		LOG_ERR("Failed to connect to %s (%u)\n", addr, err);
		return;
	}

	LOG_INF("Connected %s\n", addr);

	if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
		LOG_ERR("Failed to set security\n");
	} else {
		LOG_INF("Security set\n");
	}
}

static void disconnected(struct bt_conn* conn, uint8_t reason) {
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Disconnected from %s, reason 0x%02x %s\n", addr, reason, bt_hci_err_to_str(reason));
}

static void security_changed(
	struct bt_conn* conn,
	bt_security_t level,
	enum bt_security_err err
) {
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		LOG_INF("Security changed: %s level %u\n", addr, level);
	} else {
		LOG_ERR("Security failed: %s level %u err %d\n", addr, level, err);
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.security_changed = security_changed,
};

static void bt_ready(int err) {
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return;
	}

	LOG_INF("Bluetooth initialized\n");

	hog_init();

	// turned off with CONFIG_SETTINGS commented out
	if (IS_ENABLED(CONFIG_SETTINGS)) {
		LOG_DBG("Loading settings");
		settings_load();
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)\n", err);
		return;
	} else {
		LOG_INF("Advertising successfully started\n");
	}
}

int ble_init(void) {
	int err;

	err = bt_enable(bt_ready);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return 0;
	} else {
		LOG_INF("Bluetooth initialized\n");
	}

	hog_button_loop();
	return 0;
}

SYS_INIT(ble_init, APPLICATION, 50);

