// $Id: DeRichHPDPanel.cpp,v 1.4 2003-01-16 17:46:05 papanest Exp $
#define DERICHHPDPANEL_CPP

// Include files
#include "RichDet/DeRichHPDPanel.h"

// Gaudi
//#include "GaudiKernel/MsgStream.h"

// CLHEP files
//#include "CLHEP/Units/SystemOfUnits.h"

/// Detector description classes
//#include "DetDesc/IGeometryInfo.h"
//#include "DetDesc/SolidBox.h" 
//#include "DetDesc/SolidTubs.h" 
//#include "DetDesc/SolidSphere.h" 



//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichHPDPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRichHPDPanel = 12010;  // User defined

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel()
{}
  
// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichHPDPanel::classID() {
  return CLID_DeRichHPDPanel;
}
