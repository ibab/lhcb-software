// $Id: IPatSeedFit.h,v 1.2 2009-03-09 13:47:02 cattanem Exp $
#ifndef TRACKINTERFACES_IPATSEEDFIT_H
#define TRACKINTERFACES_IPATSEEDFIT_H 1

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/LHCbID.h"
#include "Event/State.h"

static const InterfaceID IID_IPatSeedFit("IPatSeedFit", 1, 0);

// forward declaration(s)
class PatSeedTrack;

/** @class IPatSeedFit IPatSeedFit.h
 *
 * provide a convenient interface to the internal fit used in the PatSeeding
 * algorithm in the pattern reco
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2009-01-28
 */
class IPatSeedFit : virtual public IAlgTool {

public:
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatSeedFit; }

  virtual StatusCode fitSeed( const std::vector<LHCb::LHCbID> lhcbIDs, 
			      std::vector<LHCb::State> *states) const = 0;

  // fit a PatSeedTrack
  virtual StatusCode fitTrack( PatSeedTrack& track, double maxChi2,
		  unsigned minPlanes, bool xOnly, bool forceDebug ) const = 0; 

};
#endif // INCLUDE_IPATSEEDFIT_H
