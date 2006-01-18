// $Id: ProcUnit.h,v 1.6 2006-01-18 16:49:53 jucogan Exp $

#ifndef L0MUONKERNEL_PROCUNIT_H
#define L0MUONKERNEL_PROCUNIT_H     1

/* @class ProcUnit ProcUnit.h L0MuonKernel/ProcUnit.h

Class representing a processing unit 
of the level-0 muon trigger 
   
*/

#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  class ProcUnit : public L0MUnit {

  public:

    /// Default constructor
    ProcUnit();

    /// Constructor
    ProcUnit(DOMNode* pNode);

    /// Destructor
    ~ProcUnit();

    /// Return the MuonTileID of the PU
    LHCb::MuonTileID puId(){return m_mid;} 
 
    /// Give a static type name to the unit
    std::string type() {
      return "ProcUnit";
    }  

  private:
  };

};  // namespace L0Muon

#endif      // L0MUONKERNEL_PROCUNIT_H  
