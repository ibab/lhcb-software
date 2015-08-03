//  $ID: $

// include files
#include "GaudiKernel/ISvcLocator.h"

#include "XmlConditionCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlConditionCnv)

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlConditionCnv::XmlConditionCnv (ISvcLocator* svc) :
  XmlBaseConditionCnv (svc) {
}


// -----------------------------------------------------------------------
// Fill an object with a new specific child
// -----------------------------------------------------------------------
StatusCode
XmlConditionCnv::i_fillSpecificObj (xercesc::DOMElement* /*childElement*/,
                                    Condition* /*refpObject*/,
                                    IOpaqueAddress* /*address*/) {
  // default implementation : ignore child
  return StatusCode::SUCCESS;
} // end i_fillObj


