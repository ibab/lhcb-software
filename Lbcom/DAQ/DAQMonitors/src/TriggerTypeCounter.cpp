// Include files

#include <TH1.h>
#include "GaudiUtils/Aida2ROOT.h"
#include "Event/ODIN.h"
// local
#include "TriggerTypeCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TriggerTypeCounter
//
// 2009-10-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TriggerTypeCounter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TriggerTypeCounter::TriggerTypeCounter( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
    , m_odin(NULL)
{ }
//=============================================================================
// Destructor
//=============================================================================
TriggerTypeCounter::~TriggerTypeCounter() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TriggerTypeCounter::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  m_odin      = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);

  std::vector<std::string> names;

  // Trigger Type
  int it = 0;
  while( it >= 0 ){
    std::stringstream type("");
    type << (LHCb::ODIN::TriggerType) it ;
    if( type.str().find("ERROR") != std::string::npos )it = -1;
    if( it >= 0  ){
      names.push_back( type.str() );
      it++;
    }
  }
  hbook("TriggerTypesCounter",names);


  // BXTypes
  names.clear();
  it = 0;
  while( it >= 0 ){
    std::stringstream type("");
    type << (LHCb::ODIN::BXTypes) it ;
    if( type.str().find("ERROR") != std::string::npos )it = -1;
    if( it >= 0  ){
      names.push_back( type.str() );
      it++;
    }
  }
  hbook("BXTypesCounter",names);


  // CalibrationTypes
  names.clear();
  it = 0;
  while( it >= 0 ){
    std::stringstream type("");
    type << (LHCb::ODIN::CalibrationTypes) it ;
    if( type.str().find("ERROR") != std::string::npos )it = -1;
    if( it >= 0  ){
      names.push_back( type.str() );
      it++;
    }
  }
  hbook("CalibrationTypesCounter",names);

  return StatusCode::SUCCESS;
}

void TriggerTypeCounter::hbook(const std::string& name, const std::vector<std::string>& names){
  int siz = names.size();
  if( 0 == siz )return;
  AIDA::IHistogram1D* h = book1D( name + "/1" , name , 0. , (double) siz , siz);
  TH1D* th = Gaudi::Utils::Aida2ROOT::aida2root( h );
  int i = 1;
  for( std::vector<std::string>::const_iterator bin = names.begin() ; names.end() != bin ; ++bin){
    th->GetXaxis()->SetBinLabel( i  , (*bin).c_str() );
    i++;
  }


}



//=============================================================================
// Main execution
//=============================================================================
StatusCode TriggerTypeCounter::execute() {

    if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get ODIN
  m_odin->getTime(); // trigger the ODIN bank decoding
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> (LHCb::ODINLocation::Default);
  if( NULL == odin ){
    return Warning("ODIN cannot be loaded",StatusCode::SUCCESS);
  }

  int tt = (int) odin->triggerType();
  fill( histo1D(HistoID("TriggerTypesCounter/1")), tt , 1.);
  counter("TriggerType " + Gaudi::Utils::toString( odin->triggerType()) ) += 1;



  int bt = (int) odin->bunchCrossingType();
  fill( histo1D(HistoID("BXTypesCounter/1")), bt , 1.);
  counter("BXType " + Gaudi::Utils::toString( odin->bunchCrossingType()) ) += 1;


  if( odin->triggerType() == LHCb::ODIN::CalibrationTrigger ){
    int ct = (int) odin->calibrationType();
    fill( histo1D(HistoID("CalibrationTypesCounter/1")), ct , 1.);
    counter("CalibrationType " + Gaudi::Utils::toString( odin->calibrationType()) ) += 1;
  }


  return StatusCode::SUCCESS;
}
//=============================================================================
