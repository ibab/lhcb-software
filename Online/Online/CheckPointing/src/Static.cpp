typedef unsigned long VA; /* VA = virtual address */


void *mtcp_saved_break = 0;
/* These two are used by the linker script to define the beginning and end of the image.         */
/* The '.long 0' is needed so shareable_begin>0 as the linker is too st00pid to relocate a zero. */

asm (".section __shareable_begin ; .globl mtcp_shareable_begin; .long 0 ; mtcp_shareable_begin:");
asm (".section __shareable_end   ; .globl mtcp_shareable_end; .long 0  ; mtcp_shareable_end:");
asm (".text");

VA     restore_addrspace_begin = 0, restore_addrspace_end = 0;
unsigned long restore_addrspace_size = 0;
