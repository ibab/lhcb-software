// $Id: CaloDigitsFromRaw.cpp,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
// Include files 

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

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
{  m_calo = CaloCellCode::CaloNumFromName( name ) ;
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
    m_spdTool = tool<ICaloTriggerFromRaw>( "CaloSpdBitFromRaw" );
  } else {
    m_energyTool = tool<ICaloEnergyFromRaw>( "CaloEnergyFromRaw/" + name() + "Tool" );
    if ( 1 == m_calo ) {
      m_energyTool->setScaleAndShift( 0.1 * MeV, 0. );
    } else {
      m_energyTool->setScaleAndShift( 1.0 * MeV, 0.6 );
    }  
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitsFromRaw::execute() {

  debug() << "==> Execute" << endreq;

  if       ( 0 == m_calo ) {
    convertSpd ( RawBuffer::PrsTrig, 
                 CaloDigitLocation::Spd + m_extension, 3.2 * MeV );
  } else if  ( 1 == m_calo ) {
    convertCaloEnergies ( RawBuffer::PrsE, 
                          CaloDigitLocation::Prs + m_extension );
  } else if  ( 2 == m_calo ) {
    convertCaloEnergies ( RawBuffer::EcalE, 
                          CaloDigitLocation::Ecal + m_extension );
  } else if  ( 3 == m_calo ) {
    convertCaloEnergies ( RawBuffer::HcalE, 
                          CaloDigitLocation::Hcal + m_extension );
  }
  return StatusCode::SUCCESS;
};

//=========================================================================
//  Convert the SPD trigger bits to CaloDigits
//=========================================================================
void CaloDigitsFromRaw::convertSpd ( int bankType, 
                                     std::string containerName,
                                     double energyScale ) {

  CaloDigits* digits = new CaloDigits();
  put( digits, containerName );

  m_spdTool->prepare( bankType );
  CaloCellID id;
  int dum;
  while ( m_spdTool->nextCell( id, dum ) ) {
    CaloDigit* dig = new CaloDigit( id, energyScale );
    digits->insert( dig );
  }
  
  std::stable_sort ( digits->begin(), digits->end(), 
                     CaloDigitsFromRaw::IncreasingByCellID() );
}

//=========================================================================
//  Converts the standard calorimeter adc-energy
//=========================================================================
void CaloDigitsFromRaw::convertCaloEnergies ( int bankType, 
                                              std::string containerName ) {

  CaloDigits* digits = new CaloDigits();
  put( digits, containerName );

  m_energyTool->prepare( bankType );
  CaloCellID id;
  double energy;
  while ( m_energyTool->nextCell( id, energy ) ) {
    CaloDigit* dig = new CaloDigit( id, energy );
    debug() << "ID " << id << " energy " << energy << endreq;
    digits->insert( dig );
  }
  
  std::stable_sort ( digits->begin(), digits->end(), 
                     CaloDigitsFromRaw::IncreasingByCellID() );
}
//=============================================================================
