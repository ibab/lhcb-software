#ifndef L0MUONKERNEL_MUONTRIGGERUNIT_H
#define L0MUONKERNEL_MUONTRIGGERUNIT_H     1



#include <vector>
#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  /** @class MuonTriggerUnit L0MuonKernel/MuonTriggerUnit.h

  Class representing the L0Muon Trigger.

  It inherits from L0MUnit.

  It is the 'Top Unit' containing the 4 CrateUnit of the L0muon processor (if fully configured).

  Contains also methods to get the trigger parameters. They are propagated to the Tower via the CoreUnit.
 
  Otherwise, this unit is essentialy a container. It does not have any input nor output registers.
  It triggers the initialize, execute, ... methods of its daughters.
  
  @author  Luisanna Tocco, Julien Cogan
  @date  24 September 2003
  */ 
  class MuonTriggerUnit : public L0MUnit {
     
  public:

    /// Constructor 
    MuonTriggerUnit();

    /// Constructor 
    MuonTriggerUnit(DOMNode* pNode);

    /// Destructor
    ~MuonTriggerUnit();

    /// Initialize subunits
    void initialize();

    /// Give a static type name to the unit
    std::string type() {
      return "MuonTriggerUnit";
    }

    /// Return x foi in station sta
    int xFoi(int sta);
    
    /// Return y foi in station sta
    int yFoi(int sta);
    
    /// Return the flag for searching candidates without M1 
    bool ignoreM1(){ return m_ignoreM1;} 

    /// Return the flag for searching candidates without M2
    bool ignoreM2(){ return m_ignoreM2;} 

    /// Return parameters for calculating pT
    std::vector<double> ptParameters(){ return m_ptparameters;}

    /// Return the emulator version
    int procVersion(){ return m_procVersion;}
     
  private:
     
    std::vector<int> m_xfoi; ///< Vector of FOI in X (5 elements, 1 per station)
    std::vector<int> m_yfoi; ///< Vector of FOI in Y (5 elements, 1 per station)   
    bool m_ignoreM1;         ///< Flag to ignore M1
    bool m_ignoreM2;         ///< Flag to ignore M2
    std::vector<double> m_ptparameters; ///< parameters for PT encoding
    int m_procVersion;                  ///< processor version 

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_MUONTRIGGERPROCESSORUNIT_H  
