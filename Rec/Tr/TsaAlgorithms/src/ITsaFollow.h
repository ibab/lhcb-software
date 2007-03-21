// $Id: ITsaFollow.h,v 1.2 2007-03-21 14:35:01 cattanem Exp $
#ifndef _ITsaFollow_H
#define _ITsaFollow_H

#include "GaudiKernel/IAlgTool.h"

/** @class ITsaFollow ITsaFollow.h
 *
 *  Fast follower to pick up hits not on space points
 *
 *  @author M.Needham
 *  @date   31/05/2004
 */

static const InterfaceID IID_ITsaFollow( "ITsaFollow", 0, 0 );

namespace Tsa{
  class Parabola;
  class Line;
  class Cluster;
}

class ITsaFollow: virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITsaFollow ; }

  virtual StatusCode execute(const unsigned int stationNum,
                             const Tsa::Parabola& parab, 
                             const Tsa::Line& line, 
                             std::vector<Tsa::Cluster*>& clusVector) = 0;

};

#endif // _ITsaFollow_H 
