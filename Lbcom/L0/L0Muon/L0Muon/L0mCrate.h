// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mCrate.h,v 1.1 2001-06-07 16:46:14 atsareg Exp $

#ifndef L0MUON_L0MCRATE_H     
#define L0MUON_L0MCRATE_H     1 

#include <vector>
#include "L0mProcUnit.h"

class MsgStream;

/** @class L0mCrate L0mCrate.h L0mCrate.h 

   Defines L0mCrate - a quarter of the L0 Muon processor
   
   @author Andrei Tsaregorodtsev
   @date 16 April 2001 
*/

class L0mCrate  {

public:

    /// Default constructor
    L0mCrate(int quarter);    
    /// Default destructor	      
    ~L0mCrate();
    
    /// Build Processing Units having M3 pads (towers)
    void buildUnits(const std::vector<double>& ptpara,
                    const std::vector<int>& foiX,
                    const std::vector<int>& foiY,
		    double precision,
		    int bits,
		    ObjectVector<L0mTower>* towers);
    /// Execute L0 algorithm and deliver max of 2 candidates
    L0Muon::StatusCode execute(MsgStream& log);
    /// Clean up all the processing units	       
    void clear();   
    /// Get the list of candidates
    std::vector<L0MuonCandidate*> candidates() { return m_candidates; }    
private:
    
    int m_quarter;                         // Quarter of the crate
    int m_status;
    // List of active Processing Units
    std::vector<L0mProcUnit> m_units;   
    // List of candidates to transmit further   
    std::vector<L0MuonCandidate*> m_candidates; 

};



#endif
