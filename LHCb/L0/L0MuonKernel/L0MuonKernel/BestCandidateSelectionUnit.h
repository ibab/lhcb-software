#ifndef L0MUONKERNEL_BESTCANDIDATESELECTIONUNIT_H
#define L0MUONKERNEL_BESTCANDIDATESELECTIONUNIT_H     1

/* class BestCandidateSelectionUnit L0MuonKernel/BestCandidateSelectionUnit
 
   author  Luisanna Tocco 
   date    24 September 2003
*/

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <cmath>
#include <utility>
#include "L0MuonKernel/Unit.h"
#include "Event/L0MuonCandidate.h"
#include "GaudiKernel/MsgStream.h"
#include "L0MuonKernel/BoardUnit.h"
#include "Event/L0MuonBase.h"

namespace L0Muon {

  class BestCandidateSelectionUnit : public Unit {

  public:

    BestCandidateSelectionUnit(bool & writeL0buffer);
    ~BestCandidateSelectionUnit();


    void loadCandidates(L0MuonCandidate * cand);
    void loadStatus(int st){ m_status = st; }                    
  
    void dumpCandidates(MsgStream & log);
    void dumpAddresses(MsgStream & log);

    void initialize();
    void execute();
    void initialize(MsgStream & log);
    void execute(MsgStream & log);

    void finalize();
    

    void dump(FILE *l0bufferFile);  // write the output files
    void setOutputFile(MuonTileID puid); // open the output files 
    
    // aggiungo:
    //========
    void sortCandidatesbcsu();
    void loadOffsets(std::pair<L0MuonCandidate*, std::vector<int> >);
    //===========


  private:

    void fillAddresses();
    void fillInp();

    void setOutBCSU(); // set the output field
    void setInpBCSU(); // set the input field
    void setIdBCSU();  // set the Identification field
    
    boost::dynamic_bitset<> codedPt(double pt); // code the value of pt on 8 bits

    MuonTileID m_id;

    boost::dynamic_bitset<> m_BcsuInput; // bitset for input field
    boost::dynamic_bitset<> m_BcsuOut ;  // bitset for output field
    boost::dynamic_bitset<> m_BcsuId ;   // bitset for Identification field
    
    unsigned long m_bcsueventnumber ;   // counter for event number


    std::vector<int> m_punumber;
    
    std::vector<L0MuonCandidate*> m_candidates;
    unsigned int m_status;

    std::vector<std::pair<L0MuonCandidate*, 
      boost::dynamic_bitset<> > > m_inp;
    
    std::vector<boost::dynamic_bitset<> > m_addresses ;
    std::vector<boost::dynamic_bitset<> > m_Bcsu ;
    //std::vector<boost::dynamic_bitset<> > m_Pts ;
    


    FILE *m_bcsul0bufferFile;
    
    // aggiungo
    std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > m_offsets;

    bool m_writeL0buffer;
    

  };

    class CalculateBestPt {
  public:

    int operator() (L0MuonCandidate* lmc1,
                    L0MuonCandidate* lmc2) {
      return fabs(lmc1->pt()) > fabs(lmc2->pt());
    }

    int operator() (std::pair<L0MuonCandidate*,boost::dynamic_bitset<> >  p1,
                    std::pair<L0MuonCandidate*,boost::dynamic_bitset<> >  p2) {
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
    }
    //aggiungo
    int operator() (std::pair<L0MuonCandidate*,std::vector<int> >  p1,
                    std::pair<L0MuonCandidate*,std::vector<int> >  p2) {
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
    }
     
    //fine aggiunta
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_BOARD_H  
