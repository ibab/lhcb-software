#ifndef KERNEL_INORMALIZETOOL_H
#define KERNEL_INORMALIZETOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_INormalizeTool("INormalizeTool", 1, 1);

/** @class INormalizeTool INormalizeTool.h Kernel/INormalizeTool.h
 *  Interface class for generic normalisation tool
 *
 *  @author Marco Cattaneo
 *  @date   2004-07-14
 */
class INormalizeTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_INormalizeTool;
  }

  /// Return a normalization value
  virtual double normalize() = 0;
};
#endif // KERNEL_INORMALIZETOOL_H
