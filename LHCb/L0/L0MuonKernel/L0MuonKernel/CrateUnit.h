#ifndef L0MUONKERNEL_CRATEUNIT_H
#define L0MUONKERNEL_CRATEUNIT_H     1

/** @class CrateUnit CrateUnit.h L0MuonKernel/CrateUnit.h

Class representing a quarter 
of the level-0 trigger: It constructs 
boards, best candidate selection unit and processing 
units. It initialize, execute and finalize all 
the subunits. 
   
author  Luisanna Tocco
date  24 September 2003
*/ 


#include <vector>

#include "L0MuonKernel/ProcUnit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MProNetBase.h"
#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/L0MuonStatus.h"

namespace L0Muon {

  class CrateUnit : public L0MUnit {
    
  public:
    
    /** Constructor

    @param pProNet   :  processor network from the configuration files
    @param config_pus         :  vector of PU's to be used
    */
    CrateUnit(PL0MProNet & pProNet, 
              std::vector<MuonTileID> config_pus,
              int RawBufferMode,
              int l0bufferMode);

    /** Constructor

    */
    CrateUnit(DOMNode* pNode);

    /// Destructor
    ~CrateUnit();
 
// //     /// Initialize subunits
// //     void initialize();
 
    /// Give a static type name to the unit
    std::string type() {
      return "CrateUnit";
    }

    // // ///* Set the FOIs
    // // // * @param foix      :  field of interest in the x direction 
    // // // * @param foiy      :  field of interest in the y direction 
    // // // */
    // // //void setFois(std::vector<int> & foix, std::vector<int> & foiy){m_xfoi= foix;m_yfoi= foiy;}

// //     /// Return x foi in station sta
// //     int xFoi(int sta);
    
// //     /// Return y foi in station sta
// //     int yFoi(int sta);
    
// //     /// Return the flag for searching candidates without M1 
// //     bool ignoreM1(){ return m_ignoreM1;} 
// //     // // ///* Set the flag for searching candidates without M1 
// //     // // // * @param ignoreM1  :  flag for searching candidates without M1
// //     // // // */
// //     // // //void setIgnoreM1(bool & ignoreM1){m_ignoreM1=ignoreM1;}
 
// //     /// Return parameters for calculating pT
// //     std::vector<double> ptParameters(){ return m_ptparameters;}

// //     // // ///* Set parameters for calculating pT
// //     // // // * @param ptpara    :  geometrical parameters for calculating pT
// //     // // // */
// //     // // //void setPtParameters(std::vector<double> & ptpara){m_ptparameters=ptpara;}

    
  private:

// //     /// Fois (one vector element per station)
// //     std::vector<int> m_xfoi;
// //     std::vector<int> m_yfoi;
    
// //     bool m_ignoreM1;
// //     std::vector<double> m_ptparameters;

  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_CRATEUNIT_H  
