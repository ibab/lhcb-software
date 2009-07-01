 // $Id: IIsMuonCandidateC.h,v 1.1 2009-07-01 18:27:11 polye Exp $
#ifndef IISMUONCANDIDATEC_H 
#define IISMUONCANDIDATEC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"

static const InterfaceID IID_IIsMuonCandidateC ( "IIsMuonCandidateC", 1, 0 );

/** @class IIsMuonCandidateC IIsMuonCandidateC.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2009-03-11
 */
class IIsMuonCandidateC : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IIsMuonCandidateC; }
  
  virtual bool IsMuonCandidate(const LHCb::Track& muTrack)=0;
  virtual bool IsMuonCandidate(const std::vector<LHCb::LHCbID>& LHCbIDs,const double& mom)=0;

};
#endif // IISMUONCANDIDATEC_H
