// $Id: $
// Include files
#include <boost/foreach.hpp>
#include "boost/algorithm/string/replace.hpp"

// from LHCb

#include "Event/ProtoParticle.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloPosition.h"
#include "Event/CaloHypo.h"
#include "CaloDet/DeCalorimeter.h"
#include "Kernel/CaloCellID.h"
#include "Kernel/CaloCellCode.h"
#include "Kernel/PFParticle.h"
#include "Event/Track.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Kernel/TransporterFunctions.h"
#include "CaloDet/DeCalorimeter.h"

// Relations
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted2D.h"
#include "Relations/Get.h"
#include "LoKi/PhysExtract.h"
#include "LoKi/ParticleCuts.h"
// local
#include "ParticleFlow.h"


//-----------------------------------------------------------------------------
// Implementation file for class : ParticleFlow
//
// 2011-05-10 : Victor Coco and Cedric Potterat
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ParticleFlow )


    //=============================================================================
    // Standard constructor, initializes variables
    //=============================================================================
ParticleFlow::ParticleFlow( const std::string& name,
        ISvcLocator* pSvcLocator)
    : GaudiTupleAlg ( name , pSvcLocator )
      , m_trSel ( 0 )
      , m_ppSvc ( 0 )
      , m_clust2TrLocationECAL ( LHCb::CaloAlgUtils::CaloIdLocation("ClusterMatch", context()) )
      , m_clust2TrLocationHCAL ( "Rec/Calo/HcalClusterMatch" )
      //, m_p2s ( 0 )
      // , m_filterGhost ( 0 )
      , m_protoPF ( "Rec/ProtoP/PF" )
      , m_calohypoPF ( "Rec/Calo/Hadrons" )
{
    // Default options are equivalent to StdNoPIDsPions+StdNoPIDsDownPions+StdLoosePhotonAll
    declareProperty( "PFOutputLocation"          ,  m_PFOutputLocation  =  "Phys/PFParticles/Particles"
            ,  "Output Location of the particles 4 jets") ;


    declareProperty( "TrackSelectorType", m_trSelType = ""
            , "TrackSelector for choice of track inputs");
    declareProperty( "PFProtoParticlesOutputLocation", m_protoPF
            , "Location for the protoparticles created in PF");
    declareProperty( "PFCaloHypoOutputLocation", m_calohypoPF
            , "Location for the calohypo created in PF");

    // Track related cuts
    //declareProperty( "UseTTHits" , m_useTTHits  = false ,
    //                 "Use TT info to kill long ghost");
    declareProperty( "MinInfMomentumCut" , m_cutInfMomTRVal  = -1. ,
            "Minimal cut on sigma_(q/p)/(q/p)");
    declareProperty( "MinInfMomentumCutDown" , m_cutInfMomTRVal_Down  = -1. ,
            "Minimal cut on sigma_(q/p)/(q/p) for downstream tracks");
    declareProperty( "MinInfMomentumCutUp" , m_cutInfMomTRVal_Up  = -1. ,
            "Minimal cut on sigma_(q/p)/(q/p) for upstream tracks");
    //declareProperty( "MaxChi2NoTT" , m_noTTChi2PerDof = 10. ,
    //                 "Maximum chi2 per DoF for long track with no TTHits");

    declareProperty( "ParticleLocations" ,m_particleLocations ,
            "Location of premade particles to be used" );
    declareProperty( "CompositeParticleLocations" ,m_compositeParticleLocations ,
            "Location of composite particles to be used" );
    declareProperty( "CandidateToBanLocation", m_banCandidatesLocations,
            "Location of candidate not to be included for Jet reco" );
    declareProperty( "CandidatesToKeepLocation", m_keepCandidatesLocations,
            "Location of candidate to be kept for PF, no extra cut" );


    // Calo matching related cuts
    declareProperty( "MaxMatchECALTr" , m_Chi2ECALCut = 25. ,
            "Maximal ecal<-->track matching chi2");
    declareProperty( "MaxMatchECALTr_T" , m_Chi2ECALCutTT = 25. ,
            "Maximal ecal<-->TtrackT matching chi2");

    declareProperty( "UseHCAL" , m_useHCAL  = true ,
            "Use HCAL Clusters");


    declareProperty( "MaxMatchHCALLowEValue" , m_Chi2HCAL0CutEValue = 1000.
            , "Low energy when matching HCAL to tracks");
    declareProperty( "MaxMatchHCALHighEValue" , m_Chi2HCAL1CutEValue = 5000.
            , "High energy when matching HCAL to tracks");

    declareProperty( "MaxMatchHCALTrSmallE" , m_Chi2HCAL0Cut = 25. ,
            "Maximal hcal P < MaxMatchHCALLowEValue <-->track matching chi2");
    declareProperty( "MaxMatchHCALTrMediumE" , m_Chi2HCAL1Cut = 25. ,
            "Maximal hcal MaxMatchHCALLowEValue < P < MaxMatchHCALHighEValue <-->track matching chi2");
    declareProperty( "MaxMatchHCALTrLargeE" , m_Chi2HCAL2Cut = 25. ,
            "Maximal hcal P > MaxMatchHCALHighEValue <-->track matching chi2");
    declareProperty( "VerticesLocation", m_verticesLocation = LHCb::RecVertexLocation::Primary
            , "Location of vertices to which charged paritcles should be related");
    declareProperty( "UseVelo", m_useVelo = false
            , "Use Velo tracks turning them into 1*eV mommentum particles");



    // Neutral related cuts
    declareProperty( "MinPhotonID4Photon" , m_photonID4Photon = -1. ,
            "Minimum photon pid for photon hypothesis");
    declareProperty( "MinPhotonID4PhotonTtrack" , m_photonID4PhotonTban = -2. ,
            "Minimum photon pid for photon hypothesis of T matched cluster");
    declareProperty( "MinPhotonIDMax4ResolvedPi0" , m_photonIDMax4ResolvedPi0 = -4. ,
            "Minimum photon pid for photon from resolved Pi0");
    declareProperty( "MinPhotonIDMin4ResolvedPi0" , m_photonIDMin4ResolvedPi0 = -2. ,
            "Minimum photon pid for photon from resolved Pi0");
    declareProperty( "MinHCALE",m_minHCALE = 0.,
            "Minimum value to take a HCAL cluster into account");
    declareProperty( "MinHCALEt",m_minHCALEt = 2500.,
            "Minimum Et value to take a HCAL cluster into account");
    declareProperty( "MinHCALEt4Eta4", m_minHCALEt4Eta4 = 5000.,
            "Minimum Et value to take a HCAL cluster into account for large eta > 4");
    declareProperty( "MinIsoPhotonEt", m_minIsoPhotonEt = 2500.,
            "Minimum Et value an isolated photon into account");
    declareProperty( "MinPhotonEt", m_minPhotonEt = 400.,
            "Minimum Et value an photon into account");
    declareProperty( "MinBadPhotonEt", m_minBadPhotonEt = 2500.,
            "Minimum Et value an photon into account");
    declareProperty( "MinBadPhotonMatchingTEt", m_minBadPhotonMatchingTEt = 2500.,
            "Minimum Et value an photon into account");
    declareProperty( "UseTTrackBanning", m_banFromTTrack = true, "Use TTrack to ban neutrals");

    // Neutral recovery
    declareProperty( "NeutralRecovery", m_neutralRecovery = false
            , "Turn on the neutral recovery");
    //TODO should be ET
    declareProperty( "MinE", m_MinE = 2000.
            , "Minimal energy for a hidden neutral to be recovered");
    declareProperty( "MinEt", m_MinEt = 2500.
            , "Minimal transverse energy for a hidden neutral to be recovered");
    //declareProperty( "MinEt", m_MinEt = 500.
    //                 , "Minimal transverse energy for a cluster to be used in recovery");
    //declareProperty( "MinNegFraction", m_MinNegFrac = 0.1
    //                 , "Maximal negative value allowed for barycenter substraction");
    declareProperty( "MC_recovery", m_MC = true
            , "Is it MC? Needed for the Calo E response");
    declareProperty( "MaximumFracNeutrReco", m_maxfractionofE = 1.8
            , "Maximal fraction the expected energy can have wrt calo E in order to be taken into account");
    declareProperty( "BanInfMomentumFromNR",m_doNotUseInfMominNR = false
            , "Do not use infinite momentum tracks in neutral recovery");
    declareProperty( "OnlyBestCaloMatchForNR",m_onlyBest = true
            , "Only takes the best calo to track matching for neutral recovery");

    declareProperty( "LongProbNNghostCut",m_LongProbNNghostCut=0.5
            , "Remove trk with bad ProbNNghost before cluster banning");

    declareProperty( "UpProbNNghostCut",m_UpProbNNghostCut=0.5
            , "Remove trk with bad ProbNNghost before cluster banning");

    declareProperty( "DownProbNNghostCut",m_DownProbNNghostCut=0.
            , "Remove trk with bad ProbNNghost before cluster banning");


}

