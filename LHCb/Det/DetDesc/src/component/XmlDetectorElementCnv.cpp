//  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlDetectorElementCnv.cpp,v 1.4 2001-05-17 13:26:52 sponce Exp $

// include files
// #include <cstdlib>
// #include <iostream>
// #include <string>
// #include <vector>

#include "GaudiKernel/CnvFactory.h"
// #include "GaudiKernel/GenericAddress.h"
// #include "GaudiKernel/GenericLink.h"
// #include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/ISvcLocator.h"
// #include "GaudiKernel/IDataProviderSvc.h"
// #include "GaudiKernel/IDataDirectory.h"
// #include "GaudiKernel/MsgStream.h"
// #include "GaudiKernel/RegistryEntry.h"

// #include "DetDesc/DetectorElement.h"
// #include "DetDesc/XmlAddress.h"
// #include "DetDesc/XmlCnvSvc.h"
// #include "DetDesc/XmlCnvAttrList.h"
// #include "DetDesc/XmlCnvException.h"
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


