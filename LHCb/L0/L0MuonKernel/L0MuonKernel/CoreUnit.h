// $Id: CoreUnit.h,v 1.3 2005-07-01 12:42:42 jucogan Exp $
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
#include "L0MuonKernel/Candidate.h"
//#include "Event/L0Muon.h"
#include "L0MuonKernel/L0MuonStatus.h"
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/Tower.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0mConf/L0MPuNodeBase.h"

namespace L0Muon {

  class CoreUnit : public L0MUnit {

  public:

    /// Constructor
    CoreUnit();

    CoreUnit(MuonTileID id);

    /// Destructor
    ~CoreUnit();

    /// Constructor
    CoreUnit(DOMNode* pNode);

    /// Construct logical pads 
    void makePads();

    /** Construct the tower (optical links + neighbours) 
	in the granularity of M3
    */
    void makeTower();

    /// Draw the Tower
    void drawTower() {m_tower.draw();}

    // Overloads from Unit
    void initialize(); 
    void preexecute();
    void execute();
    void postexecute();
  
    /// Give a static type name to the unit
    std::string type() {
      return "CoreUnit";
    }
     
  private:

    Tower  m_tower;
    std::vector<MuonTileID> m_pads;
    std::vector<PCandidate> m_cand;
    int m_status;
    std::vector<std::pair<PCandidate, std::vector<int> > > m_offForCand;

    bool m_buildL0Buffer;
  
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_COREUNIT_H  
