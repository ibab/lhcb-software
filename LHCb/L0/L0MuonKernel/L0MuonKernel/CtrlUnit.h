#ifndef L0MUONKERNEL_CTRLUNIT_H
#define L0MUONKERNEL_CTRLUNIT_H     1



#include "L0MuonKernel/SelectionUnit.h"


namespace L0Muon {

  /** @class CtrlUnit L0MuonKernel/CtrlUnit.h

  Class representing a controller board of the L0Muon processor.

  It inherits from SelectionUnit.

  There is one such unit per controller board (i.e. per quarter). Its parent is a CrateUnit.

  Unit's registers are :
  
  - IN  : candidates from the 12 processing boards of this quarter (filled in BCSUnit).
          - name: CAND_BCSUQ<q>R<r><x,y>,
            where q(=1,...,4), r=(1,...,4), x,y refers to the BCSU of the board attached to this unit.
          - example: CAND_BCSUQ1R110

  - OUT : selected candidates
          - name: CAND_CTRLQ<q>
            where q(=1,...,4) refers to the quarter attached to the crate.
          - example: CAND_CTRLQ1


  It receives up to 2 candidates from each of the 12 processing boards in a crate (quarter)
  and selects the 2 with the highest pT (done during the postexecute phase)
  using the generic method implemented in SelectionUnit.
 
  @author  Luisanna Tocco, Julien Cogan
  @date  24 September 2003
  */ 
  class CtrlUnit : public SelectionUnit {
    
  public:
    
    /// Default Constructor 
    CtrlUnit();

    /// Constructor with MuonTileID 
    CtrlUnit(LHCb::MuonTileID id);

    /// Constructor from xml node
    CtrlUnit(DOMNode* pNode);

    /// Destructor
    ~CtrlUnit();

    /// Overloads from Unit : initialize the CandRegisterHandler
    void initialize();
    
    /** Overloads from Unit : add the PU number to the input candidate, 
        select best candidates and fill output register 
    */
    void execute();

    /// Give a static type name to the unit
    std::string type() {
      return "CtrlUnit";
    }

   private:

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_CTRLUNIT_H  
