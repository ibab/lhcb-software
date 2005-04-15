// $Id: L0mProcUnit.h,v 1.9 2005-04-15 16:14:31 cattanem Exp $

#ifndef L0MUONKERNEL_L0MPROCUNIT_H
#define L0MUONKERNEL_L0MPROCUNIT_H     1

/* @class L0mProcUnit L0mProcUnit.h L0MuonKernel/L0mProcUnit.h

Class representing a processing unit 
of the level-0 muon trigger 
   
*/

#include <vector>
#include "ProcessorKernel/Unit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MTile.h"
#include "L0mConf/L0MFoi.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/CablingUnit.h"
#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/FormattingUnit.h"

namespace L0Muon {

  class L0mProcUnit : public Unit {

  public:

    /// Default constructor
    L0mProcUnit();

    /** Constructor

    @param puNode   :  data flow from L0mConf
    */
    L0mProcUnit(L0MPuNodeBase& puNode);

    /// Return the MuonTileID of the PU
    MuonTileID puId(){return m_pu;} 

    /// Return the MuonTileID of the board
    MuonTileID boardId(){return m_boardID;}

    /// Return the field of interest from L0mConf
    L0MFoi foi() {return m_maxFoi; }
  
    /// Bootstrap 
    void bootstrap();
    
    void execute();
 
    /// Return the list of tiles in the l0buffer
    std::vector<L0MTile> l0bufferTileList(L0MPuNodeBase & puNode);

    /// Set the registers for the l0buffer used in the testbench (temporarly)
    std::map<std::string, std::string> setPLLRegisters(MuonTileID puid, std::string L0BufferMapPath);
  
    /// Give a static type name to the unit
    std::string type() {
      return "L0mProcUnit";
    }  

  private:

    MuonTileID  m_pu;
    MuonTileID  m_boardID;
    L0MFoi      m_maxFoi;
    bool m_writeL0Buffer;
    bool m_buildL0Buffer;
    TileRegister* m_ol3[2];     // OL's in station 3 
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MPROCUNIT_H  
