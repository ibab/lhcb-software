#include <sys/types.h>
#define SOCK_LHCB 6
#define IPPROTO_MEP 0xf1
#define IPPROTO_EVENT 0xfe

typedef struct mep_hdr {
	u_int32_t	l01_id;
	u_int16_t	total_length;
	u_int16_t	number_of_events;
	u_int32_t	partition_id;
} mep_hdr_t;

typedef struct struct ef_hdr {
	u_int16_t	event_id;
	u_int16_t	length;
	char data[0];
} ef_hdr_t;
