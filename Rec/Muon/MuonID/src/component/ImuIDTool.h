#ifndef MUONID_IMUIDTOOL_H 
#define MUONID_IMUIDTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"



static const InterfaceID IID_ImuIDTool ( "ImuIDTool", 1, 0 );

/** @class ImuIDTool ImuIDTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @author Xabier Cid Vidal
 *  @date   2008-07-15
 */

class ImuIDTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ImuIDTool; }

  
  virtual StatusCode isGoodSeed(const LHCb::Track& seed)=0;
  
  virtual bool isTrackInAcceptance(const LHCb::Track& seed)=0;

  virtual StatusCode findTrackRegions(const LHCb::Track& muTrack,  std::vector<int>& trackRegion) = 0;

  virtual StatusCode muonCandidate(const LHCb::Track& seed, LHCb::Track& muTrack,
                                   const std::vector<LHCb::LHCbID> ids_init = std::vector<LHCb::LHCbID>()) = 0;

  virtual StatusCode muonQuality(LHCb::Track& muTrack, double& Quality) = 0;
  
  virtual StatusCode muonArrival(LHCb::Track& muTrack, double& Arrival)=0;
  
  virtual StatusCode muonDLL(LHCb::Track& muTrack, const double& Quality, double& CLQuality, 
                             const double& Arrival, double& CLArrival, double& DLL)=0;
  
  virtual StatusCode muonID(const LHCb::Track& seed,LHCb::Track& muTrack,
                            double& Quality, double& CLQuality, 
                            double& Arrival, double& CLArrival, double& DLL,
                            const std::vector<LHCb::LHCbID> ids_init = std::vector<LHCb::LHCbID>())=0;
  

};

#endif // MUONID_IMUIDTOOL_H
