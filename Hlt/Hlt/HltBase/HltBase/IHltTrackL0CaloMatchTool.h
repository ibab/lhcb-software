// $Id: IHltTrackL0CaloMatchTool.h,v 1.1 2007-01-11 10:32:49 hernando Exp $
#ifndef HLTBASE_IHLTTRACKL0CALOMATCHTOOL_H 
#define HLTBASE_IHLTTRACKL0CALOMATCHTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Track.h"
#include "Event/L0CaloCandidate.h"

static const InterfaceID IID_IHltTrackL0CaloMatchTool ( "IHltTrackL0CaloMatchTool", 1, 0 );

/** @class IHltTrackL0CaloMatchTool IHltTrackL0CaloMatchTool.h HltBase/IHltTrackL0CaloMatchTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-01-11
 */
class IHltTrackL0CaloMatchTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() 
  {return IID_IHltTrackL0CaloMatchTool; }
  
  virtual double match(const LHCb::Track& track, 
                       const LHCb::L0CaloCandidate& calo) = 0;

protected:

private:

};
#endif // HLTBASE_IHLTTRACKL0CALOMATCHTOOL_H
