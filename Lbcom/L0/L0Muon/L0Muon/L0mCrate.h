// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mCrate.h,v 1.3 2002-05-07 07:17:04 atsareg Exp $

#ifndef L0MUON_L0MCRATE_H     
#define L0MUON_L0MCRATE_H     1 

#include <vector>
#include "L0Muon/L0mProcUnit.h"

class MsgStream;
class L0mTrigger;

/** @class L0mCrate L0mCrate.h L0mCrate.h 

   Defines L0mCrate - a quarter of the L0 Muon processor
   
   @author Andrei Tsaregorodtsev
   @date 16 April 2001 
*/

class L0mCrate  {

public:

    /// Default constructor
    L0mCrate(int quarter, const L0mTrigger& ltp);    
    /// Default destructor	      
    ~L0mCrate();
    
    /// Build Processing Units having M3 pads (towers)
    void buildUnits(const std::vector<L0mTower*>& towers);
    /// Execute L0 algorithm and deliver max of 2 candidates
    L0Muon::StatusCode execute(MsgStream& log);
    /// Clean up all the processing units	       
    void clear();   
    /// Get the list of candidates
    std::vector<L0MuonCandidate*> candidates() { return m_candidates; } 
    /// Get crate quarter
    int quarter() const { return m_quarter; }  
    /// Get the number of active units
    int active() const { return m_nActive; }   
private:
    
    unsigned int m_quarter;       // Quarter of the crate
    unsigned int m_status;
    unsigned int m_nActive;       // current number of active Proc Units
    // reference to the trigger processor
    const L0mTrigger& m_processor;
    // List of Processing Units
    std::vector<L0mProcUnit*> m_units;   
    // List of candidates to transmit further   
    std::vector<L0MuonCandidate*> m_candidates; 

};

/// Auxilliary class for sorting candidates in the Pt decreasing order
class ComparePt {
public:
  int operator() (L0MuonCandidate* lmc1,
                  L0MuonCandidate* lmc2) {
    return fabs(lmc1->pt()) > fabs(lmc2->pt());
  }
};

#endif
