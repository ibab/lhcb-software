// $Id: IPrepareCaloSeed.h,v 1.1 2007-02-08 17:32:39 hernando Exp $
#ifndef HLTBASE_IIPREPARECALOSEED_H 
#define HLTBASE_IIPREPARECALOSEED_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"
#include "Event/L0CaloCandidate.h"

//forward declarations

static const InterfaceID IID_IPrepareCaloSeed ( "IPrepareCaloSeed", 1, 0 );

/** @class IIPrepareCaloSeed IIPrepareCaloSeed.h tmp/IIPrepareCaloSeed.h
 *
 *  functionality: 
 *      tool to simplify common task asked to the HltSummary  
 *
 *  @author Johannes Albretch
 *  @date   2007-02-08
 */
class IPrepareCaloSeed : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrepareCaloSeed; }

  virtual StatusCode prepareSeed( const LHCb::L0CaloCandidate& eL0Cand,
                                  LHCb::State& seedStatePos,
                                  LHCb::State& seedStateNeg ) = 0;

};
#endif // HLTBASE_IIPREPARECALOSEED_H
