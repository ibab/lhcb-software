// Include files

// STL
#include <math.h> // for fabs() on Windows

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IRegistry.h"

// local
#include "CompareCaloDigits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareCaloDigits
//
// 2003-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( CompareCaloDigits )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareCaloDigits::CompareCaloDigits( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Extension",       m_extension = "Test" );
  declareProperty( "PackedRawBuffer", m_packedRawBuffer = false );
}

//=============================================================================
// Destructor
//=============================================================================
CompareCaloDigits::~CompareCaloDigits() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareCaloDigits::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  std::string ecalName = LHCb::CaloDigitLocation::Ecal;
  std::string hcalName = LHCb::CaloDigitLocation::Hcal;
  if ( m_packedRawBuffer ) {
    ecalName = LHCb::CaloDigitLocation::FullEcal;
    hcalName = LHCb::CaloDigitLocation::FullHcal;
  }

  //== SPD. Digits are yes/no -> tolerance = .5
  LHCb::CaloDigits* spd1 = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Spd );
  LHCb::CaloDigits* spd2 = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Spd + m_extension );
  compareContainers( spd1, spd2, 0.5 );

  //== PreShower.
  LHCb::CaloDigits* prs1 = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Prs );
  LHCb::CaloDigits* prs2 = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Prs + m_extension );
  compareContainers( prs1, prs2, 0.1 );

  //== Ecal.
  LHCb::CaloDigits* ecal1 = get<LHCb::CaloDigits>( ecalName );
  LHCb::CaloDigits* ecal2 = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Ecal + m_extension );
  compareContainers( ecal1, ecal2, 1. );

  //== Hcal.
  LHCb::CaloDigits* hcal1 = get<LHCb::CaloDigits>( hcalName );
  LHCb::CaloDigits* hcal2 = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Hcal + m_extension );
  compareContainers( hcal1, hcal2, 1. );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Compare two CaloDigits containers
//=========================================================================
void CompareCaloDigits::compareContainers ( LHCb::CaloDigits* dig1,
                                            LHCb::CaloDigits* dig2,
                                            double tol) {
  if ( dig1->size() != dig2->size() ) {
    error() << "Incoherent size : " << dig1->registry()->identifier()
            << " : " << dig1->size() << " and "
            << dig2->registry()->identifier() << " : " << dig2->size()
            << endmsg;
  } else {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Comparing " << dig1->name()
              << " and " << dig2->name()
              << " both sizes = " << dig2->size() << endmsg;
  }

  LHCb::CaloDigits::const_iterator it1, it2;
  it1 = dig1->begin();
  it2 = dig2->begin();
  while ( dig1->end() != it1 && dig2->end() != it2 ) {
    if (  !((*it1)->cellID() ==  (*it2)->cellID()) ||
          fabs( (*it1)->e() - (*it2)->e() ) > tol ) {
      info() << "Error in sequence/energy (old-new): "
             << (*it1)->cellID() << " " << (*it2)->cellID()
             << format( "  E %7.1f %7.1f %7.2f ",
                 (*it1)->e()/Gaudi::Units::MeV, (*it2)->e()/Gaudi::Units::MeV ,
                 (*it1)->e()/Gaudi::Units::MeV - (*it2)->e()/Gaudi::Units::MeV )
             << endmsg;
    } else {
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << (*it1)->cellID() << " "
                  << (*it2)->cellID() << "  E "
                  << (*it1)->e()/Gaudi::Units::MeV  << " "
                  << (*it2)->e()/Gaudi::Units::MeV << endmsg;
    }
    if ( (*it1)->cellID() ==  (*it2)->cellID() ) {
      it1++;
      it2++;
    } else if ( (*it1)->cellID() <  (*it2)->cellID() ) {
      it1++;
    } else {
      it2++;
    }
  }
}

//=============================================================================
