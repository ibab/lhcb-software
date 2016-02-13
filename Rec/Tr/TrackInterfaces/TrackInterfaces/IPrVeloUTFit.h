#ifndef TRACKINTERFACES_IPRVELOUTFIT_H
#define TRACKINTERFACES_IPRVELOUTFIT_H 1

#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"


/** @class IPrVeloUTFit IPrVeloUTFit.h
 *
 * provide a convenient interface to the internal fit used in the PrVeloUTFit
 * algorithm in the pattern reco
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-09
 */
class IPrVeloUTFit : public extend_interfaces<IAlgTool> {

public:
  // Return the interface ID
  DeclareInterfaceID(IPrVeloUTFit, 2, 0);
  virtual StatusCode fitVUT( LHCb::Track& track) const = 0;

};
#endif // INCLUDE_IPRVELOUTFIT_H
