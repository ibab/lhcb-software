//  $ID: $

// include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/ISvcLocator.h"

#include "XmlConditionCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlConditionCnv> s_factory;
const ICnvFactory& XmlConditionCnvFactory = s_factory;


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


