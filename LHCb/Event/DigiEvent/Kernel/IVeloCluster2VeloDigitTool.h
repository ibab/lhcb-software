// $Id: IVeloCluster2VeloDigitTool.h,v 1.2 2006-03-06 09:49:53 cattanem Exp $
#ifndef KERNEL_IVELOCLUSTER2VELODIGITTOOL_H 
#define KERNEL_IVELOCLUSTER2VELODIGITTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/VeloDigit.h"
#include "GaudiKernel/SmartRefVector.h"

static const InterfaceID IID_IVeloCluster2VeloDigitTool ( "IVeloCluster2VeloDigitTool", 1, 0 );

/** @class IVeloCluster2VeloDigitTool IVeloCluster2VeloDigitTool.h 
 *         VeloAssociators/IVeloCluster2VeloDigitTool.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-01
 */

namespace LHCb
{
  class VeloCluster;
}

class IVeloCluster2VeloDigitTool : virtual public IAlgTool {

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IVeloCluster2VeloDigitTool; }

  virtual SmartRefVector<LHCb::VeloDigit> getDigits(
                                          LHCb::VeloCluster* cluster,
                                          LHCb::VeloDigits* digitCont) = 0;

protected:

private:

};
#endif // KERNEL_IVELOCLUSTER2VELODIGITTOOL_H
