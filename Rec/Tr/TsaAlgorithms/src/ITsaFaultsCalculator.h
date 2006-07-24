#ifndef _ITsaFaultsCalculator_H
#define _ITsaFaultsCalculator_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITsaFaultsCalculator( "ITsaFaultsCalculator", 0, 0 );

/** @class ITsaFaultsCalculator 
 *
 *  How many hits do we expect in a gven detector
 *
 *  @author M.Needham
 *  @date   31/05/2004
 */

namespace Tsa{
  class Parabola;
  class Line;
  class Cluster;
}

class ITsaFaultsCalculator: virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITsaFaultsCalculator ; }

  virtual double nFaults(const Tsa::Parabola& parab,
                         const Tsa::Line& line,
                         const std::vector<Tsa::Cluster*>& clusters) const = 0;

}; 


#endif // _ITsaFaultsCalculator_H

