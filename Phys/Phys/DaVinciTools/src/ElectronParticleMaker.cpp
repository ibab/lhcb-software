// $Id: ElectronParticleMaker.cpp,v 1.4 2003-08-05 17:25:19 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"

// from Event 
#include "Event/MCParticle.h"
#include "Event/TrStoredTrack.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// local
#include "ElectronParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ElectronParticleMaker
//
// Description: This tool fills the particle class with information from
// ProtoParticles and places it in the Transient Event Store
// 12/06/2002 : Jose Helder Lopes.
// 18/07/2002 : Gloria Corti
//-----------------------------------------------------------------------------
// Declaration of the Tool Factory
static const  ToolFactory<ElectronParticleMaker>          s_factory ;
const        IToolFactory& ElectronParticleMakerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ElectronParticleMaker::ElectronParticleMaker( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_ppSvc(0)
  , m_EDS(0)
  , m_nTotalElecPosiMade(0)
  , m_eID(11){
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);

  // Declare properties
  declareProperty( "InputProtoP", m_chargedinput =
                                              ProtoParticleLocation::Charged);
  declareProperty( "CutOnChi2CaloTrMatch", m_CutOnChi2CaloTrMatch );
  declareProperty( "CutOnChi2CaloEMatch", m_CutOnChi2CaloEMatch );
  declareProperty( "CutOnChi2BremMatch", m_CutOnChi2BremMatch );
  declareProperty( "CutOnRichProb", m_CutOnRichProb  );
  declareProperty( "TypeOfIdentification", m_TypeOfIdentification );
  declareProperty( "ChoiceOnBremPhot", m_choiceOnBremPhot );
  declareProperty( "CutOnClusterDist", m_cutOnClusterDist );
  declareProperty( "CutOndeOfCombinedPID", m_deCut );
  declareProperty( "ChoiceForFlag", m_flag );
  
}

//=============================================================================
// Destructor
//=============================================================================
ElectronParticleMaker::~ElectronParticleMaker( ) { }

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ElectronParticleMaker::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> ElectronParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << " Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::DEBUG << " Looking for Particle Property Service" << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << " Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;

}

