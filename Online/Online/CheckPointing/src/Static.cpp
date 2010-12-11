#define CHECKPOINTING_STATIC_CPP
#include "CheckPointing/Static.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"
#include <unistd.h>

using namespace CheckPointing;

static const int PROCESS_SYS_MARKER_BEGIN   = *(Marker*)"PSYS";
static const int PROCESS_SYS_MARKER_END     = *(Marker*)"psys";
#define PAGE_SIZE 4096
extern char mtcp_shareable_end[];
extern char mtcp_shareable_begin[];

//void *mtcp_saved_break = 0;
/* These two are used by the linker script to define the beginning and end of the image.         */
/* The '.long 0' is needed so shareable_begin>0 as the linker is too st00pid to relocate a zero. */

asm (".section __shareable_begin ; .globl mtcp_shareable_begin; .long 0 ; mtcp_shareable_begin:");
asm (".section __shareable_end   ; .globl mtcp_shareable_end; .long 0  ; mtcp_shareable_end:");
asm (".text");

namespace CheckPointing {
  SysInfo chkpt_sys;
}


#define STACKSIZE 1024      // size of temporary stack (in quadwords)
/* temp stack used internally by restore so we don't go outside the
 *   libmtcp.so address range for anything;
 * including "+ 1" since will set %esp/%rsp to tempstack+STACKSIZE
 */
static long long tempstack[STACKSIZE + 1];

static void restore_start() {
  /* If we just replace extendedStack by (tempstack+STACKSIZE) in "asm"
   * below, the optimizer generates non-PIC code if it's not -O0 - Gene
   */
  long long * extendedStack = tempstack + STACKSIZE;

  //--

  /* Switch to a stack area that's part of the shareable's memory address range
   * and thus not used by the checkpointed program
   */
  
  asm volatile (CLEAN_FOR_64_BIT(mov %0,%%esp\n\t)
                /* This next assembly language confuses gdb,
		   but seems to work fine anyway */
                CLEAN_FOR_64_BIT(xor %%ebp,%%ebp\n\t)
                : : "g" (extendedStack) : "memory");
  // This should never return
  asm volatile ("hlt");
}

SysInfo::SysInfo() {
  struct rlimit lim;
  startRestore = restore_start;
  finishRestore = 0;
  saved_break = 0;
  addrSize = 0;
  addrStart = 0;
  addrEnd = 0;
  sysInfo = 0;
  pageSize = ::sysconf(_SC_PAGESIZE);
  ::getrlimit(RLIMIT_NOFILE, &lim);
  checkpointFD = lim.rlim_cur-1;
  checkpointFile[0] = 0;
}

void SysInfo::aquire() {
  curr_break = mtcp_sys_brk(0);
  next_break = mtcp_sys_brk(saved_break);

  if (((VA)curr_break > (VA)mtcp_shareable_begin) && ((VA)saved_break < (VA)mtcp_shareable_end)) {
    mtcp_output(MTCP_ERROR,"restore: current_brk %p, saved_break %p, mtcp_shareable_begin %p, mtcp_shareable_end %p\n", 
		curr_break, saved_break, mtcp_shareable_begin, mtcp_shareable_end);
    mtcp_abort ();
  }

  if (next_break == (void*)-1) {
    mtcp_output(MTCP_FATAL,"restore: sbrk(%p): errno: %d (bad heap)\n",saved_break,mtcp_sys_errno);
  }

  /// Stack limit
  ::getrlimit(RLIMIT_STACK, &stack);
  /// Sharable image adress space
  addrStart = (((VA)mtcp_shareable_begin) & -PAGE_SIZE);
  addrSize  = ((VA)mtcp_shareable_end - addrStart + PAGE_SIZE - 1) & -PAGE_SIZE;
  addrEnd   = addrStart + addrSize;
}

void SysInfo::print() {
  mtcp_output(MTCP_INFO,"restore: curr break %p, saved break %p, shareable: begin %p, end %p\n", 
	      curr_break, saved_break, mtcp_shareable_begin, mtcp_shareable_end);

  mtcp_output(MTCP_INFO,"checkpoint: Stack resource limit: soft:%p, hard:%p\n", 
	      stack.rlim_cur, stack.rlim_max);

  mtcp_output(MTCP_INFO,"checkpoint: image of size %X at %p -> %p\n",addrSize, addrStart, addrEnd);

  // The finishRestore function pointer:
  mtcp_output(MTCP_INFO,"checkpoint: finishRestore: %p\n",finishRestore);
}

