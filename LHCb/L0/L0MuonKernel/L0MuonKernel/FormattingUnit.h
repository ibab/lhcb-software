#ifndef L0MUONKERNEL_FORMATTINGUNIT_H
#define L0MUONKERNEL_FORMATTINGUNIT_H     1

/* class FormattingUnit L0MuonKernel/FormattingUnit.h

   
   author  Luisanna Tocco
   date  24 September 2003
*/ 


#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0mConf/L0MPuNodeBase.h"
#include "L0MuonKernel/Unit.h"
#include "L0MuonKernel/TileRegister.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {
  
   class FormattingUnit : public Unit {
     
   public:
     
     FormattingUnit();
     ~FormattingUnit();

     void initialize();
     void execute();
     void finalize();
     void makePads();

   private:

     std::vector<MuonTileID> m_pads;
   };



};  // namespace L0Muon

#endif      // L0MUONKERNEL_FORMATTINGUNIT_H  
