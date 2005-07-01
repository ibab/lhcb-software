#ifndef L0MUONKERNEL_CRATEUNIT_H
#define L0MUONKERNEL_CRATEUNIT_H     1

/** @class CrateUnit CrateUnit.h L0MuonKernel/CrateUnit.h

Class representing a quarter 
of the level-0 trigger: It constructs 
boards, best candidate selection unit and processing 
units. It initialize, execute and finalize all 
the subunits. 
   
author  Luisanna Tocco
date  24 September 2003
*/ 


#include <vector>
#include <cmath> // for fabs

#include "L0MuonKernel/ProcUnit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MProNetBase.h"
#include "L0mConf/L0MXmlReader.h"
#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/Candidate.h"
#include "L0MuonKernel/L0MUnit.h"
//#include "Event/Candidate.h"
#include "L0MuonKernel/L0MuonStatus.h"
//#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

  class CrateUnit : public L0MUnit {
    
  public:
    
    /** Constructor

    @param pProNet   :  processor network from the configuration files
    @param config_pus         :  vector of PU's to be used
    */
    CrateUnit(PL0MProNet & pProNet, 
	      std::vector<MuonTileID> config_pus );

    /** Constructor

    */
    CrateUnit(DOMNode* pNode);

    /// Destructor
    ~CrateUnit();
 
    /// Initialize subunits
    void initialize();
 
    /// Preexecute subunits
    void preexecute();

    /// Execute subunits
    void execute();

    /// Postexecute subunits
    void postexecute();
    
    /// Get candidates from processing units.
    void fillCandidates(PCandidate cand); 

    /// return candidates 
    std::vector<PCandidate > candidates(){ return m_candidates; }
 
    /**   Fill vector containig pairs (Candidate, offsets) from Processing units.
          Offsets are the position in FoI of the hits in chambers 2, 1, 4 , 5
          with respect the seed position in station 3.
    */
    void fillOffset(std::pair<PCandidate, std::vector<int> > off);
   

    /// Return offsets
    std::vector<std::pair<PCandidate, std::vector<int> > > 
      offsets() { return m_offsets; }

    /** Set an integer for Crate status: if candidates are found
        status is OK
    */
    void setStatus(int st){ m_status = st; }
    
    /// Sort the 2 candidates with highest pT
    void sortCandidates();

    /// Return a status : candidates found/not found
    unsigned int sortl0mcStatus() { return m_status; }
    
    /// Give a static type name to the unit
    std::string type() {
      return "CrateUnit";
    }

    // // ///* Set the FOIs
    // // // * @param foix      :  field of interest in the x direction 
    // // // * @param foiy      :  field of interest in the y direction 
    // // // */
    // // //void setFois(std::vector<int> & foix, std::vector<int> & foiy){m_xfoi= foix;m_yfoi= foiy;}

    /// Return x foi in station sta
    int xFoi(int sta);
    
    /// Return y foi in station sta
    int yFoi(int sta);
    
    /// Return the flag for searching candidates without M1 
    bool ignoreM1(){ return m_ignoreM1;} 
    // // ///* Set the flag for searching candidates without M1 
    // // // * @param ignoreM1  :  flag for searching candidates without M1
    // // // */
    // // //void setIgnoreM1(bool & ignoreM1){m_ignoreM1=ignoreM1;}
 
    /// Return parameters for calculating pT
    std::vector<double> ptParameters(){ return m_ptparameters;}

    // // ///* Set parameters for calculating pT
    // // // * @param ptpara    :  geometrical parameters for calculating pT
    // // // */
    // // //void setPtParameters(std::vector<double> & ptpara){m_ptparameters=ptpara;}

    
  private:

    
    std::vector<PCandidate >   m_candidates;
    std::vector<std::pair<PCandidate,std::vector<int> > > m_offsets;

    unsigned int m_status;

    /// Fois (one vector element per station)
    std::vector<int> m_xfoi;
    std::vector<int> m_yfoi;
    
    bool m_ignoreM1;
    std::vector<double> m_ptparameters;

  };

  class ComparePt {
  public:

    /** @class L0Muon::ComparePt CrateUnit.h L0MuonKernel/CrateUnit.h
     * Class to compare two pts and to choose the candidates with the highest one
     */

    int operator() (PCandidate lmc1,PCandidate lmc2) {
      return fabs(lmc1->pt()) > fabs(lmc2->pt());
    }


    int operator() (std::pair<PCandidate,boost::dynamic_bitset<> >  p1,
                    std::pair<PCandidate,boost::dynamic_bitset<> >  p2) 
      {
      
	return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
      
      }
    

    int operator() (std::pair<PCandidate,std::vector<int> >  p1,
                    std::pair<PCandidate,std::vector<int> >  p2) {
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
    }
  }; 





};  // namespace L0Muon

#endif      // L0MUONKERNEL_CRATEUNIT_H  
