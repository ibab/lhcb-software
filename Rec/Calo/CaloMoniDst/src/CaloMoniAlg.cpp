// $Id: CaloMoniAlg.cpp,v 1.3 2008-09-22 00:59:56 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "CaloMoniAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloMoniAlg
//
// 2008-09-03 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloMoniAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloMoniAlg::CaloMoniAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
    : Calo2Dview(name, pSvcLocator )
    , m_inputData( "" ) // no default value
    , m_inputs()        // no default value
    , m_detData( "" ){   // no default value
  
  declareProperty( "Input",    m_inputData );
  declareProperty( "Inputs",   m_inputs );
  declareProperty( "Detector", m_detData );
  //

  declareProperty( "HistoEnergyMin"      , m_energyMin = 0. );
  declareProperty( "HistoEtMin"          , m_etMin     = 0. );
  declareProperty( "HistoXMin"           , m_xMin      = -4 * Gaudi::Units::meter  );
  declareProperty( "HistoYMin"           , m_yMin      = -4 * Gaudi::Units::meter );
  declareProperty( "HistoMultiplicityMin", m_multMin   = 0.);
  declareProperty( "HistoSizeMin"        , m_sizeMin   =  0. );
  declareProperty( "HistoMassMin"        , m_massMin = 0.   );

  declareProperty( "HistoEnergyMax"      , m_energyMax = 250. * Gaudi::Units::GeV);
  declareProperty( "HistoEtMax"          , m_etMax     = 15. * Gaudi::Units::GeV );
  declareProperty( "HistoXMax"           , m_xMax      = +4 * Gaudi::Units::meter);
  declareProperty( "HistoYMax"           , m_yMax      = +4 * Gaudi::Units::meter);
  declareProperty( "HistoMultiplicityMax", m_multMax   = 2000.);
  declareProperty( "HistoSizeMax"        , m_sizeMax   = 25. );
  declareProperty( "HistoMassMax"        , m_massMax = 500.*Gaudi::Units::MeV );

  declareProperty( "HistoEnergyBin"      , m_energyBin = 100);
  declareProperty( "HistoEtBin"          , m_etBin     = 100);
  declareProperty( "HistoXBin"           , m_xBin      = 50 );
  declareProperty( "HistoYBin"           , m_yBin      = 50);
  declareProperty( "HistoMultiplicityBin", m_multBin   = 100);
  declareProperty( "HistoSizeBin"        , m_sizeBin   = 25);
  declareProperty( "HistoMassBin"        , m_massBin   = 100  );
  
  declareProperty( "EnergyFilter" , m_eFilter   =-100.);
  declareProperty( "EtFilter"     , m_etFilter  =-100.);
  declareProperty( "MassWindowMin", m_massFilterMin=-9999999.);
  declareProperty( "MassWindowMax", m_massFilterMax=+9999999.);
                     
  declareProperty( "histoList"    , m_histoList);
  m_histoList.push_back( "All" );
  StatusCode sc=setProperty( "HistoTopDir", "CaloMoniDst/" );
  sc.isSuccess() ? 
    info() << "HistoTopDir set to 'CaloMoniDst/' " << endreq :
    info() << "HistoTopDir setProperty failed " << endreq ;      
  
  //set default detectorName
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_detData = name.substr( index, 4 ); 
  if ( name.substr(index,3) == "Prs" ) {
    m_detData = "Prs";
    m_energyMax = 250.* Gaudi::Units::MeV;
    m_etMax     = 10.*Gaudi::Units::MeV;
  }  
  if ( name.substr(index,3) == "Spd" ) {
    m_detData = "Spd";
    m_energyMax = 10.* Gaudi::Units::MeV;
    m_etMax     = 10.*Gaudi::Units::MeV;
  }
}
//=============================================================================
// Destructor
//=============================================================================
CaloMoniAlg::~CaloMoniAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloMoniAlg::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by Calo2Dview

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CaloMoniAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return Calo2Dview::finalize();
}

