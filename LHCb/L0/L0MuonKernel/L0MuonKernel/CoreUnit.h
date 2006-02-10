// $Id: CoreUnit.h,v 1.7 2006-02-10 12:31:20 jucogan Exp $
#ifndef L0MUONKERNEL_COREUNIT_H
#define L0MUONKERNEL_COREUNIT_H     1

/* @class CoreUnit CoreUnit.h  L0MuonKernel/CoreUnit.h

Class representing the kernel of the processing.
It transforms strips into logical pads.
It construct the tower.
It execute algorithms for searching candidates
   
   
*/ 

// STL includes
#include <vector>
//#include "GaudiKernel/MsgStream.h"
#include "L0MuonKernel/MuonCandidate.h"
//#include "Event/L0Muon.h"
#include "L0MuonKernel/L0MuonStatus.h"
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/Tower.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  class CoreUnit : public L0MUnit {

  public:

    /// Constructor
    CoreUnit();

    CoreUnit(LHCb::MuonTileID id);

    /// Destructor
    ~CoreUnit();

    /// Constructor
    CoreUnit(DOMNode* pNode);

    /// Construct logical pads; return false is the pad list is empty
    bool makePads();

    /** Construct the tower (from optical links + neighbours) 
        in the granularity of M3;
        Return true id a seed has been found.
    */
    bool makeTower();

    /// Draw the Tower
    void drawTower() {m_tower.draw();}
    void initializeM1TowerMap();

    /// Set the DEBUG level flag
    void setDebugMode(bool debug = true) ;

    // Overloads from Unit
    void initialize(); 
    void execute();
    void postexecute();
  
    /// Give a static type name to the unit
    std::string type() {
      return "CoreUnit";
    }
     
  private:

    Tower  m_tower;
    std::vector<LHCb::MuonTileID> m_pads;
    CandRegisterHandler m_candRegHandlerOut;
  
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_COREUNIT_H  
