// $Id: CaloDigitsFromRaw.cpp,v 1.5 2005-12-20 13:35:28 ocallot Exp $
// Include files 

#include "Kernel/SystemOfUnits.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "CaloDigitsFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitsFromRaw
//
// 2003-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloDigitsFromRaw>          s_factory ;
const        IAlgFactory& CaloDigitsFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitsFromRaw::CaloDigitsFromRaw( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{  
  m_calo = CaloCellCode::CaloNumFromName( name ) ;
  if ( m_calo < 0 || m_calo >= (int) CaloCellCode::CaloNums ) 
  { m_calo = CaloCellCode::CaloNumFromName( "Ecal" ) ; }
  
  declareProperty( "Extension",  m_extension = "" );
}

//=============================================================================
// Destructor
//=============================================================================
CaloDigitsFromRaw::~CaloDigitsFromRaw() {}; 


//=========================================================================
//  Initialization 
//=========================================================================
StatusCode CaloDigitsFromRaw::initialize ( ) {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if ( 0 == m_calo ) {
    m_spdTool = tool<ICaloTriggerBitsFromRaw>( "CaloTriggerBitsFromRaw" );
  } else {
    m_energyTool = tool<ICaloEnergyFromRaw>( "CaloEnergyFromRaw/" + name() + "Tool" );
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitsFromRaw::execute() {

  debug() << "==> Execute" << endreq;

  if       ( 0 == m_calo ) {
    convertSpd ( LHCb::CaloDigitLocation::Spd + m_extension, 3.2 * MeV );
  } else if  ( 1 == m_calo ) {
    convertCaloEnergies ( LHCb::CaloDigitLocation::Prs + m_extension );
  } else if  ( 2 == m_calo ) {
    convertCaloEnergies ( LHCb::CaloDigitLocation::Ecal + m_extension );
  } else if  ( 3 == m_calo ) {
    convertCaloEnergies ( LHCb::CaloDigitLocation::Hcal + m_extension );
  }
  return StatusCode::SUCCESS;
};

//=========================================================================
//  Convert the SPD trigger bits to CaloDigits
//=========================================================================
void CaloDigitsFromRaw::convertSpd ( std::string containerName,
                                     double energyScale ) {

  LHCb::CaloDigits* digits = new LHCb::CaloDigits();
  put( digits, containerName );

  std::vector<LHCb::CaloCellID>& spdCells = m_spdTool->firedCells( false );

  LHCb::CaloCellID id;
  for ( std::vector<LHCb::CaloCellID>::const_iterator itD = spdCells.begin();
        spdCells.end() != itD; ++itD ) {
    LHCb::CaloDigit* dig = new LHCb::CaloDigit( *itD, energyScale );
    digits->insert( dig );
  }
  
  std::stable_sort ( digits->begin(), digits->end(), 
                     CaloDigitsFromRaw::IncreasingByCellID() );

  debug() << "SPD size : " << digits->size() << endreq;
}

//=========================================================================
//  Converts the standard calorimeter adc-energy
//=========================================================================
void CaloDigitsFromRaw::convertCaloEnergies ( std::string containerName ) {

  LHCb::CaloDigits* digits = new LHCb::CaloDigits();
  put( digits, containerName );

  std::vector<LHCb::CaloDigit>& allDigits = m_energyTool->digits( );

  for ( std::vector<LHCb::CaloDigit>::const_iterator itD = allDigits.begin();
        allDigits.end() != itD; ++itD ) {
    LHCb::CaloDigit* dig = (*itD).clone();
    debug() << "ID " << dig->cellID() << " energy " << dig->e() << endreq;
    digits->insert( dig );
  }
  
  std::stable_sort ( digits->begin(), digits->end(), 
                     CaloDigitsFromRaw::IncreasingByCellID() );

  debug() << containerName << " ADC size " << digits->size() << endreq;
}
//=============================================================================
