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

#include "L0MuonKernel/L0mProcUnit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MProNetBase.h"
#include "L0mConf/L0MXmlReader.h"
#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/Candidate.h"
#include "ProcessorKernel/Unit.h"
//#include "Event/Candidate.h"
#include "L0MuonKernel/L0MuonStatus.h"
//#include "GaudiKernel/MsgStream.h"


namespace L0Muon {

  class CrateUnit : public Unit {
    
  public:

    /** Constructor

        @param pProNet   :  processor network from the configuration files
        @param ptpara    :  geometrical parameters for calculating pT
        @param ignoreM1  :  flag for searching candidates without M1
        @param foix      :  field of interest in the x direction 
        @param foiy      :  field of interest in the y direction 
        @param precision :  precision for calculating pT
        @param bits      :  number of bits for codifying pT
        @param writeL0Buffer         :  flag for writing L0Buffers on files
	@param config_pus         :  vector of PU's to be used
    */
    CrateUnit(PL0MProNet & pProNet, 
              std::vector<double> & ptpara, bool & ignoreM1,
              std::vector<int> & foix, std::vector<int> & foiy,
              double & precision,
              int & bits,
              bool & writeL0buffer,
	      std::vector<MuonTileID> config_pus );

    /// Destructor
    ~CrateUnit();

    /// Initialize subunits
    void initialize();


    /// Execute subunits
    void execute();


    /// Finalize subunits
    void finalize();
    
    /// Get candidates from processing units.
    void fillCandidates(Candidate * cand); 

    /// return candidates 
    std::vector<Candidate*> candidates(){ return m_candidates; }
 
    /**   Fill vector containig pairs (Candidate, offsets) from Processing units.
          Offsets are the position in FoI of the hits in chambers 2, 1, 4 , 5
          with respect the seed position in station 3.
    */
    void fillOffset(std::pair<Candidate*, std::vector<int> > off);
   

    /// Return offsets
    std::vector<std::pair<Candidate*, std::vector<int> > > 
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
    
  private:

    
    std::vector<Candidate*>   m_candidates;
    std::vector<std::pair<Candidate*,std::vector<int> > > m_offsets;

    unsigned int m_status;

  };

  class ComparePt {
  public:

    /** @class 
      Class to  compaire two pts and to choice 
      the candidates with the highest one
    */

    int operator() (Candidate* lmc1,
                    Candidate* lmc2) {
      return fabs(lmc1->pt()) > fabs(lmc2->pt());
    }


    int operator() (std::pair<Candidate*,boost::dynamic_bitset<> >  p1,
                    std::pair<Candidate*,boost::dynamic_bitset<> >  p2) 
    {
      
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
      
    }
    

    int operator() (std::pair<Candidate*,std::vector<int> >  p1,
                    std::pair<Candidate*,std::vector<int> >  p2) {
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
    }
  }; 





};  // namespace L0Muon

#endif      // L0MUONKERNEL_CRATEUNIT_H  
