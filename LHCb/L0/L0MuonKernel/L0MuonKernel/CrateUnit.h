#ifndef L0MUONKERNEL_CRATEUNIT_H
#define L0MUONKERNEL_CRATEUNIT_H     1

/* class CrateUnit CrateUnit L0MuonKernel/CrateUnit.h

   Class representing a crate
   of the L0Muon Trigger for hardware simulations
   
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
#include "Event/L0Muon.h"
#include "MuonTools/IMuonTileXYZTool.h"
#include "GaudiKernel/MsgStream.h"


namespace L0Muon {

  class CrateUnit : public Unit {
    
  public:

    CrateUnit(PL0MProNet & pProNet, 
              std::vector<double> & ptpara, bool & ignoreM1,
              std::vector<int> & foix, std::vector<int> & foiy,
              double & precision,
              int & bits,
              IMuonTileXYZTool * iTileXYZTool,
              bool & writeL0buffer,
              MsgStream & log );
    ~CrateUnit();

    void initialize();
    void initialize(MsgStream & log);
    void execute();
    void execute(MsgStream & log);
    void finalize();
    
    void fillCandidates(L0MuonCandidate * cand);
    std::vector<L0MuonCandidate*> candidates(){ return m_candidates; }
 

    void fillOffset(std::pair<L0MuonCandidate*, std::vector<int> > off);
    //void fillOffsetyp(std::pair<L0MuonCandidate*, std::vector<int> > off);
    //void fillOffsetym(std::pair<L0MuonCandidate*, std::vector<int> > off);

    std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > 
    offsets() { return m_offsets; }

    void setStatus(int st){ m_status = st; }
    
    void sortCandidates(MsgStream & log);
    unsigned int sortl0mcStatus() { return m_status; }
    
    
      

     /// set pointer to Muon system geometry tool
    //void setMuonTool(IMuonTileXYZTool* pmto) { m_iTileXYZTool = pmto; }
    IMuonTileXYZTool* getMuonTool() { return m_iTileXYZTool; }
    
    //void setMsgStream(MsgStream * msg){m_log = msg; }
    //MsgStream  getMsgStream() { return m_log ; }
      

  private:

    
    std::vector<L0MuonCandidate*>   m_candidates;
    std::vector<std::pair<L0MuonCandidate*,std::vector<int> > > m_offsets;
    // Muon geometry tool
    IMuonTileXYZTool *m_iTileXYZTool;

    unsigned int m_status;

  };

  class ComparePt {
  public:
    int operator() (L0MuonCandidate* lmc1,
                    L0MuonCandidate* lmc2) {
      return fabs(lmc1->pt()) > fabs(lmc2->pt());
    }

    int operator() (std::pair<L0MuonCandidate*,std::vector<int> >  p1,
                    std::pair<L0MuonCandidate*,std::vector<int> >  p2) {
      return fabs((p1.first)->pt()) > fabs((p2.first)->pt());
    }
  }; 





};  // namespace L0Muon

#endif      // L0MUONKERNEL_CRATEUNIT_H  
