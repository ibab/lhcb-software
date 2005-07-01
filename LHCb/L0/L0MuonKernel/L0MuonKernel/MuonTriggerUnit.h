#ifndef L0MUONKERNEL_MUONTRIGGERUNIT_H
#define L0MUONKERNEL_MUONTRIGGERUNIT_H     1

/* class MuonTriggerUnit L0MuonKernel/MuonTriggerUnit.h

   Class representing a board 
   of the level-o muon trigger processor 
   
   author  Luisanna Tocco
   date  24 September 2003
*/ 


#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/Candidate.h"


namespace L0Muon {

   class MuonTriggerUnit : public L0MUnit {
     
  public:

    /// Constructor 
    MuonTriggerUnit();

    /// Constructor 
    MuonTriggerUnit(std::vector< std::string >  configfile, std::vector<MuonTileID> config_pus);

    /// Constructor 
    MuonTriggerUnit(DOMNode* pNode);

    /// Destructor
    ~MuonTriggerUnit();

    /// Give a static type name to the unit
    std::string type() {
      return "MuonTriggerUnit";
    }

    /// return candidates 
    std::map<MuonTileID, std::vector<PCandidate > > candidates(){ return m_candidates; }
    /// Get candidates from processing units.
    void setCandidates(MuonTileID id, std::vector<PCandidate > vcand){m_candidates[id]=vcand;} 

  private:
     
    std::map<MuonTileID, std::vector<PCandidate > > m_candidates;
    

    };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_MUONTRIGGERPROCESSORUNIT_H  
