#ifndef L0MUONKERNEL_BESTCANDIDATESELECTIONUNIT_H
#define L0MUONKERNEL_BESTCANDIDATESELECTIONUNIT_H     1

/* class BestCandidateSelectionUnit L0MuonKernel/BestCandidateSelectionUnit
 
   author  Luisanna Tocco 
   date    24 September 2003
*/

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0MuonKernel/Unit.h"
//#include "L0mConf/L0MPuNodeBase.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

  class BestCandidateSelectionUnit : public Unit {

  public:

    BestCandidateSelectionUnit();
    ~BestCandidateSelectionUnit();

    void calculateBestPt() ;
    void setOutBCSU();
    

    void initialize();
    void execute();
    void finalize();
    

  private:
    
    int m_imax ;
    int m_PtList[8];
    std::vector<boost::dynamic_bitset<> > m_Bcsu ;
    std::vector<boost::dynamic_bitset<> > m_Pts ;
    std::vector<boost::dynamic_bitset<> > m_BcsuOut ;

  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_BOARD_H  
