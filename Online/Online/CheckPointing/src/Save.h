#ifndef CHECKPOINTING_SAVE_H
#define CHECKPOINTING_SAVE_H

#include "Checkpoining/Namespace.h"
/*
 * Checkpoining namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE  {

  struct SysInfo;
  STATIC(long) checkpointing_sys_fwrite(int fd, const SysInfo* s);

  // CHECKPOINTING_NAMESPACE::
}
#endif  // CHECKPOINTING_SAVE_H
