//  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlDetectorElementCnv.cpp,v 1.5 2001-05-17 16:34:04 sponce Exp $

// include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/ISvcLocator.h"

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
XmlDetectorElementCnv::i_fillSpecificObj (DOM_Element childElement,
                                          DetectorElement* refpObject) {
  // default implementation : ignore child
  return StatusCode::SUCCESS;
} // end i_fillObj


