#ifndef L0MUONKERNEL_FORMATTINGUNIT_H
#define L0MUONKERNEL_FORMATTINGUNIT_H     1



#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  /** @class FormattingUnit L0MuonKernel/FormattingUnit.h

  Class for preparing data for exchanges between the processing units

  It inherits from L0MUnit.

  There is one such unit per PU. Its parent is a ProcUnit.

  Unit's registers are :

  - IN  : TileRegisters with optical links :
             - name: OL_Q<q>R<r><x,y>_M<sta>_<ind>, where :
                 - q(=1,...,4), r=(1,...,4), x,y refers to the PU attached to this unit (receiver),
                 - sta(=1,...,5) refers to the muon station.
             - example: OL_Q1R120_M1_0
             - alias (used at filling time by external method): OL_<sta>_(Q<q>,R<r>,<x>,<y>), where
                 - q(=0,...,3), r=(0,...,3), x,y refers to the emitting opt. link,
                 - sta(=0,...,4) refers to the muon station.

  - OUT : TileRegisters with neighbour data for others PUs :
             - name: <function>_M<sta>_Q<q0>R<r0><x0,y0>_Q<q1>R<r1><x1,y1>, where :
                 - function(=Backplane,Crossing,Horizontal,Vertical) caracterises the type of exchange,
                 - q0(=1,...,4), r0=(1,...,4), x0,y0 refers to the emitting PU,
                 - q1(=1,...,4), r1=(1,...,4), x1,y1 refers to the receiving PU,
                 - sta(=1,...,5) refers to the muon station.
             - example: Backplane_M1_Q1R120_Q1R220

  The ouput registers are filled during the preexecute phase using the input registers.
  They will be used as inputs by the CoreUnit of the receiving PU during the execute phase
  and by the FormattingOutUnit of the receiving PU during the postexecute phase.

  @author  Luisanna Tocco, Julien Cogan
  @date  24 September 2003
  */
  class FormattingUnit : public L0MUnit {

  public:

    /// Default Constructor
    FormattingUnit();

    /// Constructor with a MuonTileID
    FormattingUnit(LHCb::MuonTileID id);

    /// Destructor
    ~FormattingUnit();

    /// Constructor from a xml node
    FormattingUnit(DOMNode* pNode);

    /// Overloads from Unit : perform the data exchange
    void preexecute();

    /// Give a static type name to the unit
    std::string type() {
      return "FormattingUnit";
    }

  private:


  };

}  // namespace L0Muon

#endif      // L0MUONKERNEL_FORMATTINGUNIT_H
