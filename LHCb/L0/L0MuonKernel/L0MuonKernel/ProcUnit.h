// $Id: ProcUnit.h,v 1.4 2005-12-15 15:53:26 jucogan Exp $

#ifndef L0MUONKERNEL_PROCUNIT_H
#define L0MUONKERNEL_PROCUNIT_H     1

#define  __TEST_BENCH_V2__ 0
/* @class ProcUnit ProcUnit.h L0MuonKernel/ProcUnit.h

Class representing a processing unit 
of the level-0 muon trigger 
   
*/

#include <vector>
#include "L0MuonKernel/L0MUnit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/CandRegisterHandler.h"
#include "ProcessorKernel/TileRegister.h"

namespace L0Muon {

  class ProcUnit : public L0MUnit {

  public:

    /// Default constructor
    ProcUnit();

    /** Constructor

    @param puNode   :  data flow from L0mConf
    */
    ProcUnit(L0MPuNodeBase& puNode,int rawBufferMode, int L0BufferMode);

    /// Constructor
    ProcUnit(DOMNode* pNode);

    /// Return the MuonTileID of the PU
    MuonTileID puId(){return m_mid;} 
  
    
    void execute();
 
    /// Give a static type name to the unit
    std::string type() {
      return "ProcUnit";
    }  

  private:

    TileRegister* m_ol3[2];     // OL's in station 3 
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_PROCUNIT_H  
