#ifndef L0MUONKERNEL_FORMATTINGOUTUNIT_H
#define L0MUONKERNEL_FORMATTINGOUTUNIT_H 

/** @class FormattingOutUnit FormattingOutUnit.h  L0MuonKernel/FormattingOutUnit.h
 
  Class to format the data for the converters.
  It takes a list of input TileRegister and fill the output TileRegister.

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {



  class FormattingOutUnit : public L0MUnit {

  public:

    /// Default Constructor
    FormattingOutUnit();

    /// Constructor
    FormattingOutUnit(LHCb::MuonTileID id);

    /// Constructor
    FormattingOutUnit(DOMNode* pNode);

    /// Destructor
    ~FormattingOutUnit();

    void initialize();
    void execute();
    void postexecute();
 
   /// Give a static type name to the unit
    std::string type() {
      return "FormattingOutUnit";
    }
    
  private:
    

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_FORMATTINGOUTUNIT_H
