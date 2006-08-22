#ifndef L0MUONKERNEL_OLDUMPUNIT_H
#define L0MUONKERNEL_OLDUMPUNIT_H 

/** @class Oldumpunit Oldumpunit.h  L0MuonKernel/Oldumpunit.h
 
  class to print out the formatted Optical Link of a PU 

  @author
  @date

*/
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0BufferUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {



  static const int OLsSize  =  32*8 ;

  class OLDumpUnit : public L0BufferUnit {

  public:

    /// Default Constructor
    OLDumpUnit();

    /// Constructor
    OLDumpUnit(LHCb::MuonTileID id);

    /// Constructor
    OLDumpUnit(DOMNode* pNode);

    /// Destructor
    ~OLDumpUnit();

    void initialize();
    void execute();
    void postexecute();
 
   /// Give a static type name to the unit
    std::string type() {
      return "OLDumpUnit";
    }

    /// Give a static size to the L0Buffer
    int bufferSize() { return OLsSize;}

  private:


  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_OLDUMPUNIT_H
