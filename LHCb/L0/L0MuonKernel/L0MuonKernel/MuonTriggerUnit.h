#ifndef L0MUONKERNEL_MUONTRIGGERUNIT_H
#define L0MUONKERNEL_MUONTRIGGERUNIT_H     1

/* class MuonTriggerUnit L0MuonKernel/MuonTriggerUnit.h

Class representing a board 
of the level-o muon trigger processor 
   
author  Luisanna Tocco
date  24 September 2003
*/ 


#include <vector>
#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

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
    

    /// Return the flag to zero supress or not the rawbuffer
    bool zip(){return m_zip;}
     
  private:
     
    /// Fois (one vector element per station)
    std::vector<int> m_xfoi;
    std::vector<int> m_yfoi;    
    bool m_ignoreM1;
    bool m_ignoreM2;
    std::vector<double> m_ptparameters;
    int m_procVersion;
    bool m_zip;

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_MUONTRIGGERPROCESSORUNIT_H  
