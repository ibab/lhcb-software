// $Id: IPrVeloUTFit.h,v 1.1 2009-03-11 15:18:17 smenzeme Exp $
#ifndef TRACKINTERFACES_IPRVELOUTFIT_H
#define TRACKINTERFACES_IPRVELOUTFIT_H 1

#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"

static const InterfaceID IID_IPrVeloUTFit("IPrVeloUTFit", 1, 0);

/** @class IPrVeloUTFit IPrVeloUTFit.h
 *
 * provide a convenient interface to the internal fit used in the PrVeloUTFit
 * algorithm in the pattern reco
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-09
 */
class IPrVeloUTFit : virtual public IAlgTool {

public:
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrVeloUTFit; }
  
  virtual StatusCode fitVUT( LHCb::Track& track) const = 0;

};
#endif // INCLUDE_IPRVELOUTFIT_H
