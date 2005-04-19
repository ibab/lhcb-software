#define SFCC_ERR_PARAM -1
#define SFCC_ERR_CONNECT_FAILED -2
#define SFCC_ERR_DISCONNECTED -3
#define SFCC_ERR_NO_MEM -4

#define SFCC_L1  0
#define SFCC_HLT 1

#ifdef __cplusplus
  extern "C" {
  typedef const char SFCC_C_CHAR;
#else
  typedef char SFCC_C_CHAR;
#endif

struct sfcc_read_buffer {
	char *buffer;
	int buffer_len;
	int max_len;
};

struct sfcc_send_buffer {
	SFCC_C_CHAR *buffer;
	int buffer_len;
};

struct sfcc_decision  {
  int accept_or_reject;
};

int sfcc_register(SFCC_C_CHAR *hostname, int level);
int sfcc_unregister();
int sfcc_read_length(int* data_length);
int sfcc_read(struct sfcc_read_buffer *rbuf);
int sfcc_send(struct sfcc_send_buffer *sbuf);
int sfcc_set_decision(struct sfcc_decision *decision);
int sfcc_send_decision();

#ifdef __cplusplus
};
#endif
