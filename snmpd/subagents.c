/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.int_watch.conf,v 5.0 2002/04/20 07:30:13 hardaker Exp $
 * hey.
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <stdlib.h>
/*#include <string>
using namespace std;*/
#include "subagents.h"

//using namespace std;
#define MAX_STR_LEN 256
/*---------------------------------------------------------------------------------------------------*/

/* Make sure those next includes files are at this path, and that you've got the files */

#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"

/*---------------------------------------------------------------------------------------------------*/
//Redis connection
static redisContext *c;
redisContext **con = &c;

/*-----------------------------------------------------------------------------------*/
/*                                                                                   */
/* Every commented "DEBUGMSGTL" had exchanged with snmp_log, it's a way of debugging */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/


void connectToRedis(void)
{
    const char *hostname = "redis_db"; // Should be the name of the container where redis at
    int port = 6379; // Redis interacts at port 6379
    struct timeval timeout = { 1, 50000 }; // 1.5 seconds
    c = redisConnectWithTimeout(hostname, port, timeout); //Calling the connection function @hiredis/hiredis.c

    snmp_log(LOG_ERR, "Connecting to redis...\n");
    // Didn't connect to redis
    if(c == NULL || c->err)
    {
     	if(c) // Which means c->err
	{
            snmp_log(LOG_ERR, "connection error: %s\n", c->errstr);
            //DEBUGMSGTL(("batteryObject", "Connection error: %s\n", c->errstr));
            redisFree(c);
        }
        else // Which means c ==NULL
        {
            snmp_log(LOG_ERR, "connection error: can't allocate redis context\n");
            //DEBUGMSGTL(("batteryObject", "Connection error: can't allocate redis context\n"));
        }
    }
    else // Connected successfully
    {
        snmp_log(LOG_ERR, "redis connection successful\n");
     	//DEBUGMSGTL(("batteryObject", "Connection Successful\n"));
    }
}


void shutdownRedis(void)
{
    redisFree(c);
}

/*
 * the variable we want to tie an OID to.  The agent will handle all
 * GET and SET requests to this variable changing it's value as needed.
 */

static int currentObject_value = 0;
static int batteryObject_value = 4; // The first value of the mib. (1-100)
static int channelObject_value = 1; // The first value of the mib. (1-12)
static char isAliveObject_value[MAX_STR_LEN] = "ALIVE"; // The first value of the mib. ("ALIVE"/"DYING"/"DEAD")
//static string isAliveObject_value = "ALIVE"; // The first value of the mib. ("ALIVE"/"DYING"/"DEAD")
static enum object_mode { battery_mode, channel_mode, current_mode, isAlive_mode};
/** Initializes the sub_agent_test module */
void
init_subagents(void)
{
//                                      The whole oid as in the MY-TUTORIAL-MIB.txt, without the .0 at the end.
//                                                                         |
    static oid batteryObject_oid[] = { 1, 3, 6, 1, 4, 1, 8072, 2, 4, 1, 1, 4 }; //   modem's "variable"

    static oid channelObject_oid[] = { 1, 3, 6, 1, 4, 1, 8072, 2, 4, 1, 1, 5 }; //   modem's "variable"

    static oid currentObject_oid[] = { 1, 3, 6, 1, 4, 1, 8072, 2, 4, 1, 1, 6 }; // program's "variable"

    static oid isAliveObject_oid[] = { 1, 3, 6, 1, 4, 1, 8072, 2, 4, 1, 1, 7 }; //   modem's "variable"
    //DEBUGMSGTL(("batteryAgentSubagentObject", "Initializing\n"));

    // Setting the current modem's id value in redis for the first time
    SET_objects_redis(current_mode); 

    // Setting the battery value in redis for the first time
    //SET_objects_redis(battery_mode);

    // Setting the channel value in redis for the first time
    //SET_objects_redis(channel_mode);

    // Registering the mib-handler, thus making the subagent able to maintain the battery mib
    netsnmp_register_handler(
        netsnmp_create_handler_registration("batteryObject", handle_batteryObject,
                                            batteryObject_oid, OID_LENGTH(batteryObject_oid),
                                            HANDLER_CAN_RWRITE
        ));

    // Registering the mib-handler, thus making the subagent able to maintain the channel mib
    netsnmp_register_handler(
        netsnmp_create_handler_registration("channelObject", handle_channelObject,
                                            channelObject_oid, OID_LENGTH(channelObject_oid),
                                            HANDLER_CAN_RWRITE
        ));

    // Registering the mib-handler, thus making the subagent able to maintain the current mib
    netsnmp_register_handler(
        netsnmp_create_handler_registration("currentObject", handle_currentObject,
                                            currentObject_oid, OID_LENGTH(currentObject_oid),
                                            HANDLER_CAN_RWRITE
        ));

    // Registering the mib-handler, thus making the subagent able to maintain the isAlive mib
    netsnmp_register_handler(
        netsnmp_create_handler_registration("isAliveObject", handle_isAliveObject,
                                            isAliveObject_oid, OID_LENGTH(isAliveObject_oid),
                                            HANDLER_CAN_RWRITE
        ));
}

