// $Id: CablingUnit.h,v 1.3 2004-07-12 16:09:00 ltocco Exp $
#ifndef L0MUONKERNEL_CABLINGUNIT_H
#define L0MUONKERNEL_CABLINGUNIT_H     1

/* class CablingUnit CablingUnit L0MuonKernel/CablingUnit.h

   Class representing a Cabling Unit of data processing logic 
   of the L0Muon Trigger for hardware simulations
   
   author  Andrei Tsaregorodtsev
   date  12 June 2003
*/ 

// STL includes
#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0Muon.h"
#include "L0MuonKernel/Unit.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/TileRegister.h"
//#include "L0MuonKernel/RegisterFactory.h"
#include "L0mConf/L0MPuNodeBase.h"

namespace L0Muon {

class CablingUnit : public L0Muon::Unit {

public:
  CablingUnit();
  ~CablingUnit();


  void setPU( MuonTileID pu ) { m_pu = pu; }

  MuonTileID getPU() { return m_pu ; }
    

  void makePads(MsgStream * log);
  void makeTower(MsgStream * log);

  void drawTower(MsgStream * log) {m_tower.draw(log);}
  //boost::dynamic_bitset<> getStatus(); 
  //boost::dynamic_bitset<> getCand1(); 
  //boost::dynamic_bitset<> getCand2();


  // Overloads from Unit
  void execute();
  void initialize();
  void finalize();

private:

  Tower  m_tower;
  MuonTileID m_pu;
  std::vector<MuonTileID> m_pads;
  std::vector<L0MuonCandidate*> m_cand;
  int m_status;
  //for offsets in ntuple
  //std::vector<std::pair<L0MuonCandidate*, int[5]> > m_offForCand;
  std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > m_offForCand;
  //std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > m_offypForCand;
  //std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > m_offymForCand;
 //
  
};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_CABLINGUNIT_H  
