// $Id: CaloProtoElectronMonitor.cpp,v 1.1 2009-12-11 17:07:40 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include  "CaloUtils/CaloMomentum.h"
// local
#include "CaloProtoElectronMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloProtoElectronMonitor
//
// 2009-12-11 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloProtoElectronMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloProtoElectronMonitor::CaloProtoElectronMonitor( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : CaloMoniAlg ( name , pSvcLocator )
{

  declareProperty("HistoEoPMin" , m_eOpMin = 0.);
  declareProperty("HistoEoPMax" , m_eOpMax = 3.);
  declareProperty("HistoEoPBin" , m_eOpBin = 100);
  declareProperty("PrsCut"      , m_prsCut = 50.* Gaudi::Units::MeV);
  setInputData( LHCb::ProtoParticleLocation::Charged );

  m_multMax = 100;
}
//=============================================================================
// Destructor
//=============================================================================
CaloProtoElectronMonitor::~CaloProtoElectronMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloProtoElectronMonitor::initialize() {
  StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  // get tools
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this);
  m_toPrs = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Prs" , this );
  m_toPrs->setCalos( "Ecal" ,"Prs");
 
  // histograms 
  hBook1(  "1", "# of CaloElectron protoP   " + inputData(),  m_multMin   ,    m_multMax   , m_multBin  );
  hBook1(  "2", "CaloElectron protoP Energy   " + inputData(),  m_energyMin ,    m_energyMax , m_energyBin );
  hBook1(  "3", "CaloElectron protoP Pt       " + inputData(),  m_etMin     ,    m_etMax     , m_etBin);
  hBook2(  "4", "CaloElectron protoP barycenter position x vs y   " 
           + inputData(),  m_xMin, m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
  hBook2(  "5", "Energy-weighted CaloElectron protoP  barycenter position x vs y " 
           + inputData(),m_xMin,m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
  hBook1(  "6", "e/p  " + inputData(),  m_eOpMin ,    m_eOpMax , m_eOpBin );
  hBook1(  "7", "Eprs  " + inputData(),  0. , 300. , m_energyBin );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloProtoElectronMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;



  if ( !produceHistos() ) return StatusCode::SUCCESS;
  // get input data
  if( !exist<LHCb::ProtoParticles> ( inputData() ) ){
    debug() << "no protoP container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS ;
  };
  
  LHCb::ProtoParticles* protos = get<LHCb::ProtoParticles> ( inputData() );
  
  initCounters();
  for( LHCb::ProtoParticles::const_iterator p = protos->begin(); protos->end () != p ; ++p ){ 
    const LHCb::ProtoParticle* proto = *p;
    if( !m_caloElectron->set(proto))continue;;
    LHCb::CaloHypo* hypo = m_caloElectron->electron();
    if ( NULL == hypo ) continue;
    LHCb::CaloMomentum momentum( hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    if(e    < m_eFilter)continue;
    if(et   < m_etFilter)continue;
    double ePrs = m_toPrs->energy ( *hypo , "Prs"  );
    if( m_prsCut > 0 &&  ePrs < m_prsCut )continue;
    LHCb::CaloCellID id = LHCb::CaloCellID();
    if ( hypo->clusters().size() > 0 ){
      SmartRef<LHCb::CaloCluster> cluster= *(hypo->clusters().begin());
      id = (*cluster).seed();      
    }
    double eOp = m_caloElectron->eOverP();

    count(id);
    hFill1(id, "2", e  );
    hFill1(id, "3", et );
    const LHCb::CaloHypo::Position *position = hypo->position();
    if( NULL != position){
      hFill2(id, "4", position->x(),position->y() );
      hFill2(id, "5", position->x(),position->y() , e);
    }
    hFill1(id, "6", eOp );
    hFill1(id, "7", ePrs );
  }
  fillCounters("1");
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloProtoElectronMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return CaloMoniAlg::finalize();  // must be called after all other actions
}

//=============================================================================
