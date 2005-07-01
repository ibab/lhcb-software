#ifndef L0MUONKERNEL_FORMATTINGUNIT_H
#define L0MUONKERNEL_FORMATTINGUNIT_H     1

/* @class FormattingUnit L0MuonKernel/FormattingUnit.h

   class for preparing data for exchanges 
   between the processing units
   input data: optical links
   output data: data for others PUs

   @author  Luisanna Tocco
   @date  24 September 2003
*/ 


#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0mConf/L0MPuNodeBase.h"
#include "L0MuonKernel/L0MUnit.h"
#include "ProcessorKernel/TileRegister.h"
//#include "GaudiKernel/MsgStream.h"

namespace L0Muon {
  
   class FormattingUnit : public L0MUnit {
     
   public:
     
     /// Constructor
     FormattingUnit();
     
     FormattingUnit(MuonTileID id);

     /// Destructor
     ~FormattingUnit();

     /// Constructor
     FormattingUnit(DOMNode* pNode);

     
     void preexecute();
   
     void postexecute();

     /// Give a static type name to the unit
     std::string type() {
       return "FormattingUnit";
     }

     // Intersect tiles of input and output data 
     void makePads();

   private:

     std::vector<MuonTileID> m_pads;
     

   };



};  // namespace L0Muon

#endif      // L0MUONKERNEL_FORMATTINGUNIT_H  
