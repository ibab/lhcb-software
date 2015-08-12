// Include files 

// local
#include "NeutralProtoParticleAddNeutralID.h"
#include "CaloUtils/CaloMomentum.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NeutralProtoParticleAddNeutralID
//
// 2014-05-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NeutralProtoParticleAddNeutralID )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NeutralProtoParticleAddNeutralID::NeutralProtoParticleAddNeutralID( const std::string& name,
                                                                    ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator )
  ,m_input(){
  declareProperty ( "Input"                      , m_input = LHCb::ProtoParticleLocation::Neutrals ) ;
  declareProperty ( "UpdateIsNotE"               , m_isNotE = true);
  declareProperty ( "UpdateIsNotH"               , m_isNotH = true);
  declareProperty ( "UpdateIsPhoton"             , m_isPhoton = true);
  declareProperty ( "MinPtIsNotE"                , m_isNotE_Pt = 75.);
  declareProperty ( "MinPtIsNotH"                , m_isNotH_Pt = 75.);
  declareProperty ( "MinPtIsPhoton"              , m_isPhoton_Pt = 2000.);
}
//=============================================================================
// Destructor
//=============================================================================
NeutralProtoParticleAddNeutralID::~NeutralProtoParticleAddNeutralID() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode NeutralProtoParticleAddNeutralID::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_gammaPi0  = tool<IGammaPi0SeparationTool>("GammaPi0SeparationTool" , "GammaPi0SeparationTool", this);
  m_neutralID = tool<INeutralIDTool>("NeutralIDTool" , "NeutralIDTool", this);



  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode NeutralProtoParticleAddNeutralID::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // locate input data
  const LHCb::ProtoParticles* pps = getIfExists<LHCb::ProtoParticles>(evtSvc(),m_input);
  for ( LHCb::ProtoParticles::const_iterator ipp = pps->begin() ; pps->end() != ipp ; ++ipp ){
    LHCb::ProtoParticle* proto = (LHCb::ProtoParticle*) *ipp ;
    const SmartRefVector<LHCb::CaloHypo>& hypos  = proto->calo() ;
    if ( hypos.empty() ) { continue ; }
    const LHCb::CaloHypo*   hypo  = *(hypos.begin());
    double pt = LHCb::CaloMomentum(hypo).pt();

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "---------- " << hypo->hypothesis() << " ---------- " <<endmsg;

    //======== re-build neutralID
    if ( m_isNotE || m_isNotH){
      double nV[10];
      bool nOk=true;
      nOk &= addInput(0,nV,proto,LHCb::ProtoParticle::CaloTrMatch);
      nOk &= addInput(1,nV,proto,LHCb::ProtoParticle::CaloDepositID,"abs");
      nOk &= addInput(2,nV,proto,LHCb::ProtoParticle::CaloNeutralE19);
      nOk &= addInput(3,nV,proto,LHCb::ProtoParticle::CaloNeutralHcal2Ecal);
      nOk &= addInput(4,nV,proto,LHCb::ProtoParticle::CaloPrsNeutralE19);
      nOk &= addInput(5,nV,proto,LHCb::ProtoParticle::CaloPrsNeutralE49);
      nOk &= addInput(6,nV,proto,LHCb::ProtoParticle::ShowerShape);
      nOk &= addInput(7,nV,proto,LHCb::ProtoParticle::CaloPrsNeutralE4max);
      nOk &= addInput(8,nV,proto,LHCb::ProtoParticle::CaloNeutralPrsM);
      nOk &= addInput(9,nV,proto,LHCb::ProtoParticle::CaloNeutralSpd);      
      const double* nnV=nV;
      if ( m_isNotE && nOk ){ 
        double temp=proto->info(LHCb::ProtoParticle::IsNotE,-1.);
        proto->eraseInfo(LHCb::ProtoParticle::IsNotE);
        double val = ( pt > m_isNotE_Pt ) ? m_neutralID->isNotE(nnV) : -1. ;
        proto->addInfo(LHCb::ProtoParticle::IsNotE,val );
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()  << "UPDATING IsNotE : "
                                                        << temp << " ---> " << proto->info(LHCb::ProtoParticle::IsNotE,-1.) 
                                                        << " (" << proto->hasInfo(LHCb::ProtoParticle::IsNotE ) << ")" << endmsg;
      }      
      if ( m_isNotH && nOk){
        double temp=proto->info(LHCb::ProtoParticle::IsNotH,-1.);
        proto->eraseInfo(LHCb::ProtoParticle::IsNotH);
        double val = (  pt > m_isNotH_Pt) ?  m_neutralID->isNotH(nnV) : -1.;
        proto->addInfo(LHCb::ProtoParticle::IsNotH, val );
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "UPDATING IsNotH : " 
                                                       << temp << " ---> " << proto->info(LHCb::ProtoParticle::IsNotH,-1.) 
                                                       << " (" << proto->hasInfo(LHCb::ProtoParticle::IsNotH ) << ")" << endmsg;
      }
    }
    
    // re-build gamma/pi0 separation
    if ( m_isPhoton &&  hypo -> hypothesis() != LHCb::CaloHypo::PhotonFromMergedPi0 ){
      double pV[15];
      bool pOk=true;
      pOk &= addInput(0,pV,proto,LHCb::ProtoParticle::CaloNeutralID,"area");
      pOk &= addInput(1,pV,proto,LHCb::ProtoParticle::ShowerShape);
      pOk &= addInput(2,pV,proto,LHCb::ProtoParticle::CaloShapeFr2r4);
      pOk &= addInput(3,pV,proto,LHCb::ProtoParticle::CaloShapeAsym);
      pOk &= addInput(4,pV,proto,LHCb::ProtoParticle::CaloShapeKappa);
      pOk &= addInput(5,pV,proto,LHCb::ProtoParticle::CaloShapeE1);
      pOk &= addInput(6,pV,proto,LHCb::ProtoParticle::CaloShapeE2);
      pOk &= addInput(7,pV,proto,LHCb::ProtoParticle::CaloPrsShapeFr2);
      pOk &= addInput(8,pV,proto,LHCb::ProtoParticle::CaloPrsShapeAsym);
      pOk &= addInput(9,pV,proto,LHCb::ProtoParticle::CaloPrsShapeEmax);
      pOk &= addInput(10,pV,proto,LHCb::ProtoParticle::CaloPrsShapeE2);
      pOk &= addInput(11,pV,proto,LHCb::ProtoParticle::CaloPrsM);
      pOk &= addInput(12,pV,proto,LHCb::ProtoParticle::CaloPrsM15);
      pOk &= addInput(13,pV,proto,LHCb::ProtoParticle::CaloPrsM30);
      pOk &= addInput(14,pV,proto,LHCb::ProtoParticle::CaloPrsM45);
      const double* ppV=pV;

      if ( m_isPhoton && pOk ){ 
        double temp=proto->info(LHCb::ProtoParticle::IsPhoton,-1.);
        proto->eraseInfo(LHCb::ProtoParticle::IsPhoton);
        double val = ( pt > m_isPhoton_Pt ) ? m_gammaPi0->isPhoton(ppV) : -1.;
        proto->addInfo(LHCb::ProtoParticle::IsPhoton, val );
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "UPDATING IsPhoton : " 
                                                       << temp << " ---> " << proto->info(LHCb::ProtoParticle::IsPhoton,-1.) 
                                                       << " (" << proto->hasInfo(LHCb::ProtoParticle::IsPhoton ) << ")" << endmsg;
      }      
    }
  }
  return StatusCode::SUCCESS;
}

bool NeutralProtoParticleAddNeutralID::addInput(int index,double* data,LHCb::ProtoParticle* proto,
                                                LHCb::ProtoParticle::additionalInfo flag,std::string func){
  double v=proto->info( flag,-1e+06 );
  if(func=="abs")v=fabs(v);
  if(func=="area")v=double( (int( v ) >> 12) & 0x3 );                  
  data[index]=v;
  return (  proto->hasInfo( flag ) ) ? true:false;
}
    

//=============================================================================
//  Finalize
//=============================================================================
StatusCode NeutralProtoParticleAddNeutralID::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
