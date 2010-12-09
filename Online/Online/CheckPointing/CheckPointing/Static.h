


extern void *mtcp_saved_break;
extern char mtcp_shareable_end[];
extern char mtcp_shareable_begin[];

typedef unsigned long VA; /* VA = virtual address */
extern VA     restore_addrspace_begin, restore_addrspace_end;
extern unsigned long restore_addrspace_size;
extern  void *const restore_finish_function_pointer;

#define HIGHEST_VA 0x7f00000000
