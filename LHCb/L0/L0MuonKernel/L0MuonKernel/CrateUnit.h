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
#include "L0MuonKernel/Unit.h"
#include "Event/L0MuonCandidate.h"
#include "L0MuonKernel/L0MuonStatus.h"
//#include "Event/L0Muon.h"
#include "MuonTools/IMuonTileXYZTool.h"
#include "GaudiKernel/MsgStream.h"


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
        @param IMuonTileXYZTool      :  tool used for the pT calculation
        @param writeL0Buffer         :  flag for writing L0Buffers on files
        @param MsgStream             :  Message Service for DEBUG
    */
    CrateUnit(PL0MProNet & pProNet, 
              std::vector<double> & ptpara, bool & ignoreM1,
              std::vector<int> & foix, std::vector<int> & foiy,
              double & precision,
              int & bits,
              IMuonTileXYZTool * iTileXYZTool,
              bool & writeL0buffer,
              MsgStream & log );

    /// Destructor
    ~CrateUnit();

    /// Initialize subunits
    void initialize();

    /** Initialize  subunits

      @param log   : MSG::DEBUG for running in Gaudi
    */
    void initialize(MsgStream & log);

    /// Execute subunits
    void execute();

    /** Execute  subunits

      @param log   : MSG::DEBUG for running in Gaudi
    */
    void execute(MsgStream & log);

    /// Finalize subunits
    void finalize();
    
    /// Get candidates from processing units.
    void fillCandidates(L0MuonCandidate * cand); 

    /// return candidates 
    std::vector<L0MuonCandidate*> candidates(){ return m_candidates; }
 
    /**   Fill vector containig pairs (L0MuonCandidate, offsets) from Processing units.
          Offsets are the position in FoI of the hits in chambers 2, 1, 4 , 5
          with respect the seed position in station 3.
    */
    void fillOffset(std::pair<L0MuonCandidate*, std::vector<int> > off);
   

    /// Return offsets
    std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > 
    offsets() { return m_offsets; }

    /** Set an integer for Crate status: if candidates are found
        status is OK
    */
    void setStatus(int st){ m_status = st; }
    
    /// Sort the 2 candidates with highest pT
    void sortCandidates(MsgStream & log);

    /// Return a status : candidates found/not found
    unsigned int sortl0mcStatus() { return m_status; }
          
    /// Return pointer to Muon system geometry tool
    IMuonTileXYZTool* getMuonTool() { return m_iTileXYZTool; }
    
    
  private:

    
    std::vector<L0MuonCandidate*>   m_candidates;
    std::vector<std::pair<L0MuonCandidate*,std::vector<int> > > m_offsets;
    IMuonTileXYZTool *m_iTileXYZTool; // Muon geometry tool 

    unsigned int m_status;

  };

  class ComparePt {
  public:

    /** @class 
      Class to  compaire two pts and to choice 
      the candidates with the highest one
    */

    int operator() (L0MuonCandidate* lmc1,
                    L0MuonCandidate* lmc2) {
      return fabs(lmc1->pt()) > fabs(lmc2->pt());
    }


    int operator() (std::pair<L0MuonCandidate*,boost::dynamic_bitset<> >  p1,
                    std::pair<L0MuonCandidate*,boost::dynamic_bitset<> >  p2) 
    {
      
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
      
    }
    

    int operator() (std::pair<L0MuonCandidate*,std::vector<int> >  p1,
                    std::pair<L0MuonCandidate*,std::vector<int> >  p2) {
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
    }
  }; 





};  // namespace L0Muon

#endif      // L0MUONKERNEL_CRATEUNIT_H  
