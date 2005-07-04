// $Id: ITagger.h,v 1.1 2005-07-04 08:20:05 pkoppenb Exp $
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

  virtual ParticleVector taggers( Particle*,  Vertex*,
				  std::vector<Particle*> ) = 0;
};
#endif // TAGGERMUONTOOL_ITAGGERMUONTOOL_H
