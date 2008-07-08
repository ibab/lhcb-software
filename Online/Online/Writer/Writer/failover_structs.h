#include "sys/types.h"
/**
 * \file failover_structs.h
 * \author Sai Suman <Sai.Suman.Cherukuwada@cern.ch>
 * This file has structs that define the format of messages that
 * pass between the failover service and client.
 */

#ifndef FAILOVER_STRUCTS_H_
#define FAILOVER_STRUCTS_H_

#define NODE_JOINED		0x01
#define NODE_LEFT		0x00

/**
 * A structure that defines a message sent by the failover service.
 */
struct failover_msg {
	__uint32_t msg_type;	/**< One of the NODE_XXX values.*/
	__uint32_t num_nodes; /**< The number of node states that will be sent.*/
} __attribute__((__packed__));

/**
 * A structure used to maintain load information.
 * One such struct exists for each server in the
 * storage cluster. Should definitely
 * hold the total number of active connections,
 * can also hold other things such as disk space,
 * etc.
 */
struct load {
	__uint32_t l_connections;	/**<< Current active TCP/IP connections.*/
} __attribute__((__packed__));


/**
 * A structure to maintain information on nodes and their current states.
 */
struct nodestate {
	__uint32_t 	n_ipaddr;	/**<< The IP address of this node.*/
	__uint64_t	n_id;		/**<< A somewhat unique ID for the node.*/
	__uint32_t	n_last_ka;		/**<< When did we last hear from it?*/
	struct load	n_load;			/**<< What sort of load does it have?*/
} __attribute__((__packed__));

#endif /*FAILOVER_STRUCTS_H_*/