//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* ElectronParticleMaker::eventSvc() const
{
  return m_EDS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ElectronParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  logbk( msgSvc(), name() );
  
  logbk << MSG::DEBUG << "==> ElectronParticleMaker::makeParticles() is running"
      << endreq;

  // make electron particles:
  int nElectronParticles=0;   // Counter of "electron" particles created.

  SmartDataPtr<ProtoParticles> chargedcandidates ( eventSvc(),m_chargedinput);
  if ( !chargedcandidates ) { 
    logbk << MSG::INFO << " No Charged ProtoParticles retrieved from "  
        << m_chargedinput << endreq;
    return StatusCode::SUCCESS;
  }
  if ( 0 == chargedcandidates->size() ) { 
    logbk << MSG::INFO << " No Charged ProtoParticles retrieved from "  
        << m_chargedinput << endreq;
    return StatusCode::SUCCESS;
  }

  // Logbk number of ProtoPartCandidates retrieved
  logbk << MSG::INFO << " Number of Charged ProtoParticles retrieved   = "
      << chargedcandidates->size() << endreq;
  
  // Loop over all Protoparticles and fill Particle if
  // satisfies the detPID and corresponding CL cut  :
  ProtoParticles::iterator icand;  // Iterator on ProtoParticles.
  for( icand=chargedcandidates->begin();
       icand!=chargedcandidates->end(); icand++ ) {
    
    ///Lorentz 4-vect for brem photons
    HepLorentzVector  m_photonmomentum = 0; 
    
    /// here we search for brem photons if the user want it
    /// and set  m_photonmomentum to the best photon candidate
    /// if the not m_photonmomentum remains = 0
    
    if ( m_choiceOnBremPhot == 1 ) { /// add brem photons
      /// retrieve calohypo for photons
      SmartDataPtr<CaloHypos> photons ( eventSvc(), "Rec/Calo/Photons" );
      if( !photons || 0 == photons->size() ) {
        logbk << MSG::INFO 
            << "Failed to locate CaloHypos at Rec/Calo/Photons " 
            << endreq;
      }
      /// retrieve track from which comes this protoparticle
      const TrStateP* trackState = (*icand)->trStateP();
      if( !trackState ){
        logbk << MSG::WARNING 
            << " There is no TrStateP corresponding to this ProtoParticle "
            << endreq;
        return StatusCode::SUCCESS;
      }
      double slopeX   = trackState->tx();
      double slopeY   = trackState->ty();
      double X        = trackState->x();
      double Y        = trackState->y();
      double Z        = trackState->z();
      /// now that we have the track, extrapolate it to ECAL 
      double ZEcal = 12490;
      double XPred = X + ( ZEcal - Z ) * slopeX ;
      double YPred = Y + ( ZEcal - Z ) * slopeY ;
      double dist = 0;
      double smallestdist = 10000;
      double XCluster;
      double YCluster;
      /// get X and Y of the clusters
      CaloHypo* m_closestPhoton;
      for ( CaloHypos::const_iterator iphoton = photons->begin();
            photons->end() != iphoton ; ++iphoton ) {
        typedef  SmartRefVector<CaloCluster> Clusters ;
        const Clusters& clusters = (*iphoton)->clusters();
        for( Clusters::const_iterator icluster = clusters.begin() ;
             clusters.end() != icluster ; ++icluster ) {
          const CaloCluster* cluster = *icluster ;
          if( 0 == cluster ) { continue ;}
          XCluster = cluster->position().x();
          YCluster = cluster->position().y();
        } /// FOR ( CALOCLUSTERS )
        dist = sqrt( (XCluster-XPred)*(XCluster-XPred) + 
                     (YCluster-YPred)*(YCluster-YPred) );
        if ( dist < smallestdist ){
          smallestdist = dist;
          m_closestPhoton = *iphoton;
        }
      } /// FOR ( CALOHYPO )
 
      if ( smallestdist < m_cutOnClusterDist ){
        /// retrieve CaloMomentum of the closest photon
        CaloHypo::Momentum* m_caloMomentumOfClosestPhot =
          m_closestPhoton->momentum();
        m_photonmomentum = m_caloMomentumOfClosestPhot->momentum();
      } /// IF (CUT ON CLUSTER DIST )
    } /// IF ( CHOICE ON BREM PHOTON )


    ProtoParticle::PIDDetVector::iterator iProtoDetID;
    for ( iProtoDetID=(*icand)->pIDDetectors().begin();
          iProtoDetID!=(*icand)->pIDDetectors().end(); ++iProtoDetID){
      
      ////////////////////
      /// ID WITH CALO ///
      ////////////////////
      if ( m_TypeOfIdentification == 1 ){ 
        if ( (*iProtoDetID).first == ProtoParticle::CaloTrMatch ){
          if ( (*iProtoDetID).second <= m_CutOnChi2CaloTrMatch ){
            ProtoParticle::PIDDetVector::iterator iProtoDetID2;
            for (iProtoDetID2=(*icand)->pIDDetectors().begin();
                 iProtoDetID2!=(*icand)->pIDDetectors().end(); ++iProtoDetID2){
              if ( (*iProtoDetID2).first == ProtoParticle::CaloEMatch ){
                if ( (*iProtoDetID2).second <= m_CutOnChi2CaloEMatch ){
                  Particle* particle = new Particle();
                  double ProtoPCharge= (*icand)->charge();
                  double protoID;
                  double protoCL;
                  if ( ProtoPCharge == 1 ){
                    protoID = -m_eID;
                    protoCL = 1.0;
                  }
                  if ( ProtoPCharge ==  (-1) ){
                    protoID = m_eID;
                    protoCL = 1.0;
                  }
                  fillParticle( *icand, protoID, protoCL, particle, 
                                m_photonmomentum );
                  // Insert particle in particle vector.
                  parts.push_back(particle);
                  nElectronParticles++;
                  m_nTotalElecPosiMade++;
                } /// IF ( CUT ON CALOEMATCH )
              } /// IF ( CALOEMATCH )
            } /// FOR iProtoDetID2
          } /// IF ( CUT ON CaloTrMatch )
        } /// IF ( CALOTRMATCH )
      } /// IF ( TYPEOFID == 1 ) 
      
      ////////////////////
      /// ID WITH RICH ///
      ////////////////////
      if ( m_TypeOfIdentification == 2 ){   
        if ( (*iProtoDetID).first == ProtoParticle::RichElectron ){
          if ( (*iProtoDetID).second > m_CutOnRichProb ){
            Particle* particle = new Particle();
            double ProtoPCharge= (*icand)->charge();
            double protoID;
            double protoCL;
            if ( ProtoPCharge == 1 ){
              protoID = -m_eID;
              protoCL = 1.0;
            }
            if ( ProtoPCharge ==  (-1) ){
              protoID = m_eID;
              protoCL = 1.0;
            }
            fillParticle( *icand, protoID, protoCL, particle, 
                          m_photonmomentum );
            // Insert particle in particle vector.
            parts.push_back(particle);
            nElectronParticles++;
            m_nTotalElecPosiMade++;
          } /// IF ( CUT ON RICH PROB )
        } /// IF ( RICHELECTRON )
      } /// IF ( TYPEOFID == 2  ) 
      
      ///////////////////////////
      /// ID WITH RICH + CALO ///
      ///////////////////////////
      if ( m_TypeOfIdentification == 3 ){  
        if ( (*iProtoDetID).first == ProtoParticle::RichElectron ){
          if ( (*iProtoDetID).second > m_CutOnRichProb ){
            ProtoParticle::PIDDetVector::iterator iProtoDetID2;
            for (iProtoDetID2=(*icand)->pIDDetectors().begin();
                 iProtoDetID2!=(*icand)->pIDDetectors().end(); ++iProtoDetID2){
              if ( (*iProtoDetID2).first == ProtoParticle::CaloTrMatch ){
                if ( (*iProtoDetID2).second <= m_CutOnChi2CaloTrMatch ){
                  ProtoParticle::PIDDetVector::iterator iProtoDetID3;
                  for (iProtoDetID3=(*icand)->pIDDetectors().begin();
                       iProtoDetID3!=(*icand)->pIDDetectors().end();
                                                               ++iProtoDetID3){
                    if ( (*iProtoDetID3).first == ProtoParticle::CaloEMatch ){
                      if ( (*iProtoDetID3).second <= m_CutOnChi2CaloEMatch ){
                        Particle* particle = new Particle();
                        double ProtoPCharge= (*icand)->charge();
                        double protoID;
                        double protoCL;
                        if ( ProtoPCharge == 1 ){
                          protoID = -m_eID;
                          protoCL = 1.0;
                        }
                        if ( ProtoPCharge ==  (-1) ){
                          protoID = m_eID;
                          protoCL = 1.0;
                        }
                        fillParticle( *icand, protoID, protoCL, particle, 
                                      m_photonmomentum );
                        // Insert particle in particle vector.
                        parts.push_back(particle);
                        nElectronParticles++;
                        m_nTotalElecPosiMade++;
                      } /// IF ( CUT ON CALOEMATCH )
                    } /// IF ( CALOEMATCH )
                  } /// FOR ( IDET3 )
                } /// IF ( CUT ON CALOTRMATCH )
              } /// IF ( CALOTRMATCH )
            } /// FOR ( IDET2 )
          } /// IF ( CUT ON RICHELECTRON )
        } /// IF ( RICHELECTRON )
      } /// IF ( TYPEOFID == 3 )

      ////////////////////
      /// ID WITH BREM ///
      ////////////////////
      if ( m_TypeOfIdentification == 4 ) { 
        if ( (*iProtoDetID).first == ProtoParticle::BremMatch ){
          if ( (*iProtoDetID).second <= m_CutOnChi2BremMatch ){
            Particle* particle = new Particle();
            double ProtoPCharge= (*icand)->charge();
            double protoID;
            double protoCL;
            if ( ProtoPCharge == 1 ){
              protoID = -m_eID;
              protoCL = 1.0;
            }
            if ( ProtoPCharge ==  (-1) ){
              protoID = m_eID;
              protoCL = 1.0;
            }
            fillParticle( *icand, protoID, protoCL, particle, 
                          m_photonmomentum);
            // Insert particle in particle vector.
            parts.push_back(particle);
            nElectronParticles++;
            m_nTotalElecPosiMade++;
          } /// IF ( CUT ON BREMMATCH )
        } /// IF ( BREMMATCH )
      } /// IF ( TYPE OF ID == 4 )
      

      //////////////////////////////////
      /// ID WITH RICH + CALO + BREM ///
      //////////////////////////////////
      if ( m_TypeOfIdentification == 5 ){  
        if ( (*iProtoDetID).first == ProtoParticle::RichElectron ){
          if ( (*iProtoDetID).second > m_CutOnRichProb ){
            ProtoParticle::PIDDetVector::iterator iProtoDetID2;
            for (iProtoDetID2=(*icand)->pIDDetectors().begin();
                 iProtoDetID2!=(*icand)->pIDDetectors().end(); ++iProtoDetID2){
              if ( (*iProtoDetID2).first == ProtoParticle::CaloTrMatch ){
                if ( (*iProtoDetID2).second <= m_CutOnChi2CaloTrMatch ){
                  ProtoParticle::PIDDetVector::iterator iProtoDetID3;
                  for (iProtoDetID3=(*icand)->pIDDetectors().begin();
                       iProtoDetID3!=(*icand)->pIDDetectors().end();
                                                               ++iProtoDetID3){
                    if ( (*iProtoDetID3).first == ProtoParticle::CaloEMatch ){
                      if ( (*iProtoDetID3).second <= m_CutOnChi2CaloEMatch ){
                        ProtoParticle::PIDDetVector::iterator iProtoDetID4;
                        for (iProtoDetID4=(*icand)->pIDDetectors().begin();
                             iProtoDetID4!=(*icand)->pIDDetectors().end();
                             ++iProtoDetID4){
                          if ((*iProtoDetID4).first==ProtoParticle::BremMatch){
                            if ((*iProtoDetID4).second <= m_CutOnChi2BremMatch){
                              Particle* particle = new Particle();
                              double ProtoPCharge= (*icand)->charge();
                              double protoID;
                              double protoCL;
                              if ( ProtoPCharge == 1 ){
                                protoID = -m_eID;
                                protoCL = 1.0;
                              }
                              if ( ProtoPCharge ==  (-1) ){
                                protoID = m_eID;
                                protoCL = 1.0;
                              }
                              fillParticle( *icand, protoID, protoCL, particle, 
                                            m_photonmomentum );
                              // Insert particle in particle vector.
                              parts.push_back(particle);
                              nElectronParticles++;
                              m_nTotalElecPosiMade++;
                            } /// IF ( CUT ON BREMMATCH )
                          } /// IF ( BREMMATCH )
                        } /// FOR ( IDET4 )
                      } /// IF ( CUT ON CALOEMATCH )
                    } /// IF ( CALOEMATCH )
                  } /// FOR ( IDET3 )
                } /// IF ( CUT ON CALOTRMATCH )
              } /// IF ( CALOTRMATCH )
            } /// FOR ( IDET2 )
          } /// IF ( CUT ON RICHELECTRON )
        } /// IF ( RICHELECTRON )
      } /// IF ( TYPEOFID == 5 )

    } /// FOR ( IDET )


    //////////////////////////////
    /// combined particle id   /// (we are still inside loop over protop)
    //////////////////////////////
    
    if ( m_TypeOfIdentification == 6 ){ 

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
      
      RichPID* m_richPID = (*icand)->richPID();
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
      
      double ce1 = -1; /// chi2 of CaloEMatch
      double ce3 = -1; /// chi2 of BremMatch
      
      ProtoParticle::PIDDetVector::iterator iDetID;
      for ( iDetID=(*icand)->pIDDetectors().begin();
            iDetID!=(*icand)->pIDDetectors().end(); ++iDetID){
        if ( (*iDetID).first == ProtoParticle::CaloEMatch ){
          ce1 = (*iDetID).second;
        }
        if ( (*iDetID).first == ProtoParticle::BremMatch ){
          ce3 = (*iDetID).second;
        }
      }
      
      // For electron ID, add information from calorimeter
      if (ce1 > -0.5) {   // CaloEMatch variable available
        double elik;
        double ebkg;
        double rbkg;
        
        // Use parametrizations of likelihood in 3 bins of momentum
        double qp = 0.0;
        const TrStateP* tkstate = (*icand)->trStateP();
        if( tkstate ) {
          qp = tkstate->qDivP();
        }
        if (0.001/fabs(qp) < 10.) {         // p < 10 GeV
          elik = (3009.*exp(-ce1/21.41)+72.56*exp(-ce1/140.6)+2.005)
            *(1.-exp(-ce1/7.700)) / 61.32;
          ebkg = (428.6*exp(-ce1/328.3)+913.4*exp(-ce1/271.7)+160.0)
            *(1.-exp(-ce1/15.31)) / 515.73;
        }
        else if (0.001/fabs(qp) < 20.) {    // 10 < p < 20 GeV
          elik = (939.0*exp(-ce1/34.62)+32.26*exp(-ce1/122.9)+1.237)
            *(1.-exp(-ce1/6.708)) / 33.14;
          ebkg = 226.5*exp(-ce1/1208.)*(1.-exp(-ce1/128.3)) / 128.84;
        }
        else {                              // p > 20 GeV
          elik = (566.7*exp(-ce1/48.23)+87.73*exp(-ce1/136.8)+1.562)
            *(1.-exp(-ce1/12.87)) / 34.93;
          ebkg = 41.91*exp(-ce1/6581.)*(1.-exp(-ce1/175.8)) / 33.07;
        }
        re = re + log(elik);  // Update log-likehood for e
        rbkg = log(ebkg);     // and for background (non-e)
        
        // Add information from BremMatch variable (if available)
        if (ce3 > -0.5) {
          elik = exp(7.621-0.09495*ce3) + exp(5.777-0.01221*ce3) + 26.77;
          ebkg = exp(5.328-0.002751*ce3) + 194.4;
          re = re + log(elik/75.14);
          rbkg = rbkg + log(ebkg/265.65);
        }
        rmu = rmu + rbkg;  // Update background hypotheses
        rpi = rpi + rbkg;
        rk  = rk  + rbkg;
        rp  = rp  + rbkg;
      }
      
      double mmu = -1.;
      bool xmu = 0;
      
      MuonID* m_muonPID = (*icand)->muonPID();
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

      // Flag to indicate which subdetector information was used
      int flag = 0;
      if (spi > -0.5) flag = flag + 1;
      if (ce1 > -0.5) flag = flag + 2;

      if ( flag == m_flag ){
        if ( de > m_deCut ){
          Particle* particle = new Particle();
          double ProtoPCharge= (*icand)->charge();
          double protoID;
          double protoCL;
          if ( ProtoPCharge == 1 ){
            protoID = -m_eID;
            protoCL = 1.0;
          }
          if ( ProtoPCharge ==  (-1) ){
            protoID = m_eID;
            protoCL = 1.0;
          }
          fillParticle( *icand, protoID, protoCL, particle, 
                        m_photonmomentum );
          // Insert particle in particle vector.
          parts.push_back(particle);
          nElectronParticles++;
          m_nTotalElecPosiMade++;
        } /// if de > cutDe
      } /// if flag ==
    } /// IF ( ID == 8 )

  } /// FOR ( ICAND )
  
  logbk << MSG::INFO << "Number of electrons/positons created : " 
      << nElectronParticles
      << endreq;
  logbk << MSG::DEBUG << "==> ElectronParticleMaker::makeParticles() is finish"
      << endreq;
  
  return StatusCode::SUCCESS;

}

//=============================================================================
// fill particles
//============================================================================= 
StatusCode ElectronParticleMaker::fillParticle( const ProtoParticle* protoCand,
                                                int protoID, double protoCL,
                                                Particle* particle, 
                                                HepLorentzVector  m_momentum ) {
					         
  MsgStream  log( msgSvc(), name() );

  log << MSG::DEBUG << "==> ElectronParticleMaker::run fillParticle " 
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
  double Ephoton = m_momentum.e();

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
StatusCode ElectronParticleMaker::finalize() {
    
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "TOTAL Number of electrons/positons created : " 
      << m_nTotalElecPosiMade
      << endreq;

  log << MSG::DEBUG << "==> ElectronParticleMaker::finalizing" << endreq;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
