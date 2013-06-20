// $Id: IPatVeloTTFit.h,v 1.1 2009-03-11 15:18:17 smenzeme Exp $
#ifndef TRACKINTERFACES_IPATVELOTTFIT_H
#define TRACKINTERFACES_IPATVELOTTFIT_H 1

#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"

static const InterfaceID IID_IPatVeloTTFit("IPatVeloTTFit", 1, 0);

/** @class IPatVeloTTFit IPatVeloTTFit.h
 *
 * provide a convenient interface to the internal fit used in the PatVeloTTFit
 * algorithm in the pattern reco
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-09
 */
class IPatVeloTTFit : virtual public IAlgTool {

public:
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatVeloTTFit; }
  
  virtual StatusCode fitVTT( LHCb::Track& track) const = 0;

};
#endif // INCLUDE_IPATVELOTTFIT_H
