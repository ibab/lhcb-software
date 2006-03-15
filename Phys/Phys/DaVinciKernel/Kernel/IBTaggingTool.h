#ifndef BTAGGINGTOOL_IBTAGGINGTOOL_H
#define BTAGGINGTOOL_IBTAGGINGTOOL_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/FlavourTag.h"

static const InterfaceID IID_IBTaggingTool("IBTaggingTool", 1 , 0);

/** @class IBTaggingTool IBTaggingTool.h
 *
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2004-12-14
 */

class IBTaggingTool : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IBTaggingTool; };

  virtual StatusCode tag( LHCb::FlavourTag&, const LHCb::Particle* ) = 0;
  virtual StatusCode tag( LHCb::FlavourTag&, const LHCb::Particle*, const  LHCb::Vertex* ) = 0;
  virtual StatusCode tag( LHCb::FlavourTag&, const LHCb::Particle*, const  LHCb::Vertex*,
			   LHCb::Particle::ConstVector& ) = 0;
};
#endif // BTAGGINGTOOL_IBTAGGINGTOOL_H