long SysInfo::write(void* addr) {
  unsigned char* out = (unsigned char*)addr;
  out += saveMarker(out,PROCESS_SYS_MARKER_BEGIN);
  out += saveInt(out,sizeof(SysInfo));
  out += m_memcpy(out,this,sizeof(SysInfo));
  out += saveMarker(out,PROCESS_SYS_MARKER_END);
  return addr_diff(out,addr);
}

long SysInfo::read(const void* addr) {
  int siz;
  const unsigned char* out = (const unsigned char*)addr;
  out += checkMarker(out,PROCESS_SYS_MARKER_BEGIN);
  out += getInt(out,&siz);
  if ( siz != sizeof(SysInfo) ) {
    mtcp_output(MTCP_FATAL,"checkpoint: Inconsistent sysInfo size: %d <-> %d."
		" Checkpointing will not work!\n",siz,(int)sizeof(SysInfo));
  }
  out += m_memcpy(this,out,sizeof(SysInfo));
  out += checkMarker(out,PROCESS_SYS_MARKER_END);
  return addr_diff(out,addr);
}

long SysInfo::length() {
  return 2*sizeof(Marker)+sizeof(int)+sizeof(SysInfo);
}

#include "CheckPointing/MemMaps.h"
#include "CheckPointing/Process.h"

