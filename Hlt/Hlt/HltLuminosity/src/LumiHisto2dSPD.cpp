// $Id: LumiHisto2dSPD.cpp,v 1.2 2009-10-12 16:09:41 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "LumiHisto2dSPD.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiHisto2dSPD
//
// 2009-01-27 : Antonio Maria Perez Calero Yzquierdo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiHisto2dSPD );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiHisto2dSPD::LumiHisto2dSPD( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Calo2Dview( name , pSvcLocator )
{
  declareProperty( "ReadoutTool"   , m_readoutTool  = "CaloDataProvider" );
  declareProperty( "HistoTitle"   , m_htitle  = "" );
}
//=============================================================================
// Destructor
//=============================================================================
LumiHisto2dSPD::~LumiHisto2dSPD() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiHisto2dSPD::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_daq = tool<ICaloDataProvider>( m_readoutTool ,"SpdReadoutTool" , this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiHisto2dSPD::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Get SPD hits:
  if( !m_daq->getBanks() )return StatusCode::SUCCESS;
  const CaloVector<LHCb::CaloAdc>& adcs= m_daq->adcs();
  if ( msgLevel(MSG::DEBUG) ) debug() << "DAQ :    "<< adcs.size() << endmsg ;

  //Make Histo:
  for( const auto& i : adcs ) fillCalo2D( "SPD" , i , m_htitle+"Spd hits" );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiHisto2dSPD::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return Calo2Dview::finalize(); // must be called after all other actions
}

//=============================================================================
