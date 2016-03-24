#include "stdio.h" include "stdlib.h" include "string.h" include 
#"stddef.h" include "MQTTClient.h" include "MQTTClientPersistence.h" 
#include "time.h" define ADDRESS "be.shineseniors.org" define CLIENTID 
#"iCitySub" define TOPIC "EXAMPLE_TOPIC" define PAYLOAD "Hello World!" 
#define QOS 2 define TIMEOUT 10000L
FILE* fp_1; FILE* fp_2; char ch[50]="\n"; char beaconID_1[50]; char 
beaconID_2[50]; volatile MQTTClient_deliveryToken deliveredtoken; void 
delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, 
MQTTClient_message *message) {
    int i;
    char* payloadptr;
    char* msgTime;
    char* id;
    char* moving;
    char* orientation;
    char* rssi;
    char* temperature;
    char* power;
    char* battery;
    char* currentMotionDuration;
    char* lastMotionDuration;
    char* hwVersion;
    char* fwVersion;
    char* fwState;
    char* x;
    char* y;
    char* z; time_t t = time(NULL); struct tm tm = *localtime(&t);
    printf("Message arrived\n");
    printf(" topic: %s\n", topicName);
    printf(" message: ");
    payloadptr = message->payload;
   /* for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }*/
    msgTime = strtok(payloadptr,",");
    id = strtok(NULL,",");
    moving = strtok(NULL,",");
    orientation = strtok(NULL,",");
    rssi = strtok(NULL,",");
    temperature = strtok(NULL,",");
    power = strtok(NULL,",");
    battery = strtok(NULL,",");
    currentMotionDuration = strtok(NULL,",");
    lastMotionDuration = strtok(NULL,",");
    hwVersion = strtok(NULL,",");
    fwVersion = strtok(NULL,",");
    fwState = strtok(NULL,",");
    x = strtok(NULL,",");
    y = strtok(NULL,",");
    z = strtok(NULL,",");
    if(strcmp(beaconID_1,id) == 0){
	fprintf(fp_1,"%s,%s,%s,%s,%s,%s",msgTime,orientation,x,y,z,ch);
 	printf("%d:%d:%d %s,%s,%s,%s,%s,%s,%s",tm.tm_hour, tm.tm_min, 
tm.tm_sec,beaconID_1,msgTime,orientation,x,y,z,ch);
    }
    else if(strcmp(beaconID_2,id) == 0){
	fprintf(fp_2,"%s,%s,%s,%s,%s,%s",msgTime,orientation,x,y,z,ch);
	printf("%d:%d:%d %s,%s,%s,%s,%s,%s,%s",tm.tm_hour, tm.tm_min, 
tm.tm_sec,beaconID_2,msgTime,orientation,x,y,z,ch);
    }
    else{
	printf("Not in the defined ID range\n");
    }
   // printf("%s,%s,%s,%s,%s",time,x,y,z,ch);
   // putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    fflush(fp_1);
    fflush(fp_2);
    return 1;
}
void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf(" cause: %s\n", cause);
}
int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = 
MQTTClient_connectOptions_initializer;
    int rc;
    fp_1 = fopen("data-1.csv","a");
    fp_2 = fopen("data-2.csv","a");
	
    //Getting the input
	printf("Enter the 1st beacon ID: ");
	scanf("%s",beaconID_1);
	printf("Enter the 2nd beacon ID: ");
        scanf("%s",beaconID_2);
	
     //Header for the files
	fprintf(fp_1,"%s\n",beaconID_1);
	fprintf(fp_2,"%s\n",beaconID_2);
    MQTTClient_create(&client, ADDRESS, CLIENTID, 
MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != 
MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    do
    {
        fgets(ch,50,stdin);
	
	if (ch[0] == 'Q' || ch[0] == 'q'){
		exit(0);
	}
    } while(1);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    fflush(fp_1);
    fflush(fp_2);
    fclose(fp_1);
    fclose(fp_2);
    return rc;
}
