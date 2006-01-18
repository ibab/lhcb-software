#ifndef L0MUONKERNEL_RAWBUFFERPROCUNIT_H
#define L0MUONKERNEL_RAWBUFFERPROCUNIT_H 

/** @class RawBufferProcUnit RawBufferProcUnit.h  L0MuonKernel/RawBufferProcUnit.h
 
  class for constructing the L0Buffer 

  @author
  @date

*/
#include "L0MuonKernel/L0MUnit.h"
#include "L0MuonKernel/CandRegisterHandler.h"

namespace L0Muon {

  typedef unsigned int   ContentType ; 

  static const ContentType BitsRawBufProcTot = 0;
  
  
  class RawBufferProcUnit : public L0MUnit {

  public:

    /// Default Constructor
    RawBufferProcUnit();

    /// Constructor
    RawBufferProcUnit(LHCb::MuonTileID id);

    /// Constructor
    RawBufferProcUnit(DOMNode* pNode);

    /// Destructor
    ~RawBufferProcUnit();

    void initialize();
    void execute();
    void postexecute();

   /// Give a static type name to the unit
    std::string type() {
      return "RawBufferProcUnit";
    }

  private:
    
    unsigned long m_evtCounter; // Evt counter

    // Input candidate registers from the Proc/core Unit (max=48)
    std::map<int,CandRegisterHandler> m_candRegHandlerProc;
    // Input candidate registers from the BCSU Unit (max=12)
    std::map<int,CandRegisterHandler> m_candRegHandlerBCSU;

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_RAWBUFFERPROCUNIT_H
