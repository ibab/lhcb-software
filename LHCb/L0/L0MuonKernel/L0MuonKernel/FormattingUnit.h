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
#include "L0MuonKernel/Unit.h"
#include "L0MuonKernel/TileRegister.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {
  
   class FormattingUnit : public Unit {
     
   public:
     
     /// Constructor
     FormattingUnit();

     /// Destructor
     ~FormattingUnit();


     
     void initialize();
    
     /** 
         Call the method makePads

         @param log   : MSG::DEBUG 
     */
     void initialize(MsgStream & log);
  
     void execute();
     void execute(MsgStream & log);
   
     void finalize();

     /**
        Intersect tiles of input and output data 
      */
     void makePads(MsgStream & log);

   private:

     std::vector<MuonTileID> m_pads;
     

   };



};  // namespace L0Muon

#endif      // L0MUONKERNEL_FORMATTINGUNIT_H  
