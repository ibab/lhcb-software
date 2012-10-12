#ifndef L0MUONKERNEL_BCSUNIT_H
#define L0MUONKERNEL_BCSUNIT_H     1

#include "L0MuonKernel/SelectionUnit.h"

namespace L0Muon {

  /** @class BCSUnit BCSUnit.h L0MuonKernel/BCSUnit.h

  Class representing a BCSU of the L0Muon processor.

  It inherits from SelectionUnit.

  There is one such unit per processing board. Its parent is a BoardUnit.

  Unit's registers are :

  - IN  : candidates from the 4 PUs of the processing board (filled in CoreUnit).
          - name: CAND_PUQ<q>R<r><x,y>,
            where q(=1,...,4), r=(1,...,4), x,y refers to the PU of the board attached to this unit.
          - example: CAND_PUQ1R120

  - OUT : selected candidates
          - name: CAND_BCSUQ<q>R<r><x,y>,
            where q(=1,...,4), r=(1,...,4), x,y refers to the BCSU of the board attached to this unit.
          - example: CAND_BCSUQ1R110

  It receives up to 2 candidates from each of the 4 PUs in a processing board
  and selects the 2 with the highest pT (done during the postexecute phase)
  using the generic method implemented in SelectionUnit.

  @author  Luisanna Tocco, Julien Cogan
  @date    24 September 2003
  */
  class BCSUnit : public SelectionUnit {

  public:

    /// Default Constructor
    BCSUnit();

    /// Constructor with MuonTileID
    BCSUnit(LHCb::MuonTileID id);

    /// Constructor from XML node
    BCSUnit(DOMNode* pNode);

    /// Destructor
    ~BCSUnit();

    /// Overloads from Unit : initialize the CandRegisterHandler
    void initialize();

    /** Overloads from Unit : add the PU number to the input candidate,
        select best candidates and fill output register
    */
    void execute();

    /// Give a static type name to the unit
    std::string type() {
      return "BCSUnit";
    }

  private:

  };


}  // namespace L0Muon

#endif      // L0MUONKERNEL_BCSUNIT_H
