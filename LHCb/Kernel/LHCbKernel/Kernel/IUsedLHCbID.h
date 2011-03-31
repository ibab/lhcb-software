#ifndef _IUsedLHCbID_H
#define _IUsedLHCbID_H

#include "GaudiKernel/IAlgTool.h"
#include "Kernel/LHCbID.h"

static const InterfaceID IID_IUsedLHCbID( "IUsedLHCbID", 0, 0 );

/** @class IUsedLHCbID IUsedLHCbID.h Kernel/IUsedLHCbID.h
 *
 *  interface for testing if a given id is used.
 *
 *  @author M.Needham
 *  @date   02/08/2006
 */

class IUsedLHCbID: virtual public IAlgTool{

public:

  /** Retrieve interface ID */
  static const InterfaceID& interfaceID() { return IID_IUsedLHCbID ; }

  /** Test if the LHCbID is used
  * @param id to be test tested
  *  @return true if used
  */
  virtual bool used(const LHCb::LHCbID id) const = 0;

};

#endif