long SysInfo::restore(const void* addr) {
  void** a = (void**)addr;
  if ( addr ) {
    Process* p = (Process*)a[0];
    unsigned char* out = (unsigned char*)a[1];

    void* current_brk = mtcp_sys_brk (NULL);
    if (current_brk > mtcp_shareable_begin && saved_break < mtcp_shareable_end) {
      mtcp_output(MTCP_FATAL,"restore: Brk:curr %p, saved %p, shareable:begin %p, end %p\n", 
		  current_brk, saved_break, mtcp_shareable_begin, mtcp_shareable_end);
    }
    void* new_brk = mtcp_sys_brk(saved_break);
    if (new_brk == (void *)-1) {
      mtcp_output(MTCP_FATAL,"restor: sbrk(%p): errno:  %d (bad heap)\n",saved_break, mtcp_sys_errno );
    }
    if (new_brk != saved_break) {
      if (new_brk == current_brk && new_brk > saved_break)
	mtcp_output(MTCP_DEBUG,"restore: new_brk == current_brk == %p"
		    " saved_break, %p, is strictly smaller; data segment not extended.\n",
		    new_brk, saved_break);
      else {
	mtcp_output(MTCP_FATAL,"restore: error: new break(%p) != saved break (%p)\n",
		    current_brk,saved_break);
      }
    }
    mtcp_output(MTCP_INFO,"current_brk: %p; mtcp_saved_break: %p; new_brk: %p\n",
		current_brk,saved_break,new_brk);
  


    /* Unmap everything except for this image as everything we need
     *   is contained in the libmtcp.so image.
     * Unfortunately, in later Linuxes, it's important also not to wipe
     *   out [vsyscall] if it exists (we may not have permission to remove it).
     *   In any case, [vsyscall] is the highest section if it exists.
     * Further, if the [vdso] when we restart is different from the old
     *   [vdso] that was saved at checkpoint time, then we need to keep
     *   both of them.  The old one may be needed if we're returning from
     *   a system call at checkpoint time.  The new one is needed for future
     *   system calls.
     * Highest_userspace_address is determined heuristically.  Primarily, it
     *   was intended to make sure we don't overwrite [vdso] or [vsyscall].
     *   But it was heuristically chosen as a constant (works for earlier
     *   Linuxes), or as the end of stack.  Probably, we should review that,
     *   and just make it beginning of [vsyscall] where that exists.
     */

    VA holebase = 0, vsyscall_addr = 0, stack_end_addr = 0, highest_va = 0, vdso_addr = 0;
    int rc;

    holebase  = (VA)mtcp_shareable_begin;
    holebase &= -PAGE_SIZE;
    asm volatile (CLEAN_FOR_64_BIT(xor %%eax,%%eax ; movw %%ax,%%fs)
		  : : : CLEAN_FOR_64_BIT(eax)); // the unmaps will wipe what it points to anyway

    MemMaps m;
    AreaInfoHandler h;
    m.scan(h);
    highest_va = h.highAddr;
    vdso_addr  = h.vdso[1];
    vsyscall_addr = h.vsyscall[1];
    stack_end_addr = h.stack[1];


    if (stack_end_addr == 0) // 0 means /proc/self/maps doesn't mark "[stack]"
      highest_va = HIGHEST_VA;
    else
      highest_va = stack_end_addr;

    mtcp_output(MTCP_INFO,"new_brk (end of heap): %p, holebase (libmtcp.so): %p, stack_end: %p\n"
		"    vdso: %p, highest_va: %p, vsyscall: %p\n",
		new_brk, holebase, stack_end_addr,
		vdso_addr, highest_va, vsyscall_addr);

    if (vdso_addr != (VA)NULL && vdso_addr < holebase) {
      mtcp_output(MTCP_INFO,"restore: unmapping %p..%p, %p..%p\n",
		NULL, vdso_addr-1, vdso_addr+PAGE_SIZE, holebase - 1);
      rc = mtcp_sys_munmap (0,vdso_addr);
      rc |= mtcp_sys_munmap (vdso_addr+PAGE_SIZE,(size_t)holebase - vdso_addr - PAGE_SIZE);
    } else {
      mtcp_output(MTCP_INFO,"restore: unmapping 0..%p\n", holebase - 1);
      // ----------------------- rc = mtcp_sys_munmap (NULL,holebase);
    }
    if (rc == -1) {
      mtcp_output(MTCP_FATAL,"munmap: error %d unmap from 0 to %p\n",mtcp_sys_errno,holebase);
    }

    // Unmap from address holebase to highest_va, except for [vdso] section
    // Value of mtcp_shareable_end (end of data segment) can change from before
    holebase  = (VA)mtcp_shareable_end;
    holebase  = (holebase + PAGE_SIZE - 1) & -PAGE_SIZE;
    if (vdso_addr != 0 && vdso_addr + PAGE_SIZE <= highest_va) {
      if (vdso_addr > holebase) {
	mtcp_output(MTCP_INFO,"restore: unmapping %p..%p, %p..%p\n",
		  holebase, vdso_addr-1, vdso_addr+PAGE_SIZE, highest_va - 1);
	rc = mtcp_sys_munmap(holebase, vdso_addr - holebase);
	rc |= mtcp_sys_munmap(vdso_addr + PAGE_SIZE,
			       highest_va - vdso_addr - PAGE_SIZE);
      } else {
	mtcp_output(MTCP_INFO,"restore: unmapping %p..%p\n",holebase,highest_va-1);
	if (highest_va < holebase) {
	  mtcp_output(MTCP_FATAL,"munmap: error unmap: highest_va(%p) < holebase(%p)\n",
		      highest_va, holebase);
	}
	rc = mtcp_sys_munmap((void*)holebase, highest_va - holebase);
      }
    }
    if (rc == -1) {
      mtcp_output(MTCP_FATAL,"munmap: error %d unmap from %p by %p bytes\n",
		  mtcp_sys_errno, holebase, highest_va - holebase);
    }

    if ( (rc=p->readFiles(out+=rc)) ) {
      mtcp_output(MTCP_INFO,"Read %ld bytes of process file info.\n",rc);
      if ( (rc=p->readMemory(out+=rc)) ) {
	mtcp_output(MTCP_INFO,"Read %ld bytes of process memory.\n",rc);
	if ( (rc=p->readTrailer(out+=rc)) ) {
	  // Everything restored, close file and finish up
	  //m.close();
	  mtcp_output(MTCP_INFO,"restore: restore complete, resuming...\n");
	  // Jump to finishrestore in original program's libmtcp.so image
	  (*finishRestore)();
	}
      }
    }
    return 1;
  }
  return 0;
}

