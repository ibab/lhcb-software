// $Id: ElectronParticleMaker_bbincl.cpp,v 1.3 2003-08-05 17:25:19 gcorti Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"

// from Event 
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// local
#include "ElectronParticleMaker_bbincl.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ElectronParticleMaker_bbincl
//
// Description: This tool fills the particle class with information from
// ProtoParticles and places it in the Transient Event Store
// 12/06/2002 : Jose Helder Lopes.
// 18/07/2002 : Gloria Corti
//-----------------------------------------------------------------------------
// Declaration of the Tool Factory
static const  ToolFactory<ElectronParticleMaker_bbincl>          s_factory ;
const        IToolFactory& ElectronParticleMaker_bbinclFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ElectronParticleMaker_bbincl::ElectronParticleMaker_bbincl(
                                            const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent ), 
    
    m_ppSvc(0), 
    m_EDS(0),
    
    m_ePID(11),
    m_nProtoP(0),
    m_nTotalElecPosiMade(0),

    /// TDR PID
    m_dll_EcalPIDe_Type     ("Associator<TrStoredTrack,float>"),
    m_dll_EcalPIDe_Name     ("EcalPIDe"),
    m_dll_EcalPIDe          ( 0 ),
    //
    m_dll_PrsPIDe_Type      ("Associator<TrStoredTrack,float>"),
    m_dll_PrsPIDe_Name      ("PrsPIDe"),
    m_dll_PrsPIDe           ( 0 ),
    //
    m_dll_BremPIDe_Type     ("Associator<TrStoredTrack,float>"),
    m_dll_BremPIDe_Name     ("BremPIDe"),
    m_dll_BremPIDe          ( 0 ),
    //
    m_dll_HcalPIDe_Type     ("Associator<TrStoredTrack,float>"),
    m_dll_HcalPIDe_Name     ("HcalPIDe"),
    m_dll_HcalPIDe          ( 0 )

{
  
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);

  // Declare properties
  declareProperty( "InputProtoP"         , m_chargedinput =
                                    ProtoParticleLocation::Charged);
  
  declareProperty( "ChoiceOnBremPhot"    , m_choiceOnBremPhot     );
  
  declareProperty( "CutOnChi2BremMatchIP", m_CutOnChi2BremMatchIP );
  declareProperty( "CutOnChi2BremMatchMP", m_CutOnChi2BremMatchMP );
  declareProperty( "CutOnChi2BremMatchOP", m_CutOnChi2BremMatchOP );
  
  declareProperty( "CutCombinedPID"      , m_DLLCut                );
   
}


//=============================================================================
// Destructor
//=============================================================================
ElectronParticleMaker_bbincl::~ElectronParticleMaker_bbincl( ) { }

