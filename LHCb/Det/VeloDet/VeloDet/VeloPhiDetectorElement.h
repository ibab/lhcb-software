#ifndef VELOPHIDETECTORELEMENT_H
#define VELOPHIDETECTORELEMENT_H 1

// Include files
#include "VeloDetectorElement.h"

// External declarations
static const CLID CLID_VeloPhiDetectorElement = 4023;

/**
 * Vertex detector element describing a \f$\phi\f$ detector.
 */

class VeloPhiDetectorElement : public VeloDetectorElement {

public:

  /// Constructors
  VeloPhiDetectorElement() {
    this->setType(Phi);
  }

  /// Destructor
  virtual ~VeloPhiDetectorElement() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloPhiDetectorElement::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloPhiDetectorElement; 
  }

private:

};

#endif // VELOPHIDETECTORELEMENT_H
