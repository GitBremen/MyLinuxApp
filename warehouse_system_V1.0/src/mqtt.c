#include "mqtt.h"

MQTTClient MQTTClient_handle;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;

/**************设置回调函数*************/
void connectionLost(void *context, char *cause)
{
    printf("\nMQTTClient_connectionLost,cause: %s\n", cause);
}
int messageArrived(void *context, char *topicName,
                   int topicLen, MQTTClient_message *message)
{
    if (!strcmp(topicName, WAREHOUSE_TOPIC))
    {
        printf("Message arrived\n");
        printf("topic: %s\n", topicName);
        printf("message: <%d>%s\n", message->payloadlen, (char *)message->payload);
    }

    MQTTClient_freeMessage(&message); // 释放内存
    MQTTClient_free(topicName);       // 释放内存
    return 1;
}

/*应用程序采用同步方式发布消息或者您不想检查是否成功发送时,可以不设置*/
void deliveryComplete(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
}
/**************设置回调函数*************/

/**************初始化MQTT客户端*************/
int MQTTClient_Init(void)
{
    int rc = 0;

    /*创建MQTT客户端对象*/
    if ((rc = MQTTClient_create(&MQTTClient_handle, ADDRESS, CLIENTID,
                                MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    /*设置回调函数*/
    if ((rc = MQTTClient_setCallbacks(MQTTClient_handle, NULL,
                                      connectionLost,
                                      messageArrived,
                                      deliveryComplete)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set Callbacks, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    will_opts.topicName = WILL_TOPIC;
    will_opts.message = "Abnormally dropped";
    will_opts.retained = 1;
    will_opts.qos = 0;

    conn_opts.will = &will_opts;
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 0;
    conn_opts.username = "rk3566";
    conn_opts.password = "rk3566";

    /*连接服务器*/
    if ((rc = MQTTClient_connect(MQTTClient_handle, &conn_opts) != MQTTCLIENT_SUCCESS))
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("MQTT server connect succefully\n");

    /*发布上线消息*/
    if (MQTTCLIENT_SUCCESS != MQTTClientPublish(WILL_TOPIC, "Online"))
    {
        fprintf(stderr, "publish error");
        return rc;
    }

    /* 订阅主题WAREHOUSE_TOPIC */
    if (MQTTCLIENT_SUCCESS !=
        MQTTClientSubscribe(WAREHOUSE_TOPIC, 0))
    {
        fprintf(stderr, "Failed to subscribe");
        return rc;
    }

    return rc;
}

/**************发布函数*************/
int MQTTClientPublish(const char *topicName, char *msg)
{
    int rc = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = msg;
    pubmsg.payloadlen = strlen(msg);
    pubmsg.qos = 0;
    pubmsg.retained = 1;

    if (rc = (MQTTClient_publishMessage(
                 MQTTClient_handle, topicName, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to publishMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("publish topic:%s success\n", topicName);
    return rc;
}

/**************订阅函数*************/
int MQTTClientSubscribe(const char *topic, int QOS)
{
    int rc = 0;

    if ((rc = MQTTClient_subscribe(MQTTClient_handle, topic, QOS)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to publishMessage, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("subscribe topic:%s success\n", topic);
    return rc;
}
