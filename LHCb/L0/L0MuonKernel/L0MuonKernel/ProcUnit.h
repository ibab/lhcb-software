// $Id: ProcUnit.h,v 1.3 2005-07-01 12:42:42 jucogan Exp $

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
#include "L0mConf/L0MTile.h"
#include "L0mConf/L0MFoi.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/CoreUnit.h"
#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/FormattingUnit.h"

namespace L0Muon {

  class ProcUnit : public L0MUnit {

  public:

    /// Default constructor
    ProcUnit();

    /** Constructor

    @param puNode   :  data flow from L0mConf
    */
    ProcUnit(L0MPuNodeBase& puNode);

    /// Constructor
    ProcUnit(DOMNode* pNode);

    /// Return the MuonTileID of the PU
    MuonTileID puId(){return m_mid;} 
  
    /// Initialize 
    void initialize();
    
    void execute();
 
    /// Return the list of tiles in the l0buffer
    std::vector<L0MTile> l0bufferTileList(L0MPuNodeBase & puNode);

    /// Give a static type name to the unit
    std::string type() {
      return "ProcUnit";
    }  

  private:

    bool m_buildL0Buffer;
    TileRegister* m_ol3[2];     // OL's in station 3 
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_PROCUNIT_H  
