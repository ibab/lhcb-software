// $Id: ITagger.h,v 1.2 2009-07-28 08:47:37 cattanem Exp $
#ifndef TAGGERMUONTOOL_ITAGGERMUONTOOL_H
#define TAGGERMUONTOOL_ITAGGERMUONTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITagger("ITagger", 1 , 0);

/** @class ITagger ITagger.h
 *
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2005-06-30
 */

class ITagger : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITagger; };

  virtual LHCb::Tagger tag( const LHCb::Particle*,
                            const LHCb::RecVertex*,
                            LHCb::Vertex::ConstVector&, 
                            LHCb::Particle::ConstVector&) = 0;

};
#endif // TAGGERMUONTOOL_ITAGGERMUONTOOL_H
