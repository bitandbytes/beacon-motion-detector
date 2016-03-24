#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
#include "time.h"
#include "pthread.h"
#include "stdbool.h"

#define ADDRESS     "localhost"
#define CLIENTID_1  "iCitySub_1"
#define CLIENTID_2  "iCitySub_2"
#define PAYLOAD     "Hello World!"
#define QOS         2
#define TIMEOUT     10000L
#define NUMB_TOPICS 2

int QOS_2[2] = {2,2};

FILE* fp_1;
FILE* fp_2;

char TOPICS[2][400];
char** TOPICS_PTR = (char**)TOPICS; //remove (char**) if a run time error occured

char ch[50]="\n";
char beaconID_1[100];
char beaconID_2[100];

volatile MQTTClient_deliveryToken deliveredtoken;

/*Link List Block*/
struct data_packet
{
    int val[3];
    struct data_packet *next;
};

struct data_packet *head = NULL;
struct data_packet *curr = NULL;

struct data_packet* create_list(int* val)
{
    printf("\n creating list with headnode as [%d]\n",val[0]);
    struct data_packet *ptr = (struct data_packet*)malloc(sizeof(struct data_packet));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->val[0] = val[0];
    ptr->val[1] = val[1];
    ptr->val[2] = val[2];
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

struct data_packet* add_to_list(int* val, bool add_to_end)
{

    add_to_end = true; //Always add to the end

    if(NULL == head)
    {
        return (create_list(val));
    }

    if(add_to_end)
        printf("\n Adding node to end of list with value [%d]\n",val[0]);
    else
        printf("\n Adding node to beginning of list with value [%d]\n",val[0]);

    struct data_packet *ptr = (struct data_packet*)malloc(sizeof(struct data_packet));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->val[0] = val[0];
    ptr->val[1] = val[1];
    ptr->val[2] = val[2];
    ptr->next = NULL;

    if(add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head = ptr;
    }
    return ptr;
}

struct data_packet* delete_list(struct data_packet **prev)
{

    printf("\nDeleting the list\n");
    struct data_packet *tmp = head->next;

//Delete the list except for the last element (curr)
    while(head != curr)
    {
	free(head);
	head = tmp;
	tmp = head->next;
    }

}

void print_list(void)
{
    struct data_packet *ptr = head;

    printf("\n -------Printing list Start------- \n");
    while(ptr != NULL)
    {
        printf("\n [%d],[%d],[%d] \n",ptr->val[0],ptr->val[1],ptr->val[2]);
        ptr = ptr->next;
    }
    printf("\n -------Printing list End------- \n");

    return;
}
//MQTT Block
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *fp, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    int val[3];
    char* x;
    char* y;
    char* z;
    char* tmp;
    char* payloadptr;
    int flag = 0;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    printf("%d:%d:%d ",tm.tm_hour,tm.tm_min,tm.tm_sec);
//    fprintf(fp,"%d%d%d,",tm.tm_hour,tm.tm_min,tm.tm_sec);
    payloadptr = message->payload;

    tmp = strtok(payloadptr,":");
    x = strtok(NULL,",");
    tmp = strtok(NULL,":");
    y = strtok(NULL,",");
    tmp = strtok(NULL,":");
    z = strtok(NULL,"}");
 
payloadptr = message->payload;
/*for(i=0; i < message->payloadlen; i++)
    {
	putchar(*payloadptr++);
    }
*/
/*    val[0] = atoi(strtok(payloadptr,":,"));
    val[1] = atoi(strtok(NULL,":,"));
    val[2] = atoi(strtok(NULL,":}"));
*/
   printf(" X:%s, Y:%s, Z:%s\n",x,y,z);
//######################################################################
//TODO convert the values to integer and add the values to the list
//######################################################################

//   add_to_list(val,true);

/* Old file print block
    for(i=0; i < message->payloadlen; i++)
    {
	if( *payloadptr == ':'){
		flag = 1;
		payloadptr++;
	}
	else if( *payloadptr == '}' ){
		flag = 2;
	}
	else if( *payloadptr == ',' ){
		flag = 0;
		putchar(*payloadptr);
		putc(*payloadptr,fp);
	}

	if (flag == 1){ 
		putchar(*payloadptr);
		putc(*payloadptr,fp);
	}
	payloadptr++;

//	putchar(*payloadptr);
    }
*/

/*    if(strcmp(beaconID_1,id) == 0){
	fprintf(fp_1,"%s,%s,%s,%s,%s,%s",msgTime,orientation,x,y,z,ch);
 	printf("%d:%d:%d %s,%s,%s,%s,%s,%s,%s",tm.tm_hour, tm.tm_min, tm.tm_sec,beaconID_1,msgTime,orientation,x,y,z,ch);
    }
    else if(strcmp(beaconID_2,id) == 0){
	fprintf(fp_2,"%s,%s,%s,%s,%s,%s",msgTime,orientation,x,y,z,ch);
	printf("%d:%d:%d %s,%s,%s,%s,%s,%s,%s",tm.tm_hour, tm.tm_min, tm.tm_sec,beaconID_2,msgTime,orientation,x,y,z,ch);
    }
    else{
	printf("Not in the defined ID range\n");
    }
*/
   // printf("%s,%s,%s,%s,%s",time,x,y,z,ch);
   // putchar('\n');
  //  putc('\n',fp);
  //  printf(",%s",ch);
 //   fprintf(fp,",%s",ch);
 //   fflush(fp);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
   // fflush(fp_1);
   // fflush(fp_2);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}
//Thread 1 function
void* MQTTClient_multiSubscribe_1(void* arg)
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;

