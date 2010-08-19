// $Id: TupleToolRecoStats.h,v 1.6 2010-08-19 11:15:57 pkoppenb Exp $
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
#include "boost/bind.hpp"
 
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

  unsigned nVelo(const LHCb::Tracks* tracks); /// velo hits
  
  // velo count from Matthew David Needham
  inline bool inCloneContainer(const std::vector<LHCb::Track*>& keepCont, 
                      const std::vector<LHCb::LHCbID>& vids) {

    
    bool keep = true;
    std::vector<LHCb::Track*>::const_iterator iter = keepCont.begin();
    for (;iter != keepCont.end(); ++iter ){
      const std::vector<LHCb::LHCbID>& vids2 = (*iter)->lhcbIDs(); 
      std::vector<LHCb::LHCbID> veloHits2; veloHits2.reserve(vids.size());
      LoKi::select(vids2.begin(), vids2.end(), std::back_inserter(veloHits2),boost::bind(&LHCb::LHCbID::isVelo,_1));
      if (std::equal(vids.begin(), vids.end(), veloHits2.begin())){
        keep = false;
        break;
      }
    }
    return keep;
  }



private:
#include "Number.icpp"
  const IOTRawBankDecoder* m_rawBankDecoder;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_l0BankDecoder;
  bool fillVerbose;
 
};
#endif // TUPLETOOLRECOSTATS_H

