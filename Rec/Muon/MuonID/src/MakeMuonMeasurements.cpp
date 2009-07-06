// $Id: MakeMuonMeasurements.cpp,v 1.2 2009-07-06 08:13:41 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "MakeMuonMeasurements.h"
#include "SmartMuonMeasProvider.h"
#include "Event/MuonCoord.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MakeMuonMeasurements
//
// 2008-07-16 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MakeMuonMeasurements );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MakeMuonMeasurements::MakeMuonMeasurements( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
MakeMuonMeasurements::~MakeMuonMeasurements() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MakeMuonMeasurements::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_measProvider = tool<IMeasurementProvider>("MeasurementProvider",this);  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MakeMuonMeasurements::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  SmartMuonMeasProvider* muprov = new SmartMuonMeasProvider();
  put(muprov,"Rec/Muon/SmartMuonMeasProvider");

  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>("Raw/Muon/Coords");
    
  debug()<<"cleared m_muonProvider"<<endmsg;
  for (LHCb::MuonCoords::iterator it = coords->begin();
       it != coords->end(); ++it){

    LHCb::MuonCoord& tile = **it;
    debug() << " MuonCoord " << tile << endmsg;
    LHCb::LHCbID id = LHCb::LHCbID(tile.key());

    int istation = id.muonID().station();
    if (istation == 0) continue;

    LHCb::Measurement* xmeas = m_measProvider->measurement(id,0);
    LHCb::Measurement* ymeas = m_measProvider->measurement(id,1);
    
    muprov->atStation(istation).push_back( MeasPair(xmeas,ymeas) );

  }
  
  for (int i = 0; i <5; i++) {
    debug() << " measurements at station " << i <<" is "
           << muprov->atStation(i).size() << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MakeMuonMeasurements::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
