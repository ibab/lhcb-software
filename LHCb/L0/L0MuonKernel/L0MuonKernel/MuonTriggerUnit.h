#ifndef L0MUONKERNEL_MUONTRIGGERUNIT_H
#define L0MUONKERNEL_MUONTRIGGERUNIT_H     1



#include <vector>
#include "L0MuonKernel/L0MUnit.h"

namespace L0Muon {

  /** @class MuonTriggerUnit L0MuonKernel/MuonTriggerUnit.h

  Class representing the L0Muon Trigger.

  It inherits from L0MUnit.

  It is the 'Top Unit' containing the 4 CrateUnit of the L0muon processor (if fully configured).

  Contains also methods to get the trigger parameters. They are propagated to the Tower via the CoreUnit.

  Otherwise, this unit is essentialy a container. It does not have any input nor output registers.
  It triggers the initialize, execute, ... methods of its daughters.

  @author  Luisanna Tocco, Julien Cogan
  @date  24 September 2003
  */
  class MuonTriggerUnit : public L0MUnit {

  public:

    /// Constructor
    MuonTriggerUnit();

    /// Constructor
    MuonTriggerUnit(DOMNode* pNode);

    /// Destructor
    ~MuonTriggerUnit();

    /// Give a static type name to the unit
    std::string type() {
      return "MuonTriggerUnit";
    }


  };
}  // namespace L0Muon

#endif      // L0MUONKERNEL_MUONTRIGGERPROCESSORUNIT_H
