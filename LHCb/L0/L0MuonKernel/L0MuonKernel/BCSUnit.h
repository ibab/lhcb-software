#ifndef L0MUONKERNEL_BCSUNIT_H
#define L0MUONKERNEL_BCSUNIT_H     1

/** @class BCSUnit BCSUnit.h L0MuonKernel/BCSUnit.h
 
   Class representing a BCSU of the hardware processor.
   It receive two candidates from each PU in the board 
   and choose the two ones with the highest pT

   author  Luisanna Tocco 
   date    24 September 2003
*/


#include "L0MuonKernel/SelectionUnit.h"

namespace L0Muon {

  class BCSUnit : public SelectionUnit {

  public:

    /// Constructor 
    BCSUnit();

    /// Constructor 
    BCSUnit(LHCb::MuonTileID id);

    /// Constructor 
    BCSUnit(DOMNode* pNode);

    /// Destructor
    ~BCSUnit();

    void initialize();
    void execute();
    
    /// Give a static type name to the unit
    std::string type() {
      return "BCSUnit";
    }

  private:

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_BCSUNIT_H 
