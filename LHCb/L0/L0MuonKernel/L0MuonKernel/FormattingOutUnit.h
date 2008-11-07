#ifndef L0MUONKERNEL_FORMATTINGOUTUNIT_H
#define L0MUONKERNEL_FORMATTINGOUTUNIT_H 

#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  /** @class FormattingOutUnit FormattingOutUnit.h  L0MuonKernel/FormattingOutUnit.h
 
  Class to format the data for the converters.
  
  It inherits from L0MUnit.

  There is one such unit per PU. Its parent is a ProcUnit.

  Unit's registers are :

  - IN  : Same input as the CoreUnit:
          - TileRegisters with optical link
                 - name: OL_Q<q>R<r><x,y>_M<sta>_<ind>, where 
                     - q(=1,...,4), r=(1,...,4), x,y refers to the PU attached to this unit (receiver),
                     - sta(=1,...,5) refers to the muon station.
                 - example: OL_Q1R120_M1_0
          - TileRegisters with neighbour data
                 - name: <function>_M<sta>_Q<q0>R<r0><x0,y0>_Q<q1>R<r1><x1,y1>, where: 
                     - function(=Backplane,Crossing,Horizontal,Vertical) caracterises the type of exchange,
                     - q0(=1,...,4), r0=(1,...,4), x0,y0 refers to the receiving PU,
                     - q1(=1,...,4), r1=(1,...,4), x1,y1 refers to the emitting PU,
                     - sta(=1,...,5) refers to the muon station.
                 - example: Crossing_M2_Q1R131_Q1R120
                 - note: these register are filled in the preexecute phase in the FormattingUnit attached to the emitting PU. 

  - OUT : The output TileRegisters of this unit are formatted to follow the content of the L0Buffer (see EDMS 339143).
          - 1 TileRegister with optical link 
                 - name: FORMATTED_OL_Q<q>R<r><x,y> 
                 where q(=1,...,4), r=(1,...,4), x,y refers to the PU attached to this unit.
                 - example: FORMATTED_OL_Q1R120
          - 1 TileRegister with neighbour data
                 - name: FORMATTED_NEIGH_Q<q>R<r><x,y>
                 where q(=1,...,4), r=(1,...,4), x,y refers to the PU attached to this unit.
                 - example: FORMATTED_NEIGH_Q1R120 

  The ouput registers are filled during the preexecute phase using the input registers.
  They will be used as inputs by converters (ProcDataCnv, ProcRawCnv) to write out the raw banks.

  @author Julien Cogan
  @date 2007 August the 27th
  
  */
  class FormattingOutUnit : public L0MUnit {

  public:

    /// Default Constructor
    FormattingOutUnit();

    /// Constructor with a MuonTileID
    FormattingOutUnit(LHCb::MuonTileID id);

    /// Constructor from xml node
    FormattingOutUnit(DOMNode* pNode);

    /// Destructor
    ~FormattingOutUnit();

    /// Overloads from Unit : fill the output register
    void execute();
    /// Overloads from Unit : release output registers
    void postexecute();
 
   /// Give a static type name to the unit
    std::string type() {
      return "FormattingOutUnit";
    }
    
  private:
    

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_FORMATTINGOUTUNIT_H
