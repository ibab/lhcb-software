// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0mProcUnit.h,v 1.2 2001-07-09 19:38:47 atsareg Exp $

#ifndef L0MUON_L0MPROCUNIT_H     
#define L0MUON_L0MPROCUNIT_H     1 

#include <vector>
#include "L0Muon/MuonTile.h"
#include "L0Muon/L0mTower.h"
#include "L0Muon/L0MuonCandidate.h"

class MsgStream;

/** @class L0mProcUnit L0mProcUnit.h L0mProcUnit.h 

   Defines L0mProcUnit - a unit of the L0 Muon processor
   
   @author Andrei Tsaregorodtsev
   @date 16 April 2001 
*/

class L0mProcUnit : public MuonTile  {

public:
    /// Default constructor
    L0mProcUnit();
    ///
    L0mProcUnit(const std::vector<double>& ptpara,
                const std::vector<int>& foiX,
                const std::vector<int>& foiY,
		const std::vector<int>& extM1,
		double precision,
		int bits,
                const MuonTile& mt); 
		   
    /// Default destructor	      
    ~L0mProcUnit();
    
    /// Execute L0 algorithm and deliver max of 2 candidates
    L0Muon::StatusCode execute(MsgStream& log);	
    /// Add a tower for processing       
    void addTower(L0mTower* lt);  
    /// Clean up all the towers;
    void clear();
    /// Get the number of active towers 
    int towers() { return m_towers.size(); }
    /// get candidates
    std::vector<L0MuonCandidate*> candidates() {return m_candidates; }
    /// print Processing Unit parameters
    void printParameters( MsgStream& log);
    // class L0mTower is our friend to get algorithm parameters
    friend class L0mTower;
    
private:
    
    int m_quarter;                      // Quarter of the crate
    std::vector<L0mTower*> m_towers;    // List of towers built on M3 pads
    // list of candidates
    std::vector<L0MuonCandidate*> m_candidates;
    
    // Pt calculation parameters
    std::vector<double> m_ptParameters;
    
    // Fields of interest
    std::vector<int> m_foiX;
    std::vector<int> m_foiY; 
    // Parameters for extrapolation to M1
    std::vector<int> m_extraM1;    
    // Pt precision
    double m_precision;
    // Number of bits for Pt encoding
    int m_bits;
    // PU status
    int m_status;
       
    // Helper functions
    void precisionPt();

};

// bool operator==(L0mProcUnit* pu1, L0mProcUnit* pu2);

#endif
