// $Id: CablingUnit.h,v 1.5 2004-12-21 14:33:02 ltocco Exp $
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
#include "GaudiKernel/MsgStream.h"
#include "Event/L0MuonCandidate.h"
//#include "Event/L0Muon.h"
#include "L0MuonKernel/L0MuonStatus.h"
#include "L0MuonKernel/Unit.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/TileRegister.h"
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
  void makePads(MsgStream & log);

  /** Construct the tower (optical links + neighbours) 
      in the granularity of M3
  */
  void makeTower(MsgStream & log);

  /// Draw the Tower
  void drawTower(MsgStream & log) {m_tower.draw(log);}

  // Overloads from Unit
  void initialize();
  void initialize(MsgStream & log);

  void execute();

  /** process the tower and select two candidates

      @param log   : MSG::DEBUG 
  */
  void execute(MsgStream & log);

  void finalize();

private:

  Tower  m_tower;
  MuonTileID m_pu;
  std::vector<MuonTileID> m_pads;
  std::vector<L0MuonCandidate*> m_cand;
  int m_status;
  std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > m_offForCand;

  
};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_CABLINGUNIT_H  
