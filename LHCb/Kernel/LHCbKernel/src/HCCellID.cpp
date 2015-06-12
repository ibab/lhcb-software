// Local
#include "Kernel/HCCellID.h"

//============================================================================
// Printout to output stream
//============================================================================
std::ostream& LHCb::HCCellID::fillStream(std::ostream& os) const {
  return os << "CellID: " << this->cellID() 
            << " | Crate: " << this->crate() 
            << " | Channel: " << this->channel(); 
}

