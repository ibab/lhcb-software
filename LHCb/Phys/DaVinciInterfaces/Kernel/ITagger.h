// $Id: ITagger.h,v 1.4 2009-08-04 09:45:31 jpalac Exp $
#ifndef TAGGERMUONTOOL_ITAGGERMUONTOOL_H
#define TAGGERMUONTOOL_ITAGGERMUONTOOL_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Event/Particle.h"
#include "Event/Tagger.h"
#include "Event/Vertex.h"

// forward declarations
namespace LHCb {
  class RecVertex;
}

/** @class ITagger ITagger.h
 *
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2005-06-30
 */

class GAUDI_API ITagger : virtual public IAlgTool {

public:

  DeclareInterfaceID(ITagger, 2, 0);

  virtual LHCb::Tagger tag( const LHCb::Particle*,
                            const LHCb::RecVertex*,
                            LHCb::Vertex::ConstVector&, 
                            LHCb::Particle::ConstVector&) = 0;

};
#endif // TAGGERMUONTOOL_ITAGGERMUONTOOL_H