	MQTTClient_create(&client, ADDRESS, CLIENTID_1, MQTTCLIENT_PERSISTENCE_NONE,NULL);
	conn_opts.keepAliveInterval = 60;
	conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(client,fp_1, connlost, msgarrvd, delivered);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    	{
        	printf("Failed to connect, return code %d\n", rc);
        	exit(-1);
    	}

	printf("Thread 1 subscribing to topic: %s\n",TOPICS[0]);
	MQTTClient_subscribe(client, TOPICS[0], QOS_2[0]);

	do
        {
                fgets(ch,50,stdin);

                if (ch[0] == 'Q' || ch[0] == 'q'){
                        break;
                }

        } while(1);

	printf("Thread 1 ended\n");

	MQTTClient_disconnect(client, 10000);
    	MQTTClient_destroy(&client);
}

//Thread 2 function
void* MQTTClient_multiSubscribe_2(void* arg)
{
        MQTTClient client;
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        int rc;

        MQTTClient_create(&client, ADDRESS, CLIENTID_2, MQTTCLIENT_PERSISTENCE_NONE,NULL);
        conn_opts.keepAliveInterval = 60;
        conn_opts.cleansession = 1;

        MQTTClient_setCallbacks(client,fp_2, connlost, msgarrvd, delivered);

        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
        {
                printf("Failed to connect, return code %d\n", rc);
                exit(-1);
        }
	printf("Thread 2 subscribing to topic: %s\n",TOPICS[1]);

        MQTTClient_subscribe(client, TOPICS[1], QOS_2[1]);

	do
    	{
        	fgets(ch,50,stdin);

        	if (ch[0] == 'Q' || ch[0] == 'q'){
                	break;
        	}

    	} while(1);
	printf("Thread 2 ended\n");
	MQTTClient_disconnect(client, 10000);
    	MQTTClient_destroy(&client);
}

int main(int argc, char* argv[])
{
    pthread_t thread1, thread2;

//    fp_1 = fopen("data-1.csv","a");
//    fp_2 = fopen("data-2.csv","a");
    //Getting the input
	printf("Enter the 1st beacon ID: ");
	scanf("%s",&beaconID_1);
	printf("Enter the 2nd beacon ID: ");
       scanf("%s",&beaconID_2);

	//Segmentation fault due to the fp_1 and fp_2
     //Header for the files
//	fprintf(fp_1,"%s\n","ddfgd");
//	fprintf(fp_2,"%s\n","sdf");
    //Customizing the topic
	sprintf(TOPICS[0],"bbb/sensor/sticker/nearable/%s/acceleration/",beaconID_1);
	sprintf(TOPICS[1],"bbb/sensor/sticker/nearable/%s/acceleration/",beaconID_2);

    printf("Subscribing to topics\n%s\n%s\nFor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPICS[0], TOPICS[1], CLIENTID_1, QOS);
//    MQTTClient_subscribeMany(client,NUMB_TOPICS, TOPICS_PTR, QOS_2);
//      MQTTClient_subscribe(client, TOPICS[0], QOS_2[0]);

    if( pthread_create(&thread1,NULL,MQTTClient_multiSubscribe_1,NULL) != 0 )
    {
	printf("Error in creating the thread\n");
	exit(1);
    }

    if( pthread_create(&thread2,NULL,MQTTClient_multiSubscribe_2,NULL) != 0 )
    {
        printf("Error in creating the thread\n");
        exit(1);
    }

    do 
    {
        fgets(ch,50,stdin);
	
	if (ch[0] == 'Q' || ch[0] == 'q'){
		break;
	}

    } while(1);
    printf("Programe Ended\n");
    fflush(fp_1);
    fflush(fp_2);
    fclose(fp_1);
    fclose(fp_2);
    return 0;
}
