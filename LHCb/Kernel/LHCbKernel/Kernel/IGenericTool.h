// $Id: IGenericTool.h,v 1.1 2005-12-14 13:25:04 cattanem Exp $
#ifndef KERNEL_IGENERICTOOL_H 
#define KERNEL_IGENERICTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IGenericTool("IGenericTool", 1, 1);

/** @class IGenericTool IGenericTool.h Kernel/IGenericTool.h
 *  General purpose interface class for tools that "do something"
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-14
 */
class IGenericTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IGenericTool;
  }

  /// Do the action
  virtual void execute() = 0;
};
#endif // KERNEL_INORMALIZETOOL_H