int
handle_batteryObject(netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info   *reqinfo,
                               netsnmp_request_info         *requests)
{
    int ret;
    /* we are never called for a GETNEXT if it's registered as a
       "instance", as it's "megically" handled for us. */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */

    switch(reqinfo->mode)
    {
     	case MODE_GET: // snmpget
            GET_objects_redis(battery_mode);
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     &batteryObject_value,
                                     sizeof(batteryObject_value));
            break;

        case MODE_SET_RESERVE1:
            break;

        case MODE_SET_RESERVE2:
            break;

        case MODE_SET_FREE:
            break;

        case MODE_SET_COMMIT:
            break;

        case MODE_SET_UNDO:
             break;

        case MODE_SET_ACTION: // snmpset
            /* perform the value change here */

            if(requests->requestvb->type != ASN_INTEGER) // If it's not an integer
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGTYPE);
                return SNMP_ERR_WRONGTYPE;
            }

            if(*requests->requestvb->val.integer < 0 || *requests->requestvb->val.integer > 100) // If battery is not between 0-100
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGVALUE);
                return SNMP_ERR_WRONGVALUE;
            }
            batteryObject_value = *requests->requestvb->val.integer; // Setting batteryObject_value as an preparation for the next function
            SET_objects_redis(battery_mode); // Calling the fucntion which takes care of redis' setting
            break;

        default:
             /* we should never get here, so this is a really bad error */
             snmp_log(LOG_ERR, "unknown mode (%d) in handle_batteryObject\n", reqinfo->mode);
             return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

int
handle_channelObject(netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info   *reqinfo,
                               netsnmp_request_info         *requests)
{
    int ret;
    /* we are never called for a GETNEXT if it's registered as a
       "instance", as it's "megically" handled for us. */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */

    switch(reqinfo->mode)
    {
     	case MODE_GET: // snmpget
            GET_objects_redis(channel_mode);
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     &channelObject_value,
                                     sizeof(channelObject_value));
            break;

        case MODE_SET_RESERVE1:
            break;

        case MODE_SET_RESERVE2:
            break;

        case MODE_SET_FREE:
            break;

        case MODE_SET_COMMIT:
            break;

        case MODE_SET_UNDO:
             break;

        case MODE_SET_ACTION: // snmpset
            /* perform the value change here */
            if(requests->requestvb->type != ASN_INTEGER) // If it's not an integer
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGTYPE);
                return SNMP_ERR_WRONGTYPE;
            }

            if(*requests->requestvb->val.integer < 0 || *requests->requestvb->val.integer > 12) // If channel is not between 0-12
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGVALUE);
                return SNMP_ERR_WRONGVALUE;
            }
            channelObject_value = *requests->requestvb->val.integer; // Setting channelObject_value as an preparation for the next function
            SET_objects_redis(channel_mode); // Calling the fucntion which takes care of redis' setting
            break;

        default:
             /* we should never get here, so this is a really bad error */
             snmp_log(LOG_ERR, "unknown mode (%d) in handle_channelObject\n", reqinfo->mode);
             return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}


int
handle_currentObject(netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info   *reqinfo,
                               netsnmp_request_info         *requests)
{
    int ret;
    /* we are never called for a GETNEXT if it's registered as a
       "instance", as it's "megically" handled for us. */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */

    switch(reqinfo->mode)
    {
     	case MODE_GET: // snmpget
            GET_objects_redis(current_mode);
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     &currentObject_value,
                                     sizeof(currentObject_value));
            break;

        case MODE_SET_RESERVE1:
            break;

        case MODE_SET_RESERVE2:
            break;

        case MODE_SET_FREE:
            break;

        case MODE_SET_COMMIT:
            break;

        case MODE_SET_UNDO:
             break;

        case MODE_SET_ACTION: // snmpset
            /* perform the value change here */

            if(requests->requestvb->type != ASN_INTEGER) // If it's not an integer
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGTYPE);
                return SNMP_ERR_WRONGTYPE;
            }

            /*if(*requests->requestvb->val.integer < 0 || *requests->requestvb->val.integer > 12) // If channel is not between 0-12
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGVALUE);
                return SNMP_ERR_WRONGVALUE;
            }*/
            
            currentObject_value = *requests->requestvb->val.integer; // Setting currentObject_value as an preparation for the next function
            SET_objects_redis(current_mode); // Calling the fucntion which takes care of redis' setting
            break;

        default:
             /* we should never get here, so this is a really bad error */
             snmp_log(LOG_ERR, "unknown mode (%d) in handle_currentObject\n", reqinfo->mode);
             return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}



