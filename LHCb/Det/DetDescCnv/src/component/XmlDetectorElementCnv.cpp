//  $ID: $

// include files
#include "GaudiKernel/ISvcLocator.h"
#include "DetDesc/TabulatedProperty.h"

#include "XmlDetectorElementCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlDetectorElementCnv)

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlDetectorElementCnv::XmlDetectorElementCnv (ISvcLocator* svc) :
  XmlBaseDetElemCnv (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child
// -----------------------------------------------------------------------
StatusCode
XmlDetectorElementCnv::i_fillSpecificObj (xercesc::DOMElement* /*childElement*/,
                                          DetectorElement* /*refpObject*/,
                                          IOpaqueAddress* /*address*/) {
  // default implementation : ignore child
  return StatusCode::SUCCESS;
} // end i_fillObj


