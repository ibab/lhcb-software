#ifndef L0MUONKERNEL_BOARDUNIT_H
#define L0MUONKERNEL_BOARDUNIT_H     1

/* class BoardUnit L0MuonKernel/BoardUnit.h

   Class representing a board 
   of the level-o muon trigger processor 
   
   author  Luisanna Tocco
   date  24 September 2003
*/ 


#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "L0mConf/L0MPuNodeBase.h"
#include "L0MuonKernel/L0mProcUnit.h"
#include "ProcessorKernel/Unit.h"


namespace L0Muon {

   class BoardUnit : public Unit {
     
  public:

    /// Constructor 
    BoardUnit();

    /// Destructor
    ~BoardUnit();


    //void initialize();
    //void execute();
    //void finalize();

    /// Give a static type name to the unit
    std::string type() {
      return "BoardUnit";
    }

  private:
     
     int m_imax ;
     
     

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_BOARDUNIT_H  