//=============================================================================
namespace Local
{
  // ==========================================================================
  /** @fn release 
   *  Helful function to release the interface
   *  @param  if  interface to be released 
   *  @return status code 
   */
  // ==========================================================================
  inline StatusCode release( IInterface* iif ) 
  {
    if( 0 != iif ) { iif -> release() ; }
    return StatusCode::SUCCESS ;
  };
  // ==========================================================================
};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ElectronParticleMaker_bbincl::initialize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE
      << "==> ElectronParticleMaker_bbincl:Initialising" 
      << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << " Unable to locate Event Data Service"
        << endreq;
    return sc;
  }

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  msg << MSG::VERBOSE << " Looking for Particle Property Service" << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << " Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }

  
  Local::release ( m_dll_EcalPIDe ) ;
  sc = toolSvc()->
    retrieveTool( m_dll_EcalPIDe_Type ,m_dll_EcalPIDe_Name , m_dll_EcalPIDe ) ;
  if ( 0 == m_dll_EcalPIDe) { 
    msg << MSG::ERROR << "Cannot retrieve EcalPIDe tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  Local::release ( m_dll_PrsPIDe ) ;
  sc = toolSvc()->
    retrieveTool( m_dll_PrsPIDe_Type ,m_dll_PrsPIDe_Name , m_dll_PrsPIDe ) ;
  if ( 0 == m_dll_PrsPIDe) {
    msg << MSG::ERROR << "Cannot retrieve PrsPIDe tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  Local::release ( m_dll_BremPIDe ) ;
  sc = toolSvc()->
    retrieveTool( m_dll_BremPIDe_Type ,m_dll_BremPIDe_Name , m_dll_BremPIDe ) ;
  if ( 0 == m_dll_BremPIDe) {
    msg << MSG::ERROR << "Cannot retrieve BremPIDe tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  Local::release ( m_dll_HcalPIDe ) ;
  sc = toolSvc()->
    retrieveTool( m_dll_HcalPIDe_Type ,m_dll_HcalPIDe_Name , m_dll_HcalPIDe ) ;
  if ( 0 == m_dll_HcalPIDe) { 
    msg << MSG::ERROR << "Cannot retrieve HcalPIDe tool" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* ElectronParticleMaker_bbincl::eventSvc() const
{
  return m_EDS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ElectronParticleMaker_bbincl::makeParticles(
                                                     ParticleVector & parts ) {
  
  MsgStream  msg( msgSvc(), name() );
  
  msg << MSG::VERBOSE 
      << "==> ElectronParticleMaker_bbincl::makeParticles() is running"
      << endreq;

  // make electron particles:
  int nElectronParticles=0;   // Counter of "electron" particles created.

  SmartDataPtr<ProtoParticles> chargedcandidates ( eventSvc(),m_chargedinput);
  if ( !chargedcandidates ){
    msg << MSG::VERBOSE << "Unable to retrieve Charged Protoparticles at "
        << m_chargedinput << endreq;
    return StatusCode::FAILURE;
  }
  if ( 0 == chargedcandidates->size() ){
    msg << MSG::VERBOSE << " No Charged ProtoParticles retrieved from "  
        << m_chargedinput << endreq;
    return StatusCode::SUCCESS;
  }
  
  m_nProtoP = m_nProtoP + chargedcandidates->size();

  // Iterator on ProtoParticles.
  ProtoParticles::iterator icand;
  for( icand=chargedcandidates->begin();
       icand!=chargedcandidates->end(); icand++ ) {
    
    // first retrieve bremphoton
    // we declare brem phot momentum  = 0
    // and if we don't find any brem phot it 
    // remains = 0
    HepLorentzVector  m_4momOfBremPhot = 0;
    
    if ( m_choiceOnBremPhot == "NO" ){ 
      /* user decide to do 
         not recover photon
         so nothing to do */ }
    
    if ( m_choiceOnBremPhot == "BREMMATCH" ){
      /*  use Chi2 Brem estimator 
          (chi2 of track before magnet and 
          neutral clusters matching)
          to recover Brem Phot         */
      
      double chi2Brem  = chi2BremMatch(*icand);
      double Xcluster  = XClusterBremMatch(*icand);
      double Ycluster  = YClusterBremMatch(*icand);
      
      // inner part of ECAL
      if( (Xcluster<970.0) && (Ycluster<727.0) ){
        if ( chi2Brem <= m_CutOnChi2BremMatchIP ){
          m_4momOfBremPhot = MomOfBremPhBremMatch(*icand);
        }
      }
      // middle part of ECAL
      if( (Xcluster<1939.0) && (Ycluster<1212.0) ) {
        if ( (Xcluster<970.0) && (Ycluster<727.0) ) { /* do nothing */ }
        else {
          if ( chi2Brem <= m_CutOnChi2BremMatchMP ) {
            m_4momOfBremPhot =  MomOfBremPhBremMatch(*icand);
          }
        }
      }
      // outer part  
      if( (Xcluster<1939.0) && (Ycluster<1212.0) ) { /* do nothing */ }
      else {
        if ( chi2Brem <= m_CutOnChi2BremMatchOP ) {
          m_4momOfBremPhot = MomOfBremPhBremMatch(*icand);
        }
      }
    }

    /* now we identify e+e- by using the 'last' update
       ( RICH ECAL (chi2 2d 3d and Brem) HCAL and MUON */
    
    double deltaLL = TDRCombinedPID(*icand);
    if( deltaLL > m_DLLCut ){
      
      Particle* particle = new Particle();
      double protoCL = 1.0 ;         
      double protoID = - m_ePID * ( (*icand)->charge() );
      fillParticle( *icand, protoID, protoCL, particle, 
                    m_4momOfBremPhot );
      // Insert particle in particle vector.
      parts.push_back(particle);
      nElectronParticles++;
      m_nTotalElecPosiMade++;
    }
    
  }///for icand
  
  msg << MSG::VERBOSE << "Number of electrons/positons created : " 
      << nElectronParticles
      << endreq;
  msg << MSG::VERBOSE 
      << "==> ElectronParticleMaker_bbincl::makeParticles() is finish"
      << endreq;
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
// fill particles
//=============================================================================
StatusCode ElectronParticleMaker_bbincl::fillParticle( 
                                                const ProtoParticle* protoCand,
                                                int protoID, double protoCL,
                                                Particle* particle, 
                                                HepLorentzVector  m_momentum) {
					         
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::VERBOSE << "==> ElectronParticleMaker_bbincl::run fillParticle " 
      << endreq;
  // Start filling the particle:     
  particle->setParticleID( protoID );
  particle->setConfLevel( protoCL );
  //Get mass from Particle Property Service to calculate the 4-momentum
  ParticleProperty* partProp = m_ppSvc->findByStdHepID( protoID );     
  double mass = (*partProp).mass();
  particle->setMass(mass);
  particle->setMassErr(0.0); // For the moment but already in constructor
  particle->setIsResonance(false);  // Already in constructor

  const TrStateP* trackState = protoCand->trStateP();
  if( !trackState ) return StatusCode::SUCCESS;
  
  // Set position of first measured point on track:
  HepPoint3D position( trackState->x(), trackState->y(), trackState->z() ) ;
  particle->setPointOnTrack( position );
  HepLorentzVector quadriMomentum;
  // Calculate and set four momentum: do this in ProtoParticle... 
  double momentum = trackState->p();
  double slopeX   = trackState->tx();
  double slopeY   = trackState->ty();	  
  double pZ = momentum/sqrt( 1.0 + slopeX*slopeX + slopeY*slopeY );

  double pXphoton = m_momentum.px();
  double pYphoton = m_momentum.py();
  double pZphoton = m_momentum.pz();
  double Ephoton  = m_momentum.e();

  /// i add photon momentum but if user don't want
  /// to add brem photon or brem cluster is outside
  /// the cut , m_momentum = 0 

  quadriMomentum.setPx( pZ*slopeX + pXphoton );
  quadriMomentum.setPy( pZ*slopeY + pYphoton );
  quadriMomentum.setPz( pZ  + pZphoton );
  quadriMomentum.setE( sqrt( mass*mass + momentum*momentum ) + Ephoton );
  
  particle->setMomentum( quadriMomentum );
    
  // Retrieve track state covariance matrix and set particle error matrices:
  const HepSymMatrix& trkCov = trackState->pCovMatrix();
    
  // Set pointOnTrackErr: (Error on x and y. No error on z!)
  HepSymMatrix pointOnTrackErr(3, 0);
  pointOnTrackErr = trkCov.sub(1,3);
  pointOnTrackErr(3,1) = 0.0;
  pointOnTrackErr(3,2) = 0.0;
  pointOnTrackErr(3,3) = 0.0;
  particle->setPointOnTrackErr(pointOnTrackErr);
  
  // Set slope+Momentum error:
  HepSymMatrix slpMomErr(3, 0);
  slpMomErr = trkCov.sub(3,5);
  particle->setSlopesMomErr(slpMomErr);

  // Set position-slopes correlation matrix. 
  // Position X Momentum correlation matrix also automatically set.
  // No correlation with Z
  HepMatrix posSlopesCorr(3, 3, 0);
  int i, j;
  for( i = 1; i <= 3; i++ ) {
    for ( j = 1; j <= 2; j++ ) {
      int istart = i+2;
      posSlopesCorr(i,j) = trkCov(istart,j);
    }
  }
  particle->setPosSlopesCorr(posSlopesCorr);

  particle->setOrigin(protoCand);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ElectronParticleMaker_bbincl::finalize() {
    
  MsgStream msg(msgSvc(), name());
  
  // TDR ID
  Local::release ( m_dll_EcalPIDe  ) ;
  Local::release ( m_dll_PrsPIDe   ) ;
  Local::release ( m_dll_BremPIDe  ) ;
  Local::release ( m_dll_HcalPIDe  ) ;



  msg << MSG::WARNING 
      << "************************************************************"
      <<endreq;
  
  msg << MSG::WARNING 
      << " Counter : Number of ProtoParticle retrieved    =  "
      << m_nProtoP
      << endreq;
  
  msg << MSG::WARNING 
      << " Counter : TOTAL Number of e+e- created         =  " 
      << m_nTotalElecPosiMade
      << endreq;
  
  msg << MSG::WARNING 
      << "************************************************************"
      <<endreq;

  msg << MSG::VERBOSE << "==> ElectronParticleMaker_bbincl::finalizing" 
      << endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Method to retrieve chi2 of CaloEMatch (if info not available = 10000)
//=============================================================================
double ElectronParticleMaker_bbincl::chi2BremMatch( ProtoParticle * proto ) 
{
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::VERBOSE << " ==> chi2BremMatch is running ! " << endreq;
  
  double chi2 = 10000;
  
  ProtoParticle::PIDDetVector::iterator iProtoDetID;
  for ( iProtoDetID=proto->pIDDetectors().begin();
          iProtoDetID!=proto->pIDDetectors().end(); ++iProtoDetID){
    if ( (*iProtoDetID).first == ProtoParticle::BremMatch ){
      chi2 = (*iProtoDetID).second;
    }
  }
  
  return chi2;
}

//=============================================================================
// Method to retrieve Momentum of Brem Photon with BremMatch technique
//              (if no photon retrieved return 0)
//=============================================================================
HepLorentzVector ElectronParticleMaker_bbincl::MomOfBremPhBremMatch(
                                                        ProtoParticle* proto ){
  
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::VERBOSE << " ==>  MomOfBremPhBremMatch is running "
      << endreq;

  HepLorentzVector  m_momentumFromBremMatch= 0;

  /// Retrieve Reference to possible calorimeters result (non-const)
  SmartRefVector<CaloHypo>& m_hypo = proto->calo(); 
  
  if( m_hypo.size() != 0 ){
    
    if( m_hypo.size() == 2 ){/// the second hypo is the brem phot
      
      SmartRefVector<CaloHypo>::iterator itHypo;
      
      for( itHypo  = m_hypo.begin() ; 
           itHypo != m_hypo.end()   ;
           itHypo++ ) {

        // take the momentum from Hypo object  
        const CaloMomentum* momentum = (*itHypo)->momentum() ;
        if( 0 != momentum ) {
          m_momentumFromBremMatch = momentum->momentum();
        }
      }
    } /// loop over hypos
  }

  return m_momentumFromBremMatch;

}

//=============================================================================
// Method to retrieve x of the brem photon cluster with BremMatch technique 
//             (if no photon retrieved return 0)
//=============================================================================
double ElectronParticleMaker_bbincl::XClusterBremMatch(ProtoParticle* proto){
  
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::VERBOSE << " ==>  XClusterBremMatch is running "
      << endreq;

  double XCluster = 1000000;
  
  /// Retrieve Reference to possible calorimeters result (non-const)
  SmartRefVector<CaloHypo>& m_hypo = proto->calo();
  
  if( m_hypo.size() != 0 ){

    if( m_hypo.size() == 2 ){/// the second hypo is the brem phot one
      
      SmartRefVector<CaloHypo>::iterator itHypo;
      
      for( itHypo  = m_hypo.begin() ; 
           itHypo != m_hypo.end()   ;
           itHypo++ ) {
    
        SmartRefVector<CaloCluster> clusters = (*itHypo)->clusters();
        
        SmartRefVector<CaloCluster>::iterator icluster;
        for( icluster = clusters.begin() ;
             icluster != clusters.end()  ;
             icluster++ ) {
          XCluster = (*icluster)->position().x();
        }
      }
    }
  }

  return XCluster; 

}

//=============================================================================
// Method to retrieve x of the brem photon cluster with BremMatch technique 
//             (if no photon retrieved return 0)
//=============================================================================
double ElectronParticleMaker_bbincl::YClusterBremMatch(ProtoParticle* 
                                                                        proto){
  
  MsgStream  msg( msgSvc(), name() );

  msg << MSG::VERBOSE << " ==>  XClusterBremMatch is running "
      << endreq;

  double YCluster = 1000000;
  
  /// Retrieve Reference to possible calorimeters result (non-const)
  SmartRefVector<CaloHypo>& m_hypo = proto->calo();
  
  if( m_hypo.size() != 0 ){

    if( m_hypo.size() == 2 ){/// the second hypo is the brem phot
      
      SmartRefVector<CaloHypo>::iterator itHypo;
      
      for( itHypo  = m_hypo.begin() ; 
           itHypo != m_hypo.end()   ;
           itHypo++ ) {
    
        SmartRefVector<CaloCluster> clusters = (*itHypo)->clusters();
        
        SmartRefVector<CaloCluster>::iterator icluster;
        for( icluster = clusters.begin() ;
             icluster != clusters.end()  ;
             icluster++ ) {
          YCluster = (*icluster)->position().y();
        }
      }
    }
  }

  return YCluster;

}



//=============================================================================
// Method to calc de of combined PID (Roger Forty)           
//=============================================================================
double ElectronParticleMaker_bbincl::TDRCombinedPID( ProtoParticle* proto ){

  MsgStream  msg( msgSvc(), name() );

  msg << MSG::VERBOSE << " ==> deCombinedPID  is running "
      << endreq;

  double se  = -1;
  double smu = -1;
  double spi = -1;
  double sk  = -1;
  double sp  = -1;
  
  // Combine particle-ID likelihoods from different subdetectors
  
  double re  = 0.;    // log-likelihoods for the 5 hypotheses
  double rmu = 0.;
  double rpi = 0.;
  double rk  = 0.;
  double rp  = 0.;
      
  //retrieve RICH informations
  RichPID* m_richPID = proto->richPID();
  if ( m_richPID ){
   se  = m_richPID -> particleDeltaLL ( Rich::Electron );
   smu = m_richPID -> particleDeltaLL ( Rich::Muon );
   spi = m_richPID -> particleDeltaLL ( Rich::Pion );
   sk =  m_richPID -> particleDeltaLL ( Rich::Kaon );
   sp =  m_richPID -> particleDeltaLL ( Rich::Proton );
  }
      
  // Take RICH values from particleDeltaLL variables
  if (spi > -0.5) {   // RICH data available
    re  = -se;
    rmu = -smu;
    rpi = -spi;
    rk  = -sk;
    rp  = -sp;
  }
  
  // retrieve MUON information
  double mmu = -1.;
  bool xmu = 0;
      
  MuonID* m_muonPID = proto->muonPID();
  if ( m_muonPID ){
    mmu = m_muonPID -> MuProb();
    xmu = m_muonPID -> IsMuon();
  }
  
  // For muon ID, add information from muon detector 
  if (xmu) {           // Check that IsMuon flag is set
    double mulik;
    double mubkg;
    
    // Use parametrization of MuProb likelihood
    if (mmu < 0.01) {
      mulik = 132.;
      mubkg = 400.;
    }
    else if (mmu >= 1.0) {
      mulik = 3616.;
      mubkg = 200.;
    }
    else {
      mulik = exp(-3.576+9.36*mmu)+exp(0.7447+4.173*mmu);
      mubkg = exp(1.593+1.883*mmu);
    }
    rmu = rmu + log(mulik / 105.56);  // Update muon log-likelihood
    re  = re  + log(mubkg / 21.80);   // and background hypotheses
    rpi = rpi + log(mubkg / 21.80);
    rk  = rk  + log(mubkg / 21.80);
    rp  = rp  + log(mubkg / 21.80);
  }
      
  // For each particle type, calculate estimator as difference between
  // log-likelihood for that hypothesis and the most likely amongst
  // the other hypotheses
  double rmax = rmu;
  if (rpi > rmax) rmax = rpi;
  if (rk > rmax) rmax = rk;
  if (rp > rmax) rmax = rp;
  double de = re - rmax;        // electron

  // now retrieve CALORIMETER INFO
  const TableTr* ecalPIDe = m_dll_EcalPIDe -> direct () ;
  if( 0 == ecalPIDe ) {
    msg << MSG::ERROR << " Table  'EcalPIDe'   points to NULL!" << endreq;
    return StatusCode::FAILURE;
  }
  
  const TableTr* prsPIDe  = m_dll_PrsPIDe  -> direct () ;
  if( 0 == prsPIDe  ) {
    msg << MSG::ERROR << " Table  'PrsPIDe'   points to NULL!" << endreq;
    return StatusCode::FAILURE;
  }
  
  const TableTr* bremPIDe = m_dll_BremPIDe -> direct () ;
  if( 0 == bremPIDe ) { 
    msg << MSG::ERROR << " Table 'BremPIDe'   points to NULL!" << endreq;
    return StatusCode::FAILURE;
  }
  
  const TableTr* hcalPIDe = m_dll_HcalPIDe -> direct () ;
  if( 0 == hcalPIDe ) {
    msg << MSG::ERROR << " Table 'HcalPIDe'   points to NULL!" << endreq;
    return StatusCode::FAILURE;
  }

  // then retrieve track
  const TrStoredTrack* m_track = proto->track();
  
  // DLL with chi2 3d
  {
    const RangeTr range = ecalPIDe -> relations ( m_track );
    if( !range.empty() ) { 
      double m_ecalpide = range.front().to() ;
      de = de + m_ecalpide;
    }
  }
  // DLL with PRS
  {
    const RangeTr range = prsPIDe -> relations ( m_track );        
    if( !range.empty() ) { 
      double m_prspide = range.front().to() ; 
      de = de + m_prspide;
    }
  }
  // DLL with chi2 brem
  {
    const RangeTr range =  bremPIDe -> relations ( m_track );
    if( !range.empty() ) { 
      double m_brempide = range.front().to() ; 
      de = de + m_brempide;
    }
  }
  // DLL with HCAL chi2      
  {
    const RangeTr range = hcalPIDe -> relations ( m_track );  
    if( !range.empty() ) { 
      double m_hcalpide = range.front().to() ;
      de = de + m_hcalpide;
    }
  }

  return de;
  
}








    