int
handle_isAliveObject(netsnmp_mib_handler *handler,
                               netsnmp_handler_registration *reginfo,
                               netsnmp_agent_request_info   *reqinfo,
                               netsnmp_request_info         *requests)
{
    int ret;
    /* we are never called for a GETNEXT if it's registered as a
       "instance", as it's "megically" handled for us. */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */

    switch(reqinfo->mode)
    {
     	case MODE_GET: // snmpget
            GET_objects_redis(isAlive_mode);
            snmp_set_var_typed_value(requests->requestvb,  ASN_OCTET_STR,
                                     isAliveObject_value,
                                     sizeof(isAliveObject_value));
            break;

        case MODE_SET_RESERVE1:
            break;

        case MODE_SET_RESERVE2:
            break;

        case MODE_SET_FREE:
            break;

        case MODE_SET_COMMIT:
            break;

        case MODE_SET_UNDO:
             break;

        case MODE_SET_ACTION: // snmpset
            /* perform the value change here */
            if(requests->requestvb->type != ASN_OCTET_STR) // If it's not a string
            {
             	netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGTYPE);
                return SNMP_ERR_WRONGTYPE;
            }
            strncpy(isAliveObject_value, requests->requestvb->val.string, MAX_STR_LEN);
	    isAliveObject_value[MAX_STR_LEN - 1] = '\0';
            //isAliveObject_value = *requests->requestvb->val.string; // Setting isAliveObject_value as an preparation for the next function
            SET_objects_redis(isAlive_mode); // Calling the fucntion which takes care of redis' setting
            break;

        default:
             /* we should never get here, so this is a really bad error */
             snmp_log(LOG_ERR, "unknown mode (%d) in handle_isAliveObject\n", reqinfo->mode);
             return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}


char* concat(char* a, char* b)
{
    snmp_log(LOG_ERR, "#1\n");
    char* total = malloc (strlen(a) + strlen(b) + 1);
    snmp_log(LOG_ERR, "#2\n");
    snmp_log(LOG_ERR, "%s|%s\n", a, b);
    fprintf(total, "%s%s\0", a, b);
    snmp_log(LOG_ERR, "#3\n");
    return &total;
}

void GET_objects_redis(int mode) // Manages snmpget
{   // mode: 0 - battery
    //       1 - channel
    //       2 - current
    //       3 - isAlive
    //GET LOCAL MANAGE INFO
    //char next_command[MAX_STR_LEN];
    

    redisReply *reply;
    reply = redisCommand(c, "GET currentObjectField");
    const char *line1 = "HGET modem_NO_";
    const char *line2 = reply->str;
    size_t len1 = strlen(line1);
    size_t len2 = strlen(line2);
    const char *line3;
    size_t len3;
    char *totalLine;
    if(mode == battery_mode)
    {
        line3 = " batteryObjectField";
    }
    else if(mode == channel_mode)
    {
        line3 = " channelObjectField";
    } else if (mode == isAlive_mode)
    {
        line3 = " isAliveObjectField";
    }
    
    len3 = strlen(line3);
    totalLine = malloc(len1 + len2 + len3 + 1);
    if (!totalLine) abort();

    memcpy(totalLine,               line1, len1);
    memcpy(totalLine + len1,        line2, len2);
    memcpy(totalLine + len1 + len2, line3, len3);
    totalLine[len1 + len2 + len3] = '\0';

    // Normal redis get command, getting batteryObjectField from redis server
    reply = redisCommand(c, totalLine);
    snmp_log(LOG_ERR, "reply  snmpget: %s\n", reply->str);

    if(mode == battery_mode)
    { batteryObject_value = atoi(reply->str); }
    else if(mode == channel_mode)
    { channelObject_value = atoi(reply->str); }
    else if(mode == current_mode)
    { currentObject_value = atoi(reply->str); }
    else
    { strncpy(isAliveObject_value, reply->str, MAX_STR_LEN);
      isAliveObject_value[MAX_STR_LEN - 1] = '\0'; }
    freeReplyObject(reply);
}

void SET_objects_redis(int mode)
{
    redisReply *reply;
    if(mode == battery_mode)
    {
        // Normal redis set command, setting batteryObjectField of currentObject_value from redis server
        reply = (redisReply*)(redisCommand(*con, "HSET modem_NO_%d batteryObjectField %d", currentObject_value, batteryObject_value));
    }
    else if(mode == channel_mode)
    {
        // Normal redis set command, setting channelObjectField of currentObject_value from redis server
        reply = (redisReply*)(redisCommand(*con, "HSET modem_NO_%d channelObjectField %d",  currentObject_value, channelObject_value));
    }
    else if(mode == current_mode)
    {
        // Normal redis set command, setting currentObjectField from redis server
        reply = (redisReply*)(redisCommand(*con, "SET currentObjectField %d",  currentObject_value));
    }
    else
    {
        // Normal redis set command, setting isAliveObjectField from redis server
        reply = (redisReply*)(redisCommand(*con, "HSET modem_NO_%d isAliveObjectField %s", currentObject_value, isAliveObject_value));
    }

    if(reply->type == REDIS_REPLY_ERROR)
    {
        snmp_log(LOG_ERR, "SET error: %s\n", reply->str);
     	//DEBUGSGTL(("batteryObject", "SET error: %s\n", reply->str));
    }
    freeReplyObject(reply);
}

