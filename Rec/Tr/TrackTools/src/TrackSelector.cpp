
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
 
// Tsa
#include "TrackSelector.h"
#include "Event/Track.h"

using namespace LHCb; 

DECLARE_TOOL_FACTORY( TrackSelector );
  
TrackSelector::TrackSelector(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
GaudiTool(type, name, parent){

 declareProperty("pCut",    m_pCut    = 0.0*Gaudi::Units::GeV );
 declareProperty("ptCut",   m_ptCut   = 0.0*Gaudi::Units::GeV );
 declareProperty("chi2Cut", m_chi2Cut = 1000.);
 declareProperty("hitCut", m_hitCut = 5.);
 declareProperty("vWeight", m_vWeight = 1.);
 declareProperty("oWeight", m_oWeight = 0.5);
 declareProperty("iWeight", m_iWeight = 1.);

 declareInterface<ITrackSelector>(this);

}

TrackSelector::~TrackSelector(){
  // destructer
}

bool TrackSelector::accept(const Track& aTrack) const{
 

  // measurements....
  const double nMeas = weightedMeasurementSum(aTrack);

  if (nMeas < m_hitCut) return false;

  if (aTrack.chi2PerDoF() > m_chi2Cut ) return false;

  // cut p
  if (aTrack.p() < m_pCut) return false;

  // cut on pt
  if (aTrack.pt() < m_ptCut) return false;
  
  return true;
}

double TrackSelector::weightedMeasurementSum(const Track& aTrack) const{

 double wSum = 0;
 const std::vector<LHCbID>& ids = aTrack.lhcbIDs();
 for (std::vector<LHCbID>::const_iterator iter = ids.begin(); iter != ids.end(); ++iter){
   if (iter->isVelo() == true){
     wSum += m_vWeight;
   }
   else if (iter->isST() == true) {
     wSum += m_iWeight;
   }
   else if (iter->isOT() == true) {
     wSum += m_oWeight;
   }
   else {
     // nothing
   }
 }

 return wSum;
}
