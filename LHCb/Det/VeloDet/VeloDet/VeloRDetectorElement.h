#ifndef VELORDETECTORELEMENT_H
#define VELORDETECTORELEMENT_H 1

// Include files
#include "VeloDetectorElement.h"

// External declarations
static const CLID CLID_VeloRDetectorElement = 4022;

/**
 * Vertex detector element describing an \f$R\f$ detector.
 */

class VeloRDetectorElement : public VeloDetectorElement {

public:

  /// Constructors
  VeloRDetectorElement() {
    this->setType(R);
  }

  /// Destructor
  virtual ~VeloRDetectorElement() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloRDetectorElement::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloRDetectorElement; 
  }

private:

};

#endif // VELORDETECTORELEMENT_H
