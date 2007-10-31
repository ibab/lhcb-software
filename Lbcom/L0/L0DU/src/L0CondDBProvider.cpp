// $Id: L0CondDBProvider.cpp,v 1.1 2007-10-31 15:04:45 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "L0CondDBProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CondDBProvider
//
// 2007-10-12 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0CondDBProvider );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CondDBProvider::L0CondDBProvider( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_gain()
{
  declareInterface<IL0CondDBProvider>(this);

}
//=============================================================================
// Destructor
//=============================================================================
L0CondDBProvider::~L0CondDBProvider() {} 

//=============================================================================
StatusCode L0CondDBProvider::initialize(){
  debug() << "Initialize L0CondDBProvider" <<endreq;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); 
  return StatusCode::SUCCESS;
}



double L0CondDBProvider::scale(unsigned int base ){
  if( L0DUBase::Type::Digit   == base)return 1;
  if( L0DUBase::Type::CaloEt  == base)return caloEtScale();
  if( L0DUBase::Type::MuonPt  == base)return muonPtScale();
  error() << "No defined type for " << base << endreq;
  return 0.;
}


double L0CondDBProvider::caloEtScale(){  
  m_gain = m_ecal->condition( "Gain" );
  double caloEtScale = 0.0;
  if ( 0 == m_gain ){
    error() << "Condition 'Gain' not found in Ecal"  << endreq;
    return 0.0;
  }
  if ( m_gain->exists( "L0EtBin" ) ) {
    caloEtScale = m_gain->paramAsDouble( "L0EtBin" ) ;
  } else {
    error() << "Parameter 'L0EtBin' not found in Ecal 'Gain'" << endreq;
  }
  debug() << "CaloEt scale set to" << caloEtScale << " MeV" << endreq;
  return caloEtScale;
}


double L0CondDBProvider::muonPtScale(){
  double muonPtScale = 40.*Gaudi::Units::MeV ;// ADC to MeV (hardcoded -- to be extracted from CondDB)
  return muonPtScale;
}
