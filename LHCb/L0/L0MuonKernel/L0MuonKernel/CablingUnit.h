// $Id: CablingUnit.h,v 1.7 2005-03-31 16:08:07 jucogan Exp $
#ifndef L0MUONKERNEL_CABLINGUNIT_H
#define L0MUONKERNEL_CABLINGUNIT_H     1

/* @class CablingUnit CablingUnit.h  L0MuonKernel/CablingUnit.h

   Class representing the kernel of the processing.
   It transforms strips into logical pads.
   It construct the tower.
   It execute algorithms for searching candidates
   
   
*/ 

// STL includes
#include <vector>
//#include "GaudiKernel/MsgStream.h"
#include "L0MuonKernel/Candidate.h"
//#include "Event/L0Muon.h"
#include "L0MuonKernel/L0MuonStatus.h"
#include "ProcessorKernel/Unit.h"
#include "L0MuonKernel/Tower.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0mConf/L0MPuNodeBase.h"

namespace L0Muon {

class CablingUnit : public L0Muon::Unit {

public:

  /// Constructor
  CablingUnit();

  /// Destructor
  ~CablingUnit();

  /// Set the MuonTileID of the PU 
  void setPU( MuonTileID pu ) { m_pu = pu; }

  /// Return the MuonTileID of the PU
  MuonTileID getPU() { return m_pu ; }
    
  /// Construct logical pads 
  void makePads();

  /** Construct the tower (optical links + neighbours) 
      in the granularity of M3
  */
  void makeTower();

  /// Draw the Tower
  void drawTower() {m_tower.draw();}

  // Overloads from Unit
  void bootstrap(); 
  void initialize();
  void execute();
  void finalize();
  
  /// Give a static type name to the unit
  std::string type() {
    return "CablingUnit";
  }
     
private:

  Tower  m_tower;
  MuonTileID m_pu;
  std::vector<MuonTileID> m_pads;
  std::vector<Candidate*> m_cand;
  int m_status;
  std::vector<std::pair<Candidate*, std::vector<int> > > m_offForCand;

  
};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_CABLINGUNIT_H  
