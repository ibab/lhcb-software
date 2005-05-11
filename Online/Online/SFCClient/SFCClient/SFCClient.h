enum {
SFCC_OK,
SFCC_ERR_PARAM,
SFCC_ERR_CONNECT_FAILED,
SFCC_ERR_DISCONNECTED,
SFCC_ERR_NO_MEM,
SFCC_ERR_BUFFER_TOO_SHORT,
__SFCC_MAX_ERR
};

#define SFCC_L1 0
#define SFCC_HLT 1

#ifdef __cplusplus
  extern "C" {
  typedef const char SFCC_C_CHAR;
#else
  typedef char SFCC_C_CHAR;
#endif



struct sfcc_event_buffer {
        char *buffer;
        int buffer_len;
        int max_len;
};

struct sfcc_decision {
        SFCC_C_CHAR *buffer;
        int buffer_len;
};

int sfcc_register(const char *eth_string, int myport, int level); /* and other parameters */
int sfcc_unregister();
int sfcc_read_length(int *data_length);
int sfcc_read_event(struct sfcc_event_buffer *rbuf);
int sfcc_set_decision(struct sfcc_decision *dec);
int sfcc_push_decision(void);
#ifdef __cplusplus
};
#endif

