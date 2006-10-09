// $Id: ITagger.h,v 1.4 2006-10-09 07:21:45 jpalac Exp $
#ifndef TAGGERMUONTOOL_ITAGGERMUONTOOL_H
#define TAGGERMUONTOOL_ITAGGERMUONTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "Kernel/DVAlgorithm.h"
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
		      std::vector<const LHCb::Vertex*>&, LHCb::Particle::ConstVector&) = 0;

};
#endif // TAGGERMUONTOOL_ITAGGERMUONTOOL_H
