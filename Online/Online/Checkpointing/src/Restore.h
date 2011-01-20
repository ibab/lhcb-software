#ifndef CHECKPOINTING_RESTORE_H
#define CHECKPOINTING_RESTORE_H

#include "Checkpointing/Namespace.h"
/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE  {

  struct FileDesc;

  STATIC(int) checkpoint_file_reopen(FileDesc* d);
  STATIC(int) checkpoint_file_fread(FileDesc* d, int fd, bool restore);
  STATIC(int) checkpoint_file_read(FileDesc* d, const void* addr, bool restore);

  struct Process;
  STATIC(int) checkpointing_process_read_header(Process*, const void* addr);
  STATIC(int) checkpointing_process_read_trailer(Process*, const void* addr);
  STATIC(int) checkpointing_process_fread_header(Process*,int fd);
  STATIC(int) checkpointing_process_fread_trailer(Process*,int fd);
  STATIC(int) checkpointing_process_skip_sys(Process*,const void* addr);
  STATIC(int) checkpointing_process_read_files(Process*,const void* addr);

  STATIC(int) checkpointing_process_read_memory(Process*, const void* addr);
  STATIC(int) checkpointing_process_read(Process* p, const void* addr);

  STATIC(int) checkpointing_process_fskip_sys(Process*,int fd);
  STATIC(int) checkpointing_process_fread_files(Process*,int fd);
  STATIC(int) checkpointing_process_fread_memory(Process*,int fd);
  STATIC(int) checkpointing_process_fread(Process* p, int fd);

  struct AreaHandler;
  struct Area;
  STATIC(int) checkpointing_process_map_memory(const AreaHandler* ,const Area& a, const unsigned char* data, int data_len);
  STATIC(int) checkpointing_process_fmap_memory(const AreaHandler* ,const Area& a, int fd, int data_len);
  STATIC(int) checkpointing_area_datalength(const Area* a);
  STATIC(void) checkpointing_area_print(const Area* a,int lvl,const char* opt);
  STATIC(int) checkpointing_area_read(Area* a, const void* addr, int (*map)(const AreaHandler*, const Area& a,const unsigned char*,int), const AreaHandler* handler);

  STATIC(int) checkpointing_area_fread(Area* a,int fd, int (*map)(const AreaHandler*, const Area& a,int fd,int), const AreaHandler* handler);
  STATIC(int) checkpointing_area_protection(const Area* a);
  STATIC(int) checkpointing_area_mapFlags(const Area* a);

  STATIC(int) checkpointing_area_map(const Area& a,int fd_in,const unsigned char* in,int data_len);

  struct SysInfo;

  STATIC(void) checkpointing_sys_print(const SysInfo& s);
  STATIC(void) checkpointing_sys_restore_start(SysInfo* sys,int print_level);
  STATIC(void) checkpointing_sys_restore_finish();
  STATIC(void) checkpointing_restore_process();

  // CHECKPOINTING_NAMESPACE::
}
#endif  // CHECKPOINTING_RESTORE_H
