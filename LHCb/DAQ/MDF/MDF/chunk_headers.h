#ifndef CHUNK_HEADERS_H_
#define CHUNK_HEADERS_H_

#include <sys/types.h>

/**
 * Maximum length of a file name in bytes.
 */
#define MAX_FILE_NAME 256

/**
 * Sent with a command to indicate a chunk is being sent.
 */
struct cmd_chunk_header {
	unsigned int seq_num;
	unsigned int size;
	off_t offset;
} __attribute__((__packed__));

/**
 * Sent with a command to indicate a new file open is requested.
 */
struct cmd_start_header {
	unsigned int seq_num;
} __attribute__((__packed__));

/**
 * Sent with a command to indicate a file is being closed.
 */
struct cmd_stop_header {
	unsigned int seq_num;
	unsigned char md5_sum[16];
	unsigned int adler32_sum;
} __attribute__((__packed__));


#define CMD_OPEN_FILE	0x01
#define	CMD_CLOSE_FILE	0x02
#define CMD_WRITE_CHUNK	0x03

/**
 * A header for a command which specifies an operation.
 */
struct cmd_header {
	unsigned int cmd;
	unsigned char file_name[MAX_FILE_NAME];
	union {
		struct cmd_start_header 	start_data;
		struct cmd_stop_header 		stop_data;
		struct cmd_chunk_header		chunk_data;
	} data;
} __attribute__((__packed__));


/**
 * An acknowledgement struct sent by the server.
 * This struct is sent out to the sender to acknowledge
 * the range of sequence numbers has been processed.
 */
struct ack_header {
	unsigned int min_seq_num;
	unsigned int max_seq_num;
} __attribute__((__packed__));

#endif
