// $Id: TupleToolRecoStats.h,v 1.4 2010-07-28 16:44:41 pkoppenb Exp $
#ifndef TUPLETOOLRECOSTATS_H 
#define TUPLETOOLRECOSTATS_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface

#include "OTDAQ/IOTRawBankDecoder.h"
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "Event/Track.h"

#include "Kernel/LHCbID.h"
#include "LoKi/select.h"
#include <algorithm>
//for velo count 
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
 
/** @class TupleToolRecoStats TupleToolRecoStats.h
 *  
 *  Fill number of protoparticles, tracks.
 *  If isVerbose, also fill muon tracks, IT and OT clusters, SPD multiplicity, backward tracks, velo tracks
 *  for IT clusters, one needs to run 
 *  \verbatim
from Configurables import RawBankToSTClusterAlg 
createITClusters = RawBankToSTClusterAlg("CreateITClusters") 
createITClusters.DetType     = "IT"\endverbatim 
 *
 *  @author Patrick Koppenburg, Fatima Soomro, Jibo He
 *  @date   2009-02-11
 */
class TupleToolRecoStats : public TupleToolBase, virtual public IEventTupleTool {
public: 
  /// Standard constructor
  TupleToolRecoStats( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~TupleToolRecoStats( ); ///< Destructor
  virtual StatusCode initialize();
  
  virtual StatusCode fill( Tuples::Tuple& );///< Fill tuple

protected:
// velo count
inline bool inCloneContainer(const std::vector<LHCb::Track*>& keepCont, const std::vector<LHCb::LHCbID>& vids) {
  bool keep = true;
  std::vector<LHCb::Track*>::const_iterator iter = keepCont.begin();
  for (;iter != keepCont.end(); ++iter ){
    const std::vector<LHCb::LHCbID>& vids2 = (*iter)->lhcbIDs(); 
    std::vector<LHCb::LHCbID> veloHits2; veloHits2.reserve(vids.size());
    LoKi::select(vids2.begin(), vids2.end(), std::back_inserter(veloHits2), 
                 boost::lambda::bind(&LHCb::LHCbID::isVelo,boost::lambda::_1));
    if (std::equal(vids.begin(), vids.end(), veloHits2.begin())){
      keep = false;
      break;
    }
  }
  return keep;
}
  
  inline unsigned nVelo(const LHCb::Tracks* tracks) {
    
    std::vector<LHCb::Track*> tmpCont;
    LHCb::Tracks::const_iterator iterT = tracks->begin();
    for(; iterT != tracks->end() ;++iterT) {
      if ((*iterT)->hasVelo() == true) tmpCont.push_back(*iterT); 
    }
    
    std::vector<LHCb::Track*> keepCont;
    keepCont.push_back(tmpCont.front());
    
    std::vector<LHCb::Track*>::const_iterator iterT2 = tmpCont.begin();
    for (;iterT2 != tmpCont.end(); ++iterT2 ){
      const std::vector<LHCb::LHCbID>& vids = (*iterT2)->lhcbIDs();
      std::vector<LHCb::LHCbID> veloHits; veloHits.reserve(vids.size());
      LoKi::select(vids.begin(), vids.end(), std::back_inserter(veloHits), 
                   boost::lambda::bind(&LHCb::LHCbID::isVelo,boost::lambda::_1));
      
      if (inCloneContainer(keepCont,veloHits) == false){
        // nothing
      } 
      else {
        keepCont.push_back(*iterT2);   
      }
    } // iterT2
    return keepCont.size();
  }

private:
#include "Number.icpp"
  const IOTRawBankDecoder* m_rawBankDecoder;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_l0BankDecoder;
  bool fillVerbose;
 
};
#endif // TUPLETOOLRECOSTATS_H

