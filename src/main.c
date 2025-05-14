/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include "ble.h"
#include "hog.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void) {
	LOG_INF("hello world!");

	int err;

	LOG_INF("initializing BLE");
	err = ble_init();

	LOG_INF("entering hog loop");
	hog_main();

	while (1) {
		LOG_DBG("heartbeat\n");
		k_msleep(1000);
	}
	return 0;
}

int usb_init(void) {
	int usb_enable_ret;

	usb_enable_ret = usb_enable(NULL);

	LOG_INF("USB enabled: %d", usb_enable_ret);

	return 0;
}

SYS_INIT(usb_init, APPLICATION, 50);