//=============================================================================
// Destructor
//=============================================================================
ParticleFlow::~ParticleFlow() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ParticleFlow::initialize() {

    StatusCode sc = GaudiAlgorithm::initialize();
    if ( sc.isFailure() ) return sc;
    if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

    // get an instance of the track selector
    if( m_trSelType != "")
        m_trSel = tool<ITrackSelector>( m_trSelType, "TrackSelector", this );

    // get the TT Hit expectation tool
    //if(m_ttExpectation == 0)
    //  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");

    // setup the vertex to particle association
    m_pf2verticesLocation = m_PFOutputLocation ;
    boost::replace_last(m_pf2verticesLocation,"/Particles","/Particle2VertexRelations");
    m_pvRelator = tool<IRelatedPVFinder>("GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2", this);

    // ParticleProperty
    if(m_ppSvc == 0){
    sc = service("LHCb::ParticlePropertySvc", m_ppSvc);
    if( sc.isFailure() ) {
        fatal() << "    Unable to locate Particle Property Service"   << endreq;
        return sc;
    }
}
    if (m_particleLocations.empty()){
        m_particleLocations.push_back("Phys/StdLooseAllPhotons/Particles");
    }

    //m_p2s = tool<IParticle2State>("Particle2State");


    m_protoMap[ "pi+" ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "pion"  , this )
            , m_ppSvc->find( "pi+" ) );
    m_protoMap[ "K+"  ] = std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "kaon"  , this )
            , m_ppSvc->find( "K+" ) );
    m_protoMap[ "p+"  ] =  std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "proton"  , this )
            , m_ppSvc->find( "p+" ) );
    m_protoMap[ "e+"  ] =  std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "electron", this )
            , m_ppSvc->find( "e+" ) );
    m_protoMap[ "mu+" ] =  std::make_pair( tool<IProtoParticleFilter>( "ProtoParticleMUONFilter"  , "muon"    , this )
            , m_ppSvc->find( "mu+" ) );

    //m_filterGhost = tool<IProtoParticleFilter>( "ProtoParticleANNPIDFilter", "ghost", this );


    m_ecal = getDet<DeCalorimeter> ( "/dd/Structure/LHCb/DownstreamRegion/Ecal" );
    m_hcal = getDet<DeCalorimeter> ( "/dd/Structure/LHCb/DownstreamRegion/Hcal" );

    //info()<<"The new PF code for no meme leak"<<endmsg;

    // For the E/p parametrisation
    m_ECALElectron_Data = new TF1("ECALElectron_Data","(-2.87675e-01 )*exp(-1.89536e-04*x)+8.86869e-01");
    m_ECALElectron_MC = new TF1("ECALElectron_MC","( -4.38526e-01 )*exp(-1.24658e-03*x)+7.22256e-01");
    m_ECALHCALHadron_Data = new TF1("ECALHCALHadron_Data","(-7.48397e-02 -1.11391e-05*x -2.51717e-09*x*x)*exp(-1.18558e-04*x)+7.76469e-01");
    m_ECALHCALHadron_MC = new TF1("ECALHCALHadron_MC",    "( 1.45913e-01 -6.70718e-05*x +1.34026e-09*x*x)*exp(-9.39504e-05*x)+8.10820e-01");
    m_ECALHadron_Data = new TF1("ECALHadron_Data","( -3.79500e-01 )*exp( -1.11353e-04*x ) + 8.38989e-01");
    m_ECALHadron_MC = new TF1("ECALHadron_MC",    "( -4.29989e-01 )*exp(-1.25334e-04*x)+9.40150e-01");
    m_HCALHadron_Data = new TF1("HCALHadron_Data","(2.21942e-01)*exp(-7.76006e-05*x)+3.51139e-01");
    m_HCALHadron_MC = new TF1("HCALHadron_MC",    "(2.16078e-01)*exp(-1.79387e-04*x)+4.77393e-01");

    return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode ParticleFlow::execute() {
    /**
     * Main function of the particle flow algorithm
     *
     * Start with loading and preprocessing data
     * Treat the charged components and the velo protoparticles
     *
     *
     */
    verbose()<<"tag 1"<<endreq;

    using namespace LHCb::Calo2Track;

    setFilterPassed(false);

    if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

    // Define / reset some counters/control containers
    m_trackKeyToBan.clear();
    m_trackKeyToKeep.clear();
    m_particleContainers.clear();
    m_PFCaloClusters.clear();

    verbose()<<"tag 2"<<endreq;
    // Load the different usefull containers
    if ( ! this->loadDatas() ){
        return Warning("Could not load datas");
    }

    verbose()<<"tag 3"<<endreq;
    // Create the output container
    std::vector<LHCb::PFParticle*> PFParticles;
    LHCb::Particles* PFParticles_ToSave = new LHCb::Particles();
    put( PFParticles_ToSave , m_PFOutputLocation );

    verbose()<<"tag 4"<<endreq;
    // Create the particle to vertex relation table
    Particle2Vertex::WTable* table = new Particle2Vertex::WTable();
    put( table, m_pf2verticesLocation );

    verbose()<<"tag 5"<<endreq;
    // Some staff for newly created particles
    //LHCb::ProtoParticles* PFProtoParticles = new LHCb::ProtoParticles();
    //put( PFProtoParticles , m_protoPF );
    //LHCb::CaloHypos* PFCaloHypos = new LHCb::CaloHypos();
    //put( PFCaloHypos , m_calohypoPF );

    // Sort and mark the composite particles and/or candidates to ban
    if ( ! this->preprocessDatas( PFParticles )){//, *table ) ){
        return Warning("Could not treat composite particles");
    }


    verbose()<<"tag 6"<<endreq;
    //************************************************
    // Start the core of the algorithm
    //************************************************

    // Standard protoparticles
    if( m_protoParticles.count("Charged") > 0.5 ){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the loop on charged particles" << endmsg;
        StatusCode sc = treatProtoParticles( "Charged" , PFParticles );//, *table );
    }
    verbose()<<"tag 7"<<endreq;

    // Home made Velo protoparticles
    if( m_protoParticles.count("Velo") > 0.5 ){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the loop on Velo particles" << endmsg;
        StatusCode sc = treatProtoParticles( "Velo" ,  PFParticles);// , *table );
    }

    verbose()<<"tag 8"<<endreq;
    // Merged Pi0
    verbose()<<"count MergedPi0 "<<m_particleContainers.count("MergedPi0")<<endreq;
    if (m_particleContainers.count("MergedPi0") > 0.5 ){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the loop on Merged Pi0" << endmsg;
        treatPi0s(LHCb::PFParticle::MergedPi0, PFParticles );
    }

    verbose()<<"tag 9"<<endreq;
    // Resolved Pi0
    verbose()<<"count ResolvedPi0 "<<m_particleContainers.count("ResolvedPi0")<<endreq;
    if (m_particleContainers.count("ResolvedPi0") > 0.5 ){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the loop on Resolved Pi0" << endmsg;
        treatPi0s(LHCb::PFParticle::ResolvedPi0, PFParticles );
    }

    verbose()<<"tag 10"<<endreq;
    /// Loop over Photons, if clusters are not tagged yet, use and tag
    if (m_particleContainers.count("Photons") > 0.5 ){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the loop on Photons" << endmsg;
        treatPhotons( PFParticles );
    }
    verbose()<<"tag 11"<<endreq;
    /// Loop over the HCAL cluster and create neutral Hadrons
    if (m_useHCAL ){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the loop on HCAL clusters" << endmsg;
        //treatNeutralHadronsAndIsolatedPhotons( PFParticles , *PFCaloHypos, *PFProtoParticles );
        treatNeutralHadronsAndIsolatedPhotons( PFParticles );
    }
    verbose()<<"tag 12"<<endreq;
    /// Perform Neutral recovery
    if (m_neutralRecovery){
        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start the neutral recovery" << endmsg;
        treatNeutralRecovery( PFParticles );
    }
    verbose()<<"Before deleting copying"<<endreq;

    // Loop over PFParticles, convert to Particles, if charged or V0 fill the vertex association.
    for (std::vector<LHCb::PFParticle*>::const_iterator i_pfp = PFParticles.begin() ; PFParticles.end()!= i_pfp ; ++i_pfp ){
        LHCb::Particle* newPart = new LHCb::Particle(*(*i_pfp));
        verbose()<<"Extrainfo: "<<newPart->info(900,-1.)<<endreq;
        if ((newPart->info(900,-1.)>1. && newPart->info(900,-1.)<10.)||(newPart->info(900,-1.)>100. && newPart->info(900,-1.)<1000.)){
            verbose()<<"Found a chargd particle, extra info has been copied"<<endreq;
            this->relate2Vertex( newPart , *table );
        }
        PFParticles_ToSave->insert( newPart );
    }
    verbose()<<"Copying and inserting finnished"<<endreq;

    // MONITORING: check all cluster are used, check there is no double counting between tracks, and between neutral calo clusters and neutral recovery clusters
    // Delete the PFCaloClusters
    for (std::map< unsigned int ,  LHCb::PFCaloCluster* >::iterator i_pfClu = m_PFCaloClusters.begin();
            m_PFCaloClusters.end() != i_pfClu ; ++i_pfClu){
        verbose()<< (*i_pfClu).second<<endreq;
        delete ( (*i_pfClu).second );
    }
    verbose()<<"Deleting PFCaloClusters finnished"<<endreq;
    for (std::vector<LHCb::PFParticle*>::const_iterator i_pfp = PFParticles.begin() ; PFParticles.end()!= i_pfp ; ++i_pfp ){
        //for (LHCb::Particles::const_iterator i_pfp = PFParticles->begin() ; PFParticles->end()!= i_pfp ; ++i_pfp ){
        verbose()<<(*i_pfp)->PFType()<<" "<<(*i_pfp)<<endreq;
        delete (*i_pfp);
    }
    verbose()<<"Deleting PFParticles components finnished"<<endreq;
    //delete PFParticles;
    //verbose()<<"Deleting PFParticles finnished"<<endreq;
    setFilterPassed ( true ) ;
    return StatusCode::SUCCESS;
    }

    //=============================================================================
    //  Finalize
    //=============================================================================
    StatusCode ParticleFlow::finalize() {

        if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

        return GaudiAlgorithm::finalize();
    }

    //=============================================================================
    //  Update the list of Calo cluster for charged particles
    //=============================================================================
    StatusCode ParticleFlow::updatePFCaloClusters( LHCb::PFParticle& theParticle , unsigned int seedID )
    {
        /**
         *
         * Update the PFCaloCluster list with a given particle
         *
         * The list of PFCaloClusters is updated if a calocluster match with the particle
         * The particle is added to the list of particule contributing to a given calorimeter cluster
         * The status of the PFCaloCluster is also updated ( discard it from neutral recovery for example )
         *
         */

        verbose()<<"Type in updatePFCaloClusters "<<theParticle.PFType()<<endreq;
        // Case of normal charged particles / composite daugthers and ChargedInfMomentum
        if( ( theParticle.PFType() > LHCb::PFParticle::Charged &&  theParticle.PFType() < LHCb::PFParticle::Neutral )
                || theParticle.PFType() == LHCb::PFParticle::ChargedInfMomentum
                || ( theParticle.PFType() == LHCb::PFParticle::Daughter
                    && ( (theParticle.PFDaugType()  > LHCb::PFParticle::Charged &&  theParticle.PFDaugType() < LHCb::PFParticle::Neutral )
                        || theParticle.PFType() == LHCb::PFParticle::ChargedInfMomentum ) ) ){

            if (theParticle.proto() == NULL || theParticle.proto()->track() == NULL ){
                Warning("this is not a proper charged particle");
                return StatusCode::FAILURE;
            }
            verbose()<<"tag UPF ch 1"<<endreq;
            const LHCb::Track * track = theParticle.proto()->track();
            // Treat the ECAL Cluster first
            LHCb::Calo2Track::IClusTrTable2D::InverseType::Range ecalRange = m_tableTrECAL->inverse()->relations ( track ) ;
            LHCb::Calo2Track::IClusTrTable2D::InverseType::Range hcalRange ;
            verbose()<<"tag UPF ch 2"<<endreq;
            if( m_useHCAL ) hcalRange = m_tableTrHCAL->inverse()->relations ( track ) ;
            verbose()<<"tag UPF ch 3"<<endreq;
            this->checkClustersForNeutralRecovery( theParticle , ecalRange, hcalRange );

            verbose()<<"tag UPF ch 4"<<endreq;
            return StatusCode::SUCCESS;
        }
        // Case of Charged0Momentum
        else if ( theParticle.PFType() == LHCb::PFParticle::Charged0Momentum ){
            return StatusCode::SUCCESS;
        }
        // Case of neutral particles
        else if ( ( theParticle.PFType() > LHCb::PFParticle::Neutral && theParticle.PFType() < LHCb::PFParticle::Composite )
                || theParticle.PFType()== LHCb::PFParticle::BadPhotonMatchingT ||theParticle.PFType()== LHCb::PFParticle::BadPhoton || theParticle.PFType()== LHCb::PFParticle::IsolatedPhoton ){
            verbose()<<"Neutral in updatePFCaloClusters "<<theParticle.PFType()<<endreq;
            if ( theParticle.PFType() == LHCb::PFParticle::ResolvedPi0 ){

                verbose()<<"tag UPF n 1"<<endreq;
                m_PFCaloClusters[theParticle.daughtersVector()[0]->proto()->calo()[0]->clusters()[0].target()->seed().all()]
                    ->setStatus( LHCb::PFCaloCluster::Used );
                verbose()<<"tag UPF n 2"<<endreq;
                m_PFCaloClusters[theParticle.daughtersVector()[1]->proto()->calo()[0]->clusters()[0].target()->seed().all()]
                    ->setStatus( LHCb::PFCaloCluster::Used );
                verbose()<<"tag UPF n 3"<<endreq;
                return StatusCode::SUCCESS;
            }
            //m_PFCaloClusters[theParticle.proto()->calo()[0]->clusters ()[0].target()->seed().all()]->updateParticleList(&theParticle);
            //if(theParticle.PFType() != LHCb::PFParticle::BadPhoton)
            if ( theParticle.PFType() == LHCb::PFParticle::IsolatedPhoton || theParticle.PFType() == LHCb::PFParticle::NeutralHadron){
                verbose()<<"tag UPF n 4"<<endreq;
                m_PFCaloClusters[seedID]->setStatus( LHCb::PFCaloCluster::Used );
                verbose()<<"tag UPF n 5"<<endreq;
                return StatusCode::SUCCESS;
            }

            verbose()<<"tag UPF n 6"<<endreq;
            m_PFCaloClusters[theParticle.proto()->calo()[0]->clusters ()[0].target()->seed().all()]->setStatus( LHCb::PFCaloCluster::Used );

            verbose()<<"tag UPF n 7"<<endreq;

            return StatusCode::SUCCESS;
        }
        else if ( theParticle.PFType() == LHCb::PFParticle::Unknown ){
            verbose()<<theParticle<<endreq;
            verbose()<<theParticle.proto()<<endreq;
            verbose()<<theParticle.proto()->track()<<endreq;
            Warning("Unknown type of particle in updatePFCaloClusters");
            return StatusCode::FAILURE;
        }
        Warning("No implementation for this type of particle in updatePFCaloClusters");
        return StatusCode::FAILURE;
    }

    //=============================================================================
    // Extra Track selection
    //=============================================================================
    int ParticleFlow::tagTrack( const LHCb::Track* track )
    {
        /**
         * Tag the track before further treatement
         *
         * Tag the track for different treatement depending on the category
         * First run the trackselector. If the track do not pass elementary cuts --> Tag: Reject
         * Extra requirements for long tracks with TT hits, if do not pass       --> Tag: Reject
         * If the track is a clone of a banned track ( ie composite particle )   --> Tag: Reject
         * Long or Downstream with large curvature error                         --> Tag: KeepInfMom
         * Upstream with large curvature error                                   --> Tag: TurnTo0Momentum
         * Velo track                                                            --> Tag: TurnTo0Momentum
         *
         */

        // Track selector cuts.
        if ( m_trSel!= 0 ){
            // If the track fails the "ghost"/"clone"/"min pt cuts", reject but keep the cluster.
            verbose()<<"Track "<<track->type()<<" with pt: "<<track->pt()<<" chi2 per dof "<<track->chi2PerDoF()<<endreq;
            if ( !m_trSel->accept(*track) ) return LHCb::PFParticle::Reject;
            verbose()<<"Track accepted"<<endreq;
        }
        verbose()<<"selectTrack: Check inf mom..."<<endreq;
        // Tag long and downstream tracks with large curvature errors.
        if(track->type()== LHCb::Track::Long || track->type()== LHCb::Track::Downstream){
            LHCb::State& firstS = track -> firstState () ;
            if ( ( track->type()== LHCb::Track::Long && std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal ) ||
                    ( track->type()== LHCb::Track::Downstream && std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal_Down )  ){
                return LHCb::PFParticle::KeepInfMom;
            }
        }
        // Tag upstream tracks with large curvature errors.
        else if(track->type()== LHCb::Track::Upstream){
            LHCb::State& firstS = track -> firstState () ;
            if ( std::fabs( firstS.qOverP()/sqrt(firstS.errQOverP2()) ) < m_cutInfMomTRVal_Up ){
                return LHCb::PFParticle::TurnTo0Momentum;
            }
        }
        // Velo tracks for turning them to 0 momenta.
        else if(track->type()== LHCb::Track::Velo){
            return LHCb::PFParticle::TurnTo0Momentum;
        }
        verbose()<<"selectTrack: passed all cuts"<<endreq;
        // Otherwise tag as default use.
        return LHCb::PFParticle::Keep;
    }


    //=============================================================================
    // Load the different constainers according to the configuration
    //=============================================================================

    StatusCode ParticleFlow::loadDatas() {
        /**
         * Load the input data according to the configuration.
         *
         * Load standard particels that needs corrections ( electrons, pi0s, photons).
         * Load composite particles ( D, V0s, B etc... ).
         * Load charged protoparticles ( to be used for charged hadron and neutral recovery ).
         * Load ECAL and HCAL clusters as well as the Track to cluster relation tables.
         * Load vertices.
         *
         */

        verbose()<<"Enter LoadData"<<endreq;
        // Get simple reconstructed particles ( short cut not to have the reconstructed / corrected particles)
        for (std::vector< std::string >::const_iterator i_location = m_particleLocations.begin() ;
                m_particleLocations.end() != i_location ; ++i_location ){
            verbose()<<"Look for location: "<<*i_location<<endreq;
            // Check the location exists
            if(!exist<LHCb::Particle::Range>(*i_location)){
                verbose()<<"No particle at location: "<<*i_location<<endreq;
                continue;
            }
            // Split by category Electrons,MergedPi0,ResolvedPi0,Photons,Composite,ToBan
            if ((*i_location).find("Electron")<100 && exist<LHCb::Particle::Range>(*i_location)){
                m_particleContainers["Electrons"].push_back( get<LHCb::Particle::Range>(*i_location) );
            }
            else if ((*i_location).find("Merged")<100 && exist<LHCb::Particle::Range>(*i_location)){
                m_particleContainers["MergedPi0"].push_back( get<LHCb::Particle::Range>(*i_location) );
            }
            else if ((*i_location).find("Resolved")<100 && exist<LHCb::Particle::Range>(*i_location)){
                m_particleContainers["ResolvedPi0"].push_back(  get<LHCb::Particle::Range>(*i_location) );
            }
            else if ((*i_location).find("Photon")<100 && exist<LHCb::Particle::Range>(*i_location)){
                m_particleContainers["Photons"].push_back(get<LHCb::Particle::Range>(*i_location) ) ;
            }
            else{
                return Warning("Unkown case for location: TODO");
            }
        }
        // Get the composite particles to be used instead of their componants
        for (std::vector< std::string >::const_iterator i_location = m_compositeParticleLocations.begin() ;
                m_compositeParticleLocations.end() != i_location ; ++i_location ){
            if ( exist<LHCb::Particle::Range>(*i_location) ) m_particleContainers["Composite"].push_back(get<LHCb::Particle::Range>(*i_location));
        }
        // Get the particles to be kept from jet inputs
        for (std::vector< std::string >::const_iterator i_location = m_keepCandidatesLocations.begin() ;
                m_keepCandidatesLocations.end() != i_location ; ++i_location ){
            if( exist<LHCb::Particle::Range>(*i_location) ) m_particleContainers["ToKeep"].push_back(get<LHCb::Particle::Range>(*i_location));
        }

        // Get the particles to be banned from jet inputs
        for (std::vector< std::string >::const_iterator i_location = m_banCandidatesLocations.begin() ;
                m_banCandidatesLocations.end() != i_location ; ++i_location ){
            if( exist<LHCb::Particle::Range>(*i_location) ) m_particleContainers["ToBan"].push_back(get<LHCb::Particle::Range>(*i_location));
        }

        // Get Charged ProtoParticle container
        if (! exist<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged) ){
            return Warning("No charged protopacticles at"+LHCb::ProtoParticleLocation::Charged);
        }
        else{
            m_protoParticles["Charged"] = get<LHCb::ProtoParticles*>(LHCb::ProtoParticleLocation::Charged);
        }

        // Get Velo protoparticles
        if (m_useVelo && ! exist<LHCb::ProtoParticles>("Rec/ProtoP/VeloProtoPMaker") ){
            return Warning("No charged protopacticles at Rec/ProtoP/VeloProtoPMaker");
        }
        else if ( m_useVelo ){
            m_protoParticles["Velo"] = get<LHCb::ProtoParticles*>("Rec/ProtoP/VeloProtoPMaker");
        }
        // Get ECAL Clusters container
        if(exist<LHCb::CaloClusters>( "Rec/Calo/EcalClusters" )){
            const LHCb::CaloClusters* ecalClusters = get<LHCb::CaloClusters>( "Rec/Calo/EcalClusters" ) ;
            // Fill the PFCaloCluster with it
            BOOST_FOREACH(const LHCb::CaloCluster* cluster, *ecalClusters){
                m_PFCaloClusters[cluster->seed().all()]= new LHCb::PFCaloCluster(*cluster);
            }
        }

        // Get the ECAL to track relations
        if (!exist<LHCb::Calo2Track::IClusTrTable2D> (m_clust2TrLocationECAL)) {
            return Warning("No track to ECAL cluster relation table");
        }
        m_tableTrECAL = get<LHCb::Calo2Track::IClusTrTable2D>(m_clust2TrLocationECAL);

        // If use HCAL, get the datas
        if (m_useHCAL){
            // Get HCAL Clusters container
            if(exist<LHCb::CaloClusters>( "Rec/Calo/HcalClusters" )){
                const LHCb::CaloClusters* hcalClusters = get<LHCb::CaloClusters>( "Rec/Calo/HcalClusters" ) ;
                // Fill the PFCaloCluster with it
                BOOST_FOREACH(const LHCb::CaloCluster* cluster, *hcalClusters){
                    m_PFCaloClusters[cluster->seed().all()]= new LHCb::PFCaloCluster(*cluster);
                }
            }
            // Get the HCAL to track relations
            if (!exist<LHCb::Calo2Track::IClusTrTable2D> (m_clust2TrLocationHCAL) && m_useHCAL) {
                return Warning("No track to HCAL cluster relation table");
            }
            m_tableTrHCAL = get<LHCb::Calo2Track::IClusTrTable2D>(m_clust2TrLocationHCAL);
        }
        // Load the vertices container
        if (! exist<LHCb::RecVertex::Range>(m_verticesLocation) ){
            return Warning("No Vertices at "+m_verticesLocation);
        }
        else{
            m_vertices = get<LHCb::RecVertex::Range>(m_verticesLocation);
        }
        return StatusCode::SUCCESS;
    }

    //=============================================================================
    // Preprocess the containers ( treat banned candidates and composite particles)
    //=============================================================================

    StatusCode ParticleFlow::preprocessDatas( std::vector<LHCb::PFParticle*>& PFParticles ){//, Particle2Vertex::WTable& table ) {
        /**
         * Function the preprocess the inputs (treat banned candidates and composite particles)
         *
         * Store the tracks key of banned candidates in ther m_trackKeyToBan map
         * and the actual particle in the particleToBan container,
         * sort the composite particles per vertex chi2 to avoid double counting
         * and store their tracks key in the m_trackKeyToBan map.
         *
         */
        // Treat the banned candidates
        if(  m_particleContainers.count("ToKeep") > 0.5){
            // Store them in the particleToKeep container
            LHCb::Particle::ConstVector particleToKeep;
            for( unsigned int i = 0 ; i < m_particleContainers["ToKeep"].size(); i++){
                BOOST_FOREACH(const LHCb::Particle* tokeep, m_particleContainers["ToKeep"][i] ){
                    particleToKeep.push_back(tokeep);
                }
            }
            // Store their track key in the m_trackKeyToKeep map
            BOOST_FOREACH(const LHCb::Particle* cand, particleToKeep ){
                LHCb::Particle::ConstVector particleToKeep_daug = cand->daughtersVector();
                if ( particleToKeep_daug.size() == 0 ){
                    if (cand->proto()==0)continue;
                    if (cand->proto()->track()==0)continue;
                    m_trackKeyToKeep[cand->proto()->track()->key()]=cand;
                    // IMPROVEMENT: get the calocluster and if chi2 is good put it to to NeutralRecovery with this particle attached
                    // Not done yet because assume most of the time one anyway wants it to be isolated of the jet
                }
                else{
                    for(LHCb::Particle::ConstVector::const_iterator i_p = particleToKeep_daug.begin() ;
                            particleToKeep_daug.end() != i_p ; ++i_p ){
                        if ((*i_p)->proto()==0)continue;
                        if ((*i_p)->proto()->track()==0)continue;
                        m_trackKeyToKeep[(*i_p)->proto()->track()->key()]=(*i_p);
                        // IMPROVEMENT: get the calocluster and if chi2 is good put it to to NeutralRecovery with this particle attached
                        // Not done yet because assume most of the time one anyway wants it to be isolated of the jet
                    }
                }
            }
        }

        // Treat the banned candidates
        if(  m_particleContainers.count("ToBan") > 0.5){
            // Store them in the particleToBan container
            LHCb::Particle::ConstVector particleToBan;
            for( unsigned int i = 0 ; i < m_particleContainers["ToBan"].size(); i++){
                BOOST_FOREACH(const LHCb::Particle* toban, m_particleContainers["ToBan"][i] ){
                    particleToBan.push_back(toban);
                }
            }
            // Store their track key in the m_trackKeyToBan map
            BOOST_FOREACH(const LHCb::Particle* cand, particleToBan ){
                LHCb::Particle::ConstVector particleToBan_daug = cand->daughtersVector();
                if ( particleToBan_daug.size() == 0 ){
                    if (cand->proto()==0)continue;
                    if (cand->proto()->track()==0)continue;
                    m_trackKeyToBan[cand->proto()->track()->key()]=cand->proto()->track();
                    // IMPROVEMENT: get the calocluster and if chi2 is good put it to to NeutralRecovery with this particle attached
                    // Not done yet because assume most of the time one anyway wants it to be isolated of the jet
                }
                else{
                    for(LHCb::Particle::ConstVector::const_iterator i_p = particleToBan_daug.begin() ;
                            particleToBan_daug.end() != i_p ; ++i_p ){
                        if ((*i_p)->proto()==0)continue;
                        if ((*i_p)->proto()->track()==0)continue;
                        m_trackKeyToBan[(*i_p)->proto()->track()->key()]=(*i_p)->proto()->track();
                        // IMPROVEMENT: get the calocluster and if chi2 is good put it to to NeutralRecovery with this particle attached
                        // Not done yet because assume most of the time one anyway wants it to be isolated of the jet
                    }
                }
            }
        }
        // If there is composite particles, make sure there are no clone and tracks are banned
        if(  m_particleContainers.count("Composite") > 0.5){
            //Gather the composite particles
            LHCb::Particle::ConstVector sortComposites;
            for( unsigned int i = 0 ; i < m_particleContainers["Composite"].size(); i++){
                BOOST_FOREACH(const LHCb::Particle* composit, m_particleContainers["Composite"][i] ){
                    if ( composit->particleID().abspid() == 310 || composit->particleID().abspid() == 3122 ){
                        if ( fabs( composit-> measuredMass() - 497.614 )< 25 || fabs( composit-> measuredMass() - 1115.683 )< 10 )
                            sortComposites.push_back(composit);
                    }
                    else{
                        sortComposites.push_back(composit);
                    }
                }
            }

            // Sort the composite particles by vertex chi2
            std::sort( sortComposites.begin(), sortComposites.end(), sortChi2() );
            BOOST_FOREACH(const LHCb::Particle* comp, sortComposites){
                // LHCb::Particle::ConstVector daughters = comp->daughtersVector();
                LHCb::Particle::ConstVector daughters;
                int motherID = LoKi::Cuts::ID(comp);
                LoKi::Extract::getParticles( comp , std::back_inserter ( daughters ) ,  LoKi::Cuts::ID!=motherID && LoKi::Cuts::HASPROTO ) ;
                bool overlap = false;
                // Loop over the daughters and check if they overlapp
                for (LHCb::Particle::ConstVector::const_iterator i_p = daughters.begin();daughters.end()!=i_p;++i_p){
                    //IMPROVEMENT: the case where the composite particle has neutral or composite daugther
                    if ((*i_p)->proto()==NULL) return Error("Composite daugthers of composite particles treatement is not implemented");
                    if ((*i_p)->proto()->track()==NULL) Error("Neutral daugthers of composite particles treatement is not implemented");
                    // Check if the track is banned already
                   if (m_trackKeyToKeep.count((*i_p)->proto()->track()->key())>0.5){
                        overlap = true;
                        break;
                    }
                 if (m_trackKeyToBan.count((*i_p)->proto()->track()->key())>0.5){
                        overlap = true;
                        break;
                    }
                }
                if (!overlap){
                    LHCb::PFParticle* theParticle = new LHCb::PFParticle( *comp );
                    //this->relate2Vertex( theParticle , table );
                    PFParticles.push_back( theParticle );
                    // Ban the unbanned
                    for (LHCb::Particle::ConstVector::const_iterator i_p = daughters.begin();daughters.end()!=i_p;++i_p){
                        if ((*i_p)->proto()==NULL) continue;
                        if ((*i_p)->proto()->track()==NULL) continue;
                        m_trackKeyToBan[(*i_p)->proto()->track()->key()]=(*i_p)->proto()->track();
                        LHCb::PFParticle* theDaugtherParticle = new LHCb::PFParticle( *(*i_p) , true );
                        this->updatePFCaloClusters(*theDaugtherParticle);
                        PFParticles.push_back( theDaugtherParticle );
                    }
                }
            }
        }
        return StatusCode::SUCCESS ;
    }

    //=============================================================================
    // Add the PF particle to vertex relation in the main relation table
    //=============================================================================
    void ParticleFlow::relate2Vertex( const LHCb::Particle* p
            , Particle2Vertex::WTable& table)
    {
        /**
         * Add the particle to vertex relation in the main relation table
         *
         */

        // Create the table for this single particle
        const Particle2Vertex::LightWTable bestPVTable =
            m_pvRelator->relatedPVs(p,
                    LHCb::VertexBase::ConstVector(m_vertices.begin(),
                        m_vertices.end()));
        const Particle2Vertex::LightWTable::Range range = bestPVTable.relations();
        // Merge it to the main table
        table.merge(range);
    }


    //=============================================================================
    // Treat and save the charged protoparticles
    //=============================================================================
    StatusCode ParticleFlow::treatProtoParticles( std::string pptype , std::vector<LHCb::PFParticle*>& PFParticles ){// , Particle2Vertex::WTable& table){
        /**
         * Function treating the charged protparticles
         *
         * Loop over charged Protoparticles, reject bad tracks, make best PID hypo particle out of it
         * store the used ECAL and HCAL clusters
         *
         */
        verbose()<<"In treat protoparticles"<<endreq;
        BOOST_FOREACH(const LHCb::ProtoParticle* ch_pp, *m_protoParticles[pptype] ){


            //custom ghost removal
            const LHCb::Track* track = ch_pp->track();

            int tag(LHCb::PFParticle::Unknown);
            const LHCb::ParticleProperty* pprop = NULL ;
            // Continue either if the track does not exist or if it has been banned already
            if ( track == NULL || m_trackKeyToBan.count(track->key()) ) continue;
            if(!(m_trackKeyToKeep.count(track->key()))){

            if(pptype == "Charged"){
                double nNN = ch_pp->info(LHCb::ProtoParticle::ProbNNghost,-1);

                if(track->type()== LHCb::Track::Long){
                    if(nNN > m_LongProbNNghostCut) continue;
                }

                if(track->type()== LHCb::Track::Downstream){
                    if(nNN > m_DownProbNNghostCut) continue;
                }

                if(track->type()== LHCb::Track::Upstream){
                    if(nNN > m_UpProbNNghostCut) continue;
                }
            }



            // Extra track check
            tag = tagTrack(track);
            }else{
            tag = LHCb::PFParticle::Keep;


            pprop = m_ppSvc->find((m_trackKeyToKeep[track->key()])->particleID());

            }

            if( tag == LHCb::PFParticle::Reject) continue;
            // Ban the track from further use (not needed just safety if later addition of code)
            m_trackKeyToBan[track->key()] = track ;
            // Create the particles
            LHCb::PFParticle*theParticle = new LHCb::PFParticle( ch_pp  , tag ,  m_protoMap, pprop );
            if ( theParticle == NULL ) delete(theParticle);
            else{
                verbose()<<"In treat protoparticles "<<theParticle->particleID()<<endreq;
                this->updatePFCaloClusters( *theParticle );
                // Turns the PFParticle into Particle
                //this->relate2Vertex( theParticle , table );
                PFParticles.push_back( theParticle );
            }
        }
        return StatusCode::SUCCESS;
    }

    //=============================================================================
    // Treat and save the Pi0s
    //=============================================================================
    StatusCode ParticleFlow::treatPi0s ( int pftype , std::vector<LHCb::PFParticle*>& PFParticles){
        /**
         * Function treating the pi0
         *
         * Loop over the pi0, depending if Merged or Resolved,
         * check the calo cluster Photon ID and banned status
         *
         */

        // Get the container
        std::string typeCont = "ResolvedPi0";
        verbose()<<pftype<<" "<<LHCb::PFParticle::MergedPi0<<endreq;
        if (pftype == LHCb::PFParticle::MergedPi0 ){
            verbose()<<"testbef "<<pftype<<endreq;
            typeCont = "MergedPi0";
            verbose()<<"testaft "<<pftype<<endreq;
        }
        verbose()<<"In treat pi0 a "<<typeCont<<endreq;
        // verbose()<<m_particleContainers<<endreq;
        // not possible to "cout" it
        LHCb::Particle::ConstVector realPart ;

        BOOST_FOREACH(const LHCb::Particle* pi0, m_particleContainers[typeCont][0] ){
            verbose()<<"Pi0 of type "<<typeCont<<" and pt"<<pi0->pt()<<endreq;
            std::vector< const LHCb::CaloCluster* > caloClusters;
            // Check if they pass photon ID cut for resolved pi0
            if ( pftype == LHCb::PFParticle::ResolvedPi0 ) {
                LHCb::Particle::ConstVector daugs = pi0->daughtersVector ();
                double minPhotonID = std::min(daugs[0]->proto()->info( LHCb::ProtoParticle::PhotonID,-100.),
                        daugs[1]->proto()->info( LHCb::ProtoParticle::PhotonID,-100.));
                double maxPhotonID = std::max(daugs[0]->proto()->info( LHCb::ProtoParticle::PhotonID,-100.),
                        daugs[1]->proto()->info( LHCb::ProtoParticle::PhotonID,-100.));
                if ( minPhotonID < m_photonIDMin4ResolvedPi0 || maxPhotonID < m_photonIDMax4ResolvedPi0  ) continue ;
                // Loop over daughters
                //realPart = pi0->daughtersVector ();
                for  (LHCb::Particle::ConstVector::const_iterator i_daug = daugs. begin()  ;
                        daugs.end() != i_daug ; ++i_daug ){
                    realPart.push_back(*i_daug);
                }
            }
            else if ( pftype == LHCb::PFParticle::MergedPi0){
                realPart.push_back(pi0);
            }
            verbose()<<"N real part "<<realPart.size()<<endreq;
            // Now check the banning status
            int goodCluster = 0;
            int nSatCell(0);
            for  (LHCb::Particle::ConstVector::const_iterator i_daug = realPart. begin()  ;
                    realPart.end() != i_daug ; ++i_daug ){
                const LHCb::ProtoParticle* n_pp = (*i_daug)->proto();
                if (n_pp == NULL){ continue; }
                // ban the corresponding cluster
                const SmartRefVector< LHCb::CaloHypo > &  caloHypos = n_pp -> calo();
                const SmartRefVector< LHCb::CaloCluster > & hypoClusters =  caloHypos[0]->clusters ();
                if ( !hypoClusters[0] ){  break; }
                // If the cluster have already been tagged, skip.
                if (m_PFCaloClusters[hypoClusters[0].target()->seed().all()]->status() != LHCb::PFCaloCluster::Available ) break;
                ++goodCluster;
                nSatCell+= this->numberOfSaturatedCells( hypoClusters[0].target() );
            }
            verbose()<<"N good cluster "<<goodCluster<<endreq;

            // If less than 2 clusters are left (or 1 for merged ), skip this candidate
            if ( (goodCluster<2 && pftype == LHCb::PFParticle::ResolvedPi0) ||
                    (goodCluster<1 && pftype == LHCb::PFParticle::MergedPi0) ) continue;
            // Clone the original particle

            LHCb::PFParticle* theParticle = new LHCb::PFParticle(*pi0,false, pftype);
            this->updatePFCaloClusters( *theParticle );
            theParticle->setNSaturatedCellsECAL( nSatCell );
            PFParticles.push_back(theParticle);
        }
        return StatusCode::SUCCESS;
    }

    //=============================================================================
    // Treat and save the Photons
    //=============================================================================
    StatusCode ParticleFlow::treatPhotons ( std::vector<LHCb::PFParticle*>& PFParticles){
        /**
         * Function treating the photons
         *
         * Loop over the photons, skipping those with already banned cluster,
         * Check for T banning use and in case it is used, check if a T track points to it.
         * If yes the photon ID requirement is tighter, if not
         *
         */
        verbose()<<"In treat photon "<<endreq;
        BOOST_FOREACH(const LHCb::Particle* photon, m_particleContainers["Photons"][0]){
            // Get the protoparticle information
            const LHCb::ProtoParticle* n_pp = photon->proto();
            if (n_pp == NULL)  continue;
            const SmartRefVector< LHCb::CaloHypo > &  caloHypos = n_pp -> calo();
            const SmartRefVector< LHCb::CaloCluster > & hypoClusters =  caloHypos[0]->clusters ();
            // If the cluster is already tagged... then skip it
            if ( ! hypoClusters[0] || m_PFCaloClusters[hypoClusters[0].target()->seed().all()]->status() != LHCb::PFCaloCluster::Available  ) continue;
            // TTrack banning, if the photon match a TTrack, being a real photon requires better photonID
            if ( m_banFromTTrack ){
                LHCb::Calo2Track::IClusTrTable2D::DirectType::Range tRange = m_tableTrECAL->relations( hypoClusters[0].target() );
                if (tRange.size() > 0 ){
                    if (tRange.front().to()->type() == LHCb::Track::Ttrack && tRange.front().weight()< m_Chi2ECALCutTT ){
                        // Check the PID
                        int PFType = LHCb::PFParticle::Unknown;
                        if ( n_pp ->info( LHCb::ProtoParticle::PhotonID,-100.)< m_photonID4PhotonTban
                                || n_pp ->info( LHCb::ProtoParticle::CaloNeutralSpd,-100.) > 1 ){
                            //Bad photon ID
                            PFType = LHCb::PFParticle::BadPhotonMatchingT;
                        }
                        else{
                            //Good photon ID
                            PFType = LHCb::PFParticle::Photon;
                        }
                        // Clone the original particle
                        LHCb::PFParticle* theParticle = new LHCb::PFParticle(*photon);
                        theParticle->setPFType(PFType);
                        int nSatCell = this->numberOfSaturatedCells( hypoClusters[0].target() );
                        // Update the cluster
                        this->updatePFCaloClusters( *theParticle );
                        theParticle->setNSaturatedCellsECAL( nSatCell );
                        // Save the particle
                        PFParticles.push_back(theParticle);
                        continue ;
                    }
                }
            }
            // Photon ID banning
            int PFType(LHCb::PFParticle::Photon);
            if( n_pp ->info( LHCb::ProtoParticle::PhotonID,-100.)< m_photonID4Photon ) PFType = LHCb::PFParticle::BadPhoton;
            // Clone the original particle
            if( photon->pt() < m_minPhotonEt && PFType == LHCb::PFParticle::Photon  ) continue;
            if( photon->pt() < m_minBadPhotonEt && PFType == LHCb::PFParticle::BadPhoton ) continue;
            if( photon->pt() < m_minBadPhotonMatchingTEt && PFType == LHCb::PFParticle::BadPhotonMatchingT ) continue;
            LHCb::PFParticle* theParticle = new LHCb::PFParticle(*photon);
            int nSatCell = this->numberOfSaturatedCells( hypoClusters[0].target() );
            theParticle->setNSaturatedCellsECAL( nSatCell );
            theParticle->setPFType(PFType);
            verbose()<<"Type in photons: "<<PFType<<" "<<theParticle->PFType()<<" "<<theParticle->info(900,-1000.)<<endreq;
            this->updatePFCaloClusters( *theParticle );
            // Save the particle
            PFParticles.push_back(theParticle);
        }

        return StatusCode::SUCCESS;
    }

    //StatusCode ParticleFlow::treatNeutralHadronsAndIsolatedPhotons( std::vector<LHCb::PFParticle*>& PFParticles , LHCb::CaloHypos& PFCaloHypos, LHCb::ProtoParticles& PFProtoParticles ){
    StatusCode ParticleFlow::treatNeutralHadronsAndIsolatedPhotons( std::vector<LHCb::PFParticle*>& PFParticles ){
        /**
         * Function to treat the HCAL clusters not matching to any track.
         *
         * TODO
         *
         */
        verbose()<<"In treat neutRAL HADRON "<<endreq;
        for (std::map< unsigned int ,  LHCb::PFCaloCluster* >::iterator iclu = m_PFCaloClusters.begin(); m_PFCaloClusters.end() != iclu ; ++iclu){
            if ( (*iclu).second->seed().calo() != CaloCellCode::HcalCalo ) {
                if ( (*iclu).second->status() != LHCb::PFCaloCluster::Available ) continue;

                verbose()<<"tag aa"<<endreq;
                const LHCb::CaloCluster::Position clupos = (*iclu).second->position();
                double x = clupos.x() , y = clupos.y() , z = clupos.z();
                double px = (*iclu).second->e()*x/sqrt(x*x + y*y + z*z);
                double py = (*iclu).second->e()*y/sqrt(x*x + y*y + z*z);
                double pz = (*iclu).second->e()*z/sqrt(x*x + y*y + z*z);
                Gaudi::XYZTVector momentum = Gaudi::XYZTVector(px,py,pz,(*iclu).second->e());
                if(momentum.Pt()<m_minIsoPhotonEt){
                    (*iclu).second->setStatus( LHCb::PFCaloCluster::Used );
                    continue;
                }
                // Make the CaloHypo
                /*
                   LHCb::CaloHypo* hypo = new LHCb::CaloHypo();
                   hypo->addToClusters((*iclu).second );
                   hypo->setHypothesis ( LHCb::CaloHypo::Photon );
                   for ( unsigned ii = 0; ii<(*iclu).second->entries().size() ; ++ii )
                   hypo->addToDigits((*iclu).second->entries().at(ii).digit());
                   PFCaloHypos.insert(hypo);
                // Make the Protoparticle
                LHCb::ProtoParticle* NewProto = new LHCb::ProtoParticle();
                NewProto->addToCalo(hypo);
                // Make a new Particle
                PFProtoParticles.insert(NewProto);*/
                verbose()<<"tag a"<<endreq;
                unsigned int cellID = (*iclu).second->seed().all();

                verbose()<<"tag a bis"<<endreq;

                LHCb::PFParticle* theParticle = new LHCb::PFParticle( );
                //theParticle->setProto(NULL);
                verbose()<<"tag b"<<endreq;
                theParticle->setParticleID( LHCb::ParticleID(22) );
                verbose()<<"tag c"<<endreq;
                theParticle->setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
                verbose()<<"tag d"<<endreq;
                theParticle->setMomentum( Gaudi::XYZTVector(px,py,pz,(*iclu).second->e()) ) ;
                verbose()<<"tag e"<<endreq;
                theParticle->setPFType(LHCb::PFParticle::IsolatedPhoton);
                verbose()<<"tag f"<<endreq;
                // Update the caloClusters container
                this->updatePFCaloClusters( *theParticle , cellID );
                verbose()<<"tag i"<<endreq;
                int nSatCell = this->numberOfSaturatedCells( (*iclu).second );
                verbose()<<"tag j"<<endreq;
                theParticle->setNSaturatedCellsECAL( nSatCell );
                verbose()<<"tag k"<<endreq;
                PFParticles.push_back(theParticle);
                verbose()<<"tag l"<<endreq;
            }
            else{

                // Create
                verbose()<<"tagX aa"<<endreq;
                const LHCb::CaloCluster::Position clupos = (*iclu).second->position();
                double x = clupos.x() , y = clupos.y() , z = clupos.z();
                double px = (*iclu).second->e()*x/sqrt(x*x + y*y + z*z);
                double py = (*iclu).second->e()*y/sqrt(x*x + y*y + z*z);
                double pz = (*iclu).second->e()*z/sqrt(x*x + y*y + z*z);
                Gaudi::XYZTVector momentum = Gaudi::XYZTVector(px,py,pz,(*iclu).second->e());
                //Check minimal cluster energy
                if ( momentum.Pt() < m_minHCALEt || momentum.E() < m_minHCALE ) {
                    // Just for test
                    (*iclu).second->setStatus( LHCb::PFCaloCluster::Used );
                    continue;
                };
                if ( momentum.Pt() < m_minHCALEt4Eta4 && momentum.Eta() > 4 ) {
                    // Just for test
                    (*iclu).second->setStatus( LHCb::PFCaloCluster::Used );
                    continue;
                };
                // Sanity check
                if ( (*iclu).second->status() != LHCb::PFCaloCluster::Available || momentum.eta() > 4.2 ){
                    if ((*iclu).second->status() == LHCb::PFCaloCluster::Available && momentum.eta() > 4.2 ){
                        (*iclu).second->setStatus( LHCb::PFCaloCluster::Used );
                    }
                    continue;
                }

                // Make the CaloHypo
                /*LHCb::CaloHypo* hypo = new LHCb::CaloHypo();
                  hypo->addToClusters((*iclu).second );
                  hypo->setHypothesis ( LHCb::CaloHypo::NeutralHadron );
                  for ( unsigned ii = 0; ii<(*iclu).second->entries().size() ; ++ii )
                  hypo->addToDigits((*iclu).second->entries().at(ii).digit());
                  PFCaloHypos.insert(hypo);
                // Make the Protoparticle
                LHCb::ProtoParticle* NewProto = new LHCb::ProtoParticle();
                NewProto->addToCalo(hypo);
                // Make a new Particle
                verbose()<<"tagX aa"<<endreq;
                verbose()<<"tagX aa"<<endreq;
                PFProtoParticles.insert(NewProto);*/
                verbose()<<"tagX a"<<endreq;
                LHCb::PFParticle* theParticle = new LHCb::PFParticle( );

                verbose()<<"tagX a bis"<<endreq;
                unsigned int cellID = (*iclu).second->seed().all();

                //theParticle->setProto(NewProto);
                verbose()<<"tagX b"<<endreq;
                theParticle->setParticleID( LHCb::ParticleID(22) );
                verbose()<<"tagX c"<<endreq;
                theParticle->setReferencePoint( Gaudi::XYZPoint(0.,0.,0.)  );
                verbose()<<"tagX d"<<endreq;
                theParticle->setMomentum( Gaudi::XYZTVector(px,py,pz,(*iclu).second->e()) ) ;
                verbose()<<"tagX e"<<endreq;
                theParticle->setPFType(LHCb::PFParticle::NeutralHadron);
                // Update the caloClusters container
                verbose()<<"tagX f"<<endreq;
                this->updatePFCaloClusters( *theParticle , cellID);
                verbose()<<"tagX g"<<endreq;
                int nSatCell = this->numberOfSaturatedCells( (*iclu).second );
                verbose()<<"tagX h"<<endreq;
                theParticle->setNSaturatedCellsHCAL( nSatCell );
                verbose()<<"tagX i"<<endreq;
                PFParticles.push_back(theParticle);
                verbose()<<"tagX j"<<endreq;
            }
        }
        return StatusCode::SUCCESS;
    }

    //=============================================================================
    // Treat the remaining clusters and save them as particles
    //=============================================================================

    StatusCode ParticleFlow::treatNeutralRecovery( std::vector<LHCb::PFParticle*>& PFParticles ){
        /**
         * Function to do the neutral recovery
         *
         * TODO
         *
         */
        verbose()<<"In neutralreco "<<endreq;
        std::map<  const LHCb::PFParticle* , bool > ParticlesStatus;
        // Loop over the CaloCluster to make super clusters
        for (std::map< unsigned int ,  LHCb::PFCaloCluster* >::const_iterator i_pfClu = m_PFCaloClusters.begin();
                m_PFCaloClusters.end() != i_pfClu ; ++i_pfClu){
            // Check the cluster is usable
            if ( (*i_pfClu).second->status() != LHCb::PFCaloCluster::AvailableForNeutralRecovery ) continue;
            verbose()<<"   Main available cluster "<<(*i_pfClu).second->seed().all()<<" "<<(((*i_pfClu).second)->particleList()).size()<<" "<<(((*i_pfClu).second)->particleList()).front()<<endreq;
            // Vector to store correlated clusters and particles correponding to it.
            std::vector < LHCb::PFCaloCluster* > SuperClusters;
            std::vector < const LHCb::PFParticle*  > SuperParticles;
            // Check the cluster type
            int CaloType = ((*i_pfClu).second->seed ().calo() == CaloCellCode::HcalCalo) ? LHCb::PFParticle::ClustHCAL : LHCb::PFParticle::ClustECAL ;
            // To keep track of the iteration, store the iterator at each level of the hierarchy, the last particle of the level, and the corresponding cluster
            std::vector< std::vector< const LHCb::PFParticle* > > partLists_level ;
            std::vector< int > iterator_level;
            std::vector< int > end_level;
            std::vector< LHCb::PFCaloCluster* > originCalo_level;
            // The current particle
            int it(0);
            // Initialize
            int level(0);
            std::vector< int > previous_level;
            originCalo_level.push_back((*i_pfClu).second);
            previous_level.push_back(-1);
            partLists_level.push_back(((*i_pfClu).second)->particleList());
            iterator_level.push_back(0);
            end_level.push_back(partLists_level[level].size()-1);
            bool exit = false;
            // Condition to exit the loop
            while ( ! exit ){
                // Check if the particle is registered already
                if ( ParticlesStatus.find(partLists_level[level][it]) != ParticlesStatus.end() ){
                    // In this case iterate
                    continue;
                }
                else{
                    // Update the iterator level value
                    iterator_level[level] = it;
                    // Add to the list of particles and cluster
                    SuperParticles.push_back(partLists_level[level][it]);
                    ParticlesStatus[partLists_level[level][it]] = false;
                    if(originCalo_level[level]->status() == LHCb::PFCaloCluster::AvailableForNeutralRecovery){
                        SuperClusters.push_back(originCalo_level[level]);
                        originCalo_level[level]->setStatus( LHCb::PFCaloCluster::Used);
                    }
                }
                // Now iterate
                bool iterated = false;
                while( ! iterated ){
                    // LEVEL UP If next level calo exists and it has not yet been treated
                    CaloType = (originCalo_level[level]->seed ().calo() == CaloCellCode::HcalCalo) ? LHCb::PFParticle::ClustHCAL : LHCb::PFParticle::ClustECAL ;
                    CaloType = (CaloType-LHCb::PFParticle::ClustECAL + 1)%2 + LHCb::PFParticle::ClustECAL ;
                    if( partLists_level[level][it]->info(CaloType,-100.)>-1 &&  m_PFCaloClusters.find(partLists_level[level][it]->info(CaloType,-100.))!=  m_PFCaloClusters.end()
                            && m_PFCaloClusters[partLists_level[level][it]->info(CaloType,-100.)]->status() == LHCb::PFCaloCluster::AvailableForNeutralRecovery ){
                        //level up
                        LHCb::PFCaloCluster* theClu = m_PFCaloClusters[partLists_level[level][it]->info(CaloType,-100)];
                        originCalo_level.push_back(theClu);
                        previous_level.push_back(level);
                        level = previous_level.size()-1;
                        iterator_level.push_back(0);
                        it = iterator_level[level];
                        end_level.push_back(theClu->particleList().size()-1);
                        partLists_level.push_back(theClu->particleList());
                        // save this new one
                        if (ParticlesStatus.find(partLists_level[level][it])==ParticlesStatus.end()){
                            SuperParticles.push_back(partLists_level[level][it]);
                            ParticlesStatus[partLists_level[level][it]] = false;
                        }
                        SuperClusters.push_back(originCalo_level[level]);
                        originCalo_level[level]->setStatus( LHCb::PFCaloCluster::Used);
                        continue;
                    }
                    // Go to the next untreated particle of this level (ie check if this one is treated already)

                    if ( ParticlesStatus.find(partLists_level[level][it]) != ParticlesStatus.end() ){
                        // if it is not already the end, go to next one and update the iterator_level
                        if ( it != end_level[level] ){
                            it++;
                            iterator_level[level]=it;
                        }
                        // if is the end then go down in level
                        else{
                            if ( previous_level[level] == -1  ) {
                                // this is the end, beautifull friend.
                                iterated = true; exit = true;
                            }
                            else{
                                level = previous_level[level];
                                it = iterator_level[level];
                            }
                        }
                    }
                    // the given particle is not yet treated goes on
                    else{
                        iterated = true;
                    }
                }
            }
            verbose()<<"Super cluster size: "<<SuperClusters.size()<<" Number of particles associated to it: "<<SuperParticles.size()<<endreq;
            this->createHiddenNeutral(SuperClusters,SuperParticles,PFParticles);
        }
        return StatusCode::SUCCESS;
    }

    StatusCode ParticleFlow::createHiddenNeutral( const std::vector<LHCb::PFCaloCluster*>& clusters,
            const std::vector<const LHCb::PFParticle*>& particles,
            std::vector<LHCb::PFParticle*>& PFParticles )
    {
        // Loop over the particles. For each one get the most probable response as function of the associated cluster and its direction
        // Get the direction and Energy of each cluster
        // Do the barycenter: di.Ei.Xi/di.Ei with di=1 for cluster and di=-1 for particles
        Gaudi::Vector6 Barycenter(0.,0.,0.,0.,0.,0.);
        int nSatCellHCAL(0),nSatCellECAL(0);
        for (std::vector < LHCb::PFCaloCluster* >::const_iterator i_c = clusters.begin() ; clusters.end() != i_c ; ++i_c ){
            Barycenter[0] += (*i_c)->e()*(*i_c)->position().x ();
            Barycenter[1] += (*i_c)->e()*(*i_c)->position().y ();
            Barycenter[2] += (*i_c)->e()*(*i_c)->position().z ();
            Barycenter[3] += (*i_c)->e()*(*i_c)->position().x ()/(*i_c)->position().z ();
            Barycenter[4] += (*i_c)->e()*(*i_c)->position().y ()/(*i_c)->position().z ();
            Barycenter[5] += (*i_c)->e();
            if ( (*i_c)->seed ().calo() == CaloCellCode::HcalCalo) nSatCellHCAL += this->numberOfSaturatedCells( *i_c );
            else  nSatCellECAL += this->numberOfSaturatedCells( *i_c );
        }
        double oldEnergy(Barycenter[5]);
        for (std::vector < const LHCb::PFParticle*  >::const_iterator i_p = particles.begin() ; particles.end() != i_p ; ++i_p ){
            const LHCb::PFParticle* p = *i_p;
            Gaudi::Vector6 expectedChargedContrib = this->expectedEnergy(p);
            if (expectedChargedContrib[5]<1e-6)continue;
            Barycenter -= expectedChargedContrib;
        }
        // change (*i_pfClu).second by the barycenter....
        Barycenter[0]/=Barycenter[5];
        Barycenter[1]/=Barycenter[5];
        Barycenter[2]/=Barycenter[5];
        Barycenter[3]/=Barycenter[5];
        Barycenter[4]/=Barycenter[5];
        if (Barycenter[5]>m_MinE){
            double pt = abs(sin(atan(sqrt(Barycenter[0]*Barycenter[0]+Barycenter[1]*Barycenter[1])/Barycenter[2]))*Barycenter[5]);
            if (pt>m_MinEt){
                LHCb::PFParticle* theParticle = new LHCb::PFParticle( Barycenter , oldEnergy);
                theParticle->setNSaturatedCellsHCAL( nSatCellHCAL );
                theParticle->setNSaturatedCellsECAL( nSatCellECAL );
                PFParticles.push_back( theParticle );
            }
        }
        return StatusCode::SUCCESS;
    }

    Gaudi::Vector6 ParticleFlow::expectedEnergy(const LHCb::PFParticle* p ){

        // Check somewhere that it is even compatible... otherwise don't substract (if not cluster good associated...)
        Gaudi::Vector6 Barycenter(0.,0.,0.,0.,0.,0.);
        // Put the positon of the clusters there.
        bool hasECAL(false),hasHCAL(false);
        if (p->info(LHCb::PFParticle::ClustECAL,-100.)>-1.){
            hasECAL=true;
            //get the position
            Barycenter[0]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustECAL,-100.)]->position().x();
            Barycenter[1]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustECAL,-100.)]->position().y();
            Barycenter[2]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustECAL,-100.)]->position().z();
            Barycenter[3]+=Barycenter[0]/Barycenter[2];
            Barycenter[4]+=Barycenter[1]/Barycenter[2];
            Barycenter[5]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustECAL,-100.)]->e();
        }
        if (p->info(LHCb::PFParticle::ClustHCAL,-100.)>-1.){
            hasHCAL=true;
            //get the position
            Barycenter[0]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustHCAL,-100.)]->position().x();
            Barycenter[1]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustHCAL,-100.)]->position().y();
            Barycenter[2]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustHCAL,-100.)]->position().z();
            Barycenter[3]+=Barycenter[0]/Barycenter[2];
            Barycenter[4]+=Barycenter[1]/Barycenter[2];
            Barycenter[5]+=m_PFCaloClusters[p->info(LHCb::PFParticle::ClustHCAL,-100.)]->e();
        }
        if ( hasHCAL && hasECAL ){
            Barycenter[0]/=2;
            Barycenter[1]/=2;
            Barycenter[2]/=2;
            Barycenter[3]/=2;
            Barycenter[4]/=2;
        }

        double momentum = p->p();
        double expectedE = 0.;
        int theType = (int)p->PFType();
        if( theType == LHCb::PFParticle::Daughter )theType = (int)p->PFDaugType();
        if ( theType == LHCb::PFParticle::Muon ){
            //Eventually remove a mip... nothing for the moment
            verbose()<<"This is a Muon don't remove anything for the moment"<<endreq;
        }
        else if ( theType == LHCb::PFParticle::Electron ){
            if ( hasHCAL && hasECAL ){
                if( m_MC ) expectedE = m_ECALElectron_MC->Eval(momentum) * momentum;
                else  expectedE = m_ECALElectron_Data->Eval(momentum) * momentum;
            }
            else if ( hasHCAL && !hasECAL ){
                if( m_MC ) expectedE = m_ECALElectron_MC->Eval(momentum) * momentum;
                else  expectedE = m_ECALElectron_Data->Eval(momentum) * momentum;
            }
            else if ( !hasHCAL && hasECAL ){
                expectedE = momentum ;
                if( m_MC ) expectedE = m_ECALElectron_MC->Eval(momentum) * momentum;
                else  expectedE = m_ECALElectron_Data->Eval(momentum) * momentum;
            }
        }
        else if ( theType == LHCb::PFParticle::ChargedHadron || theType == LHCb::PFParticle::ChargedInfMomentum ) {
            if ( hasHCAL && hasECAL ){
                if( m_MC ) expectedE = m_ECALHCALHadron_MC->Eval(momentum) * momentum;
                else  expectedE = m_ECALHCALHadron_Data->Eval(momentum) * momentum;
                //Put the parametrisation
            }
            else if ( hasHCAL && !hasECAL ){
                if( m_MC ) expectedE = m_HCALHadron_MC->Eval(momentum) * momentum;
                else  expectedE = m_HCALHadron_Data->Eval(momentum) * momentum;
                //Put the parametrisation
            }
            else if ( !hasHCAL && hasECAL ){
                if( m_MC ) expectedE = m_ECALHadron_MC->Eval(momentum) * momentum;
                else  expectedE = m_ECALHadron_Data->Eval(momentum) * momentum;
                //Put the parametrisation
            }
        }
        //else if( theType == LHCb::PFParticle::ChargedInfMomentum ){
        //  verbose()<<"This is a infintie momenta staff"<<endreq;
        // TODO... treat if (removing the E + 2 sigma???)
        //}
        else{
            verbose()<<"Track type: "<<theType<<endreq;
            Warning("Undefined case in neutral recovery "); // Might be a neutral particle....
        }
        ///// RECHECK this one....
        if( expectedE> m_maxfractionofE*Barycenter[5] ){
            Barycenter[0]=0.;
            Barycenter[1]=0.;
            Barycenter[2]=0.;
            Barycenter[3]=0.;
            Barycenter[4]=0.;
            Barycenter[5]=0.;
        }
        else{
            Barycenter[0]*=expectedE;
            Barycenter[1]*=expectedE;
            Barycenter[2]*=expectedE;
            Barycenter[3]*=expectedE;
            Barycenter[4]*=expectedE;
            Barycenter[5] = expectedE;
        }
        return Barycenter;
    }


    StatusCode ParticleFlow::checkClustersForNeutralRecovery( LHCb::PFParticle& theParticle ,
            const LHCb::Calo2Track::IClusTrTable2D::InverseType::Range& ecalAssoc,
            const LHCb::Calo2Track::IClusTrTable2D::InverseType::Range& hcalAssoc )
    {
        if( m_onlyBest ){
            // Should it be banned? Compare the chi2 of the track<->cluster 2D association (x,y projection) to settings
            if( ecalAssoc.size()>0){
                if( ecalAssoc.front().weight() < m_Chi2ECALCut ) {
                    m_PFCaloClusters[ecalAssoc.front().to()->seed().all()]->updateParticleList(&theParticle);
                    theParticle.addInfo(LHCb::PFParticle::ClustECAL,ecalAssoc.front().to()->seed().all());
                    m_PFCaloClusters[ecalAssoc.front().to()->seed().all()]->setStatus( LHCb::PFCaloCluster::AvailableForNeutralRecovery );
                    // In the case of non composite particles (ie itr is theParticle)
                    if ( theParticle.PFType() == LHCb::PFParticle::ChargedInfMomentum  && m_doNotUseInfMominNR ){
                        m_PFCaloClusters[ecalAssoc.front().to()->seed().all()]->setStatus( LHCb::PFCaloCluster::ContainsInfMomentum );
                    }
                }
            }
            // Now if needed treat HCAL
            if( m_useHCAL ){
                if( hcalAssoc.size()>0){
                    // Should it be banned? Compare the chi2 of the track<->cluster 2D association (x,y projection) to settings
                    if(    ( hcalAssoc.front().weight() < m_Chi2HCAL0Cut  && hcalAssoc.front().to()->e() <= m_Chi2HCAL0CutEValue )
                            || ( hcalAssoc.front().weight() <  m_Chi2HCAL1Cut && hcalAssoc.front().to()->e() > m_Chi2HCAL0CutEValue
                                && hcalAssoc.front().to()->e() <= m_Chi2HCAL1CutEValue )
                            || ( hcalAssoc.front().weight() < m_Chi2HCAL2Cut  && hcalAssoc.front().to()->e() > m_Chi2HCAL1CutEValue ) ) {
                        m_PFCaloClusters[hcalAssoc.front().to()->seed().all()]->updateParticleList(&theParticle);
                        theParticle.addInfo(LHCb::PFParticle::ClustHCAL,hcalAssoc.front().to()->seed().all());
                        m_PFCaloClusters[hcalAssoc.front().to()->seed().all()]->setStatus( LHCb::PFCaloCluster::AvailableForNeutralRecovery );
                        // In the case of non composite particles (ie itr is theParticle)
                        if ( theParticle.PFType() == LHCb::PFParticle::ChargedInfMomentum && m_doNotUseInfMominNR){
                            m_PFCaloClusters[hcalAssoc.front().to()->seed().all()]->setStatus( LHCb::PFCaloCluster::ContainsInfMomentum );
                        }
                    }
                }
            }
        }
        else{
            // Should it be banned? Compare the chi2 of the track<->cluster 2D association (x,y projection) to settings
            if( ecalAssoc.size()>0){
                for (LHCb::Calo2Track::IClusTrTable2D::InverseType::Range::const_iterator ic = ecalAssoc.begin();
                        ecalAssoc.end()!= ic; ++ic){
                    if( (*ic).weight() < m_Chi2ECALCut ) {
                        m_PFCaloClusters[(*ic).to()->seed().all()]->updateParticleList(&theParticle);
                        theParticle.addInfo(LHCb::PFParticle::ClustECAL,(*ic).to()->seed().all());
                        m_PFCaloClusters[(*ic).to()->seed().all()]->setStatus( LHCb::PFCaloCluster::AvailableForNeutralRecovery );
                        // In the case of non composite particles (ie itr is theParticle)
                        if ( theParticle.PFType() == LHCb::PFParticle::ChargedInfMomentum  && m_doNotUseInfMominNR){
                            m_PFCaloClusters[(*ic).to()->seed().all()]->setStatus( LHCb::PFCaloCluster::ContainsInfMomentum );
                        }
                    }
                }
            }
            // Now if needed treat HCAL
            if( m_useHCAL ){
                if( hcalAssoc.size()>0){
                    for (LHCb::Calo2Track::IClusTrTable2D::InverseType::Range::const_iterator ic = hcalAssoc.begin();
                            hcalAssoc.end()!= ic; ++ic){
                        // Should it be banned? Compare the chi2 of the track<->cluster 2D association (x,y projection) to settings
                        if(    ( (*ic).weight() < m_Chi2HCAL0Cut  && (*ic).to()->e() <= m_Chi2HCAL0CutEValue )
                                || ( (*ic).weight() <  m_Chi2HCAL1Cut && (*ic).to()->e() > m_Chi2HCAL0CutEValue
                                    && (*ic).to()->e() <= m_Chi2HCAL1CutEValue )
                                || ( (*ic).weight() < m_Chi2HCAL2Cut  && (*ic).to()->e() > m_Chi2HCAL1CutEValue ) ) {
                            m_PFCaloClusters[(*ic).to()->seed().all()]->updateParticleList(&theParticle);
                            theParticle.addInfo(LHCb::PFParticle::ClustHCAL,(*ic).to()->seed().all());
                            m_PFCaloClusters[(*ic).to()->seed().all()]->setStatus( LHCb::PFCaloCluster::AvailableForNeutralRecovery );
                            // In the case of non composite particles (ie itr is theParticle)
                            if ( theParticle.PFType() == LHCb::PFParticle::ChargedInfMomentum  && m_doNotUseInfMominNR){
                                m_PFCaloClusters[(*ic).to()->seed().all()]->setStatus( LHCb::PFCaloCluster::ContainsInfMomentum );
                            }
                        }
                    }
                }
            }
        }
        return StatusCode::SUCCESS;
    }

    int ParticleFlow::numberOfSaturatedCells( const LHCb::CaloCluster* cluster )
    {
        const DeCalorimeter* calo = ( cluster->seed ().calo() == CaloCellCode::HcalCalo ?
                m_hcal : m_ecal );
        int nbSatCells = 0 ;
        const std::vector< LHCb::CaloClusterEntry > & entries = cluster->entries();
        for ( std::vector< LHCb::CaloClusterEntry >::const_iterator i_entry = entries.begin();
                entries.end() != i_entry; ++i_entry )
        {
            const LHCb::CaloDigit* digit = (*i_entry).digit();
            if( digit->e()/calo->cellGain(digit->cellID())+calo->pedestalShift() > calo->adcMax()-256-0.5 )
            {
                ++nbSatCells;
            }
        }
        return nbSatCells;
    }
