// $Id: L0mProcUnit.h,v 1.7 2005-03-31 16:08:07 jucogan Exp $

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

    @param pProNet   :  data flow from L0mConf
    @param ptpara    :  geometrical parameters for calculating pT
    @param ignoreM1  :  flag for searching candidates without M1
    @param foix      :  field of interest in the x direction
    @param foiy      :  field of interest in the y direction
    @param precision :  precision for calculating pT
    @param bits      :  number of bits for codifying pT
    @param writeL0Buffer         :  flag for writing L0Buffers on files
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
  
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MPROCUNIT_H  
