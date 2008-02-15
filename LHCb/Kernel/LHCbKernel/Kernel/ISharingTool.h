// $Id: ISharingTool.h,v 1.1 2008-02-15 09:35:41 cattanem Exp $
#ifndef KERNEL_ISHARINGTOOL_H
#define KERNEL_ISHARINGTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class ISharingTool ISharingTool.h Kernel/ISharingTool.h
 *
 *  Interface Class for a sharing tool. Generalisation of ISTChargeSharingTool
 *
 *  @author M.Needham, generalisation by M.Cattaneo
 *  @date   14/3/2002, generalisation on 2008/02/15
 */

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ISharingTool("ISharingTool",0 ,0);

class ISharingTool : virtual public IAlgTool {

public: 

  /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISharingTool; }
   
  /** Calculate the sharing
   *  @param relDist fraction of share
   */
  virtual double sharing(const double relDist) const=0;

};

#endif // KERNEL_ISHARINGTOOL_H
