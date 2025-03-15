#ifndef __MQTT_H
#define __MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

#define ADDRESS "broker.emqx.io:1883"
#define CLIENTID "rk3566"
#define WILL_TOPIC "rk3566/will"
#define WAREHOUSE_TOPIC "rk3566/warehouse"

int MQTTClient_Init(void);

int MQTTClientPublish(const char *topicName, char *msg);
int MQTTClientSubscribe(const char *topic, int QOS);

#endif