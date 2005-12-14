// $Id: IRndmTool.h,v 1.1 2005-12-14 13:25:04 cattanem Exp $
#ifndef KERNEL_IRNDMTOOL_H 
#define KERNEL_IRNDMTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRndmTool("IRndmTool", 1, 1);

/** @class IRndmTool IRndmTool.h Kernel/IRndmTool.h
 *  Interface class for tools that manipulate random numbers
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-14
 */
class IRndmTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IRndmTool;
  }

  /// Initialize with an unsigned int and an unsigned 64 bit seed
  virtual void initRndm( unsigned int, ulonglong ) = 0;

};
#endif // KERNEL_INORMALIZETOOL_H
