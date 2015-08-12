// Include files 

// local
#include "CaloMoniAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloMoniAlg
//
// 2008-09-03 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloMoniAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloMoniAlg::CaloMoniAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
    : Calo2Dview(name, pSvcLocator )
      , m_inputData( "" ) // no default value
      , m_inputs()        // no default value
      , m_detData( "" )   // no default value
      , m_nAreas()
      , m_count(0)
{

  declareProperty( "Input",    m_inputData );
  declareProperty( "Inputs",   m_inputs );
  declareProperty( "Detector", m_detData = "Ecal" );
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
  declareProperty( "HistoMassMax"        , m_massMax = 250.*Gaudi::Units::MeV );

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
                     
  declareProperty( "histoList"           , m_histoList);
  declareProperty( "removeFromHistoList" , m_removeHisto);
  //  declareProperty( "splitAreas"          , m_split=false);
  declareProperty( "listOfAreas"         , m_areas); // list of areas to be split
  declareProperty ("SaturationBin1D"       , m_sat = true);
  declareProperty ("SaturationBin2D"       , m_sat2D = false);
  declareProperty ("SplitSides"           , m_splitSides = false);
  declareProperty ("PrintOut" , m_print = false);
  

  m_removeHisto.clear();
  m_histoList.clear();

  m_histoList.push_back( "All" );

  // Areas
  m_nAreas = 1 << (CaloCellCode::BitsArea +1);
  m_mcount.reserve(m_nAreas);
  m_scount.reserve(2);
  m_areas.push_back("Outer");
  m_areas.push_back("Middle");
  m_areas.push_back("Inner");  

  
  //set default detectorName
  m_detData = LHCb::CaloAlgUtils::CaloNameFromAlg( name );
  if ( m_detData == "Prs" ) 
    m_energyMax = 300.* Gaudi::Units::MeV;
  if ( m_detData == "Spd" ) 
    m_energyMax = 10.* Gaudi::Units::MeV;  
  
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

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  if("" == histoTopDir()) setHistoTopDir("CaloMoniDst/");

  if( m_split && m_splitSides ){
    warning() << "Cannot split simultaneously the calo sides and areas, so far - Area splitting wins" << endmsg;
    m_splitSides=false;
  }  

  return StatusCode::SUCCESS;
}
