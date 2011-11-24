
//=============================================================================
/** @file DeRichPDPanel.cpp
 *
 *  Implementation file for class : DeRichPDPanel
 *
 *  @author Chris Jones  christopher.rob.jones@cern.ch
 *  @date   2011-09-23
 */
//=============================================================================

// local
#include "RichDet/DeRichPDPanel.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichPDPanel::DeRichPDPanel( const std::string & name )
  : DeRichBase    ( name                  ),
    m_PDColumns   ( 0                     ),
    m_PDNumInCol  ( 0                     ),
    m_PDMax       ( 0                     ),
    m_rich        ( Rich::InvalidDetector ),
    m_side        ( Rich::InvalidSide     ),
    m_pdType      ( LHCb::RichSmartID::Undefined ), 
    m_localOffset ( 0.0                   ),
    m_detPlaneZ   ( 0.0                   ) { }

//=============================================================================
// Destructor
//=============================================================================
DeRichPDPanel::~DeRichPDPanel() { } 

//=============================================================================
