#ifndef LHCBEVENT_VELOPHIDIGIT_H
#define LHCBEVENT_VELOPHIDIGIT_H 1


/// Includes
#include "GaudiKernel/SmartRef.h"
#include "VeloEvent/VeloDigit.h"

/// Forward declarations
class VeloPhiDetectorElement;

// Externals 
static const CLID CLID_VeloPhiDigit = 4003;

/**
 * Digitisation of a \f$\phi\f$ strip.
 */

class VeloPhiDigit : public VeloDigit {

public:

  /// Constructors
  VeloPhiDigit() {

  }
  
  /// Destructor
  virtual ~VeloPhiDigit() {

  }
  
  /// Reference to the detector element for the \f$\phi\f$ detector
  /// IN FACT THIS CAN'T BE DONE SINCE THERE ARE NO LINKS BETWEEN
  /// DATA STORES... NEEDS ANOTHER SOLUTION
  virtual SmartRef<VeloPhiDetectorElement>& det();
  virtual const SmartRef<VeloPhiDetectorElement>& det() const;

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloPhiDigit::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloPhiDigit; 
  }
  
private:
  
  /// Reference to the detector element for the \f$R\f$ detector
  SmartRef<VeloPhiDetectorElement> m_det;
  
};

#include "Det/VeloDet/VeloPhiDetectorElement.h"

/// Reference to the detector element for the \f$\phi\f$ detector
inline SmartRef<VeloPhiDetectorElement>& VeloPhiDigit::det() {
  return m_det;
}
inline const SmartRef<VeloPhiDetectorElement>& VeloPhiDigit::det() const{
  return m_det;
}


#endif // LHCBEVENT_VELOPHIDIGIT_H
