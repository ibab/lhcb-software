//  $ID: $

// include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "DetDesc/TabulatedProperty.h"

#include "XmlDetectorElementCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlDetectorElementCnv> s_factory;
const ICnvFactory& XmlDetectorElementCnvFactory = s_factory;


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


