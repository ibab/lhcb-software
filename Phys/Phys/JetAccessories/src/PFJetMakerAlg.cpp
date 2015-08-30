// $Id: LoKiJetMakerAlg.cpp,v 1.6 2010-01-11 08:37:42 cocov Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IJetMaker.h"
#include "Kernel/Particle2Vertex.h"
#include "Kernel/PFParticle.h"
#include "Kernel/JetEnums.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"


#include "GaudiKernel/IHistogramSvc.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "AIDA/IHistogram3D.h"
#include "boost/lexical_cast.hpp"

#include "TH1D.h"
#include "TMath.h"
#include "Math/VectorUtil.h"

class TH1D;


namespace {
  // Helper method for readability
  template<class T>
  const T& boundTo( const T& lower, const T& value, const T& upper )
  {
    return std::min( std::max( lower, value ), upper );
  }
}



// ============================================================================
namespace LoKi
{
    // ==========================================================================
    /** @class PFJetMaker
     *
     *  This file is a part of LoKi project -
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
     *  contributions and advices from G.Raven, J.van Tilburg,
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-21
     */
    class PFJetMaker : public LoKi::Algo
    {
        // ========================================================================
        /// the friend factory for instantiation
        friend class AlgFactory<LoKi::PFJetMaker> ;
        // ========================================================================
        protected:
        // ========================================================================
        /** Standard constructor
         *  @param name instance name
         *  @param pSvc pointer to Service Locator
         */
        PFJetMaker
            ( const std::string& name ,
              ISvcLocator*       pSvc )
            : LoKi::Algo ( name , pSvc )
              //
              , m_makerName ( "LoKi::FastJetMaker"   )
              , m_maker     ( 0   )
              , m_associate2Vertex ( false  )
              , m_applyJEC ( false  )
              , m_onlysavewithB ( false  )
              , m_minIPChi2PVassoc( false  )
              , m_dist(0)
              , m_minlogIPtoPV_Down(4.)
              , m_minlogIPtoPV_Up(1.5)
              , m_minlogIPtoPV_Long(1.5)
              , m_minlogIPChi2toPV_Down(7.5)
              , m_minlogIPChi2toPV_Up(9.5)
              , m_minlogIPChi2toPV_Long(9.5)
              , m_appendPileUpInfo(true)
        {
            //
            declareProperty
                ( "JetMaker"  ,
                  m_makerName ,
                  "Type type/name of jet-maker tool (IJetMaker interface)") ;
            declareProperty
                ( "Associate2Vertex"  ,
                  m_associate2Vertex ,
                  "Jet reconstruction per vertex") ;
            declareProperty ( "ApplyJetID"  ,
                    m_applyJetID = false,
                    "Apply jet ID cuts") ;
            declareProperty ( "JetIDMinMPT"  ,
                    m_JetID_MPT = 1200,
                    "jet ID mpt cut") ;
            declareProperty ( "JetIDMinCPF"  ,
                    m_JetID_CPF = 0.1,
                    "jet ID cpf cut") ;
            declareProperty ( "JetIDMinNTrk"  ,
                    m_JetID_NTRK = 1.5,
                    "jet ID ntrk cut") ;
            declareProperty ( "JetIDMaxMPF"  ,
                    m_JetID_MPF = 0.8,
                    "jet ID MPF cut") ;
            declareProperty("ApplyJEC"  ,
                    m_applyJEC = false ,
                    "Apply jet energy corrections");
            declareProperty("ShiftJEC"  ,
                    m_shiftJEC = 0. ,
                    "Shift jet energy correction by n sigma");
            declareProperty("PFParticleTypes"  ,
                    m_inputTypes  ,
                    "Type of particles to consider");
            declareProperty("OnlySaveWithB"  ,
                    m_onlysavewithB  ,
                    "Only save jets containing a B meson");
            declareProperty("PVassociationMinIPChi2"  ,
                    m_minIPChi2PVassoc  ,
                    "True: will use the minIPchi2 to associate particles with pointing info to the PV, False is more refined");
            declareProperty("BanCandidates",
                    m_banCandidatesLocations  ,
                    "Ban the particle flow particles corresponding to this candidate");

            declareProperty("HistoPath"  , m_histo_path = "JEC14" , "The path of the JEC histograms" );
            declareProperty("AppendPileUpInfo"  , m_appendPileUpInfo, "addpend the pileup info as extra info" );
            //
        }
        /// destructor
        virtual ~PFJetMaker( ){}
        // ========================================================================
        public:
        // ========================================================================
        /** standard execution of the algorithm
         *  @see LoKi::Algo
         *  @return status code
         */

        virtual StatusCode initialize   () ;
        virtual StatusCode analyse   () ;
        // Append the Jet ID variables to the jet extra infos
        StatusCode appendJetIDInfo   ( LHCb::Particle * jet ) ;
        // Apply the Jet energy corrections
        StatusCode JEC               ( LHCb::Particle * jet )  ;
        //add PileUp usefull info
        StatusCode PileUpInfo        ( LHCb::Particle* jet )  ;
        // ========================================================================
        private:
        // ========================================================================
        // the default constructor is disabled
        PFJetMaker () ;
        // the copy constructor is disabled
        PFJetMaker ( const PFJetMaker& )  ;
        // the assignement operator is disabled
        PFJetMaker& operator=( const  PFJetMaker& )  ;
        // ========================================================================
        private:
        // ========================================================================
        /// maker name
        std::string      m_makerName ; // jet maker name
        /// maker
        const IJetMaker* m_maker     ; // jet maker to be used
        /// associate two vertex?
        bool m_associate2Vertex      ; // make jet per vertex
        /// apply JEC
        bool m_applyJEC              ;
        float m_shiftJEC             ;
        /// append Jet ID
        bool m_applyJetID              ; // append jet ID info
        /// histograms for JEC
        TH1D* m_histosJEC[4][11][4][3] ;

        //std::vector<TH1D*> m_histosJEC ;
        /// histo path
        std::string m_histo_path ;
        /// input particles to consider
        std::vector < int > m_inputTypes ;
        bool m_onlysavewithB;
        bool m_minIPChi2PVassoc;
        const IDistanceCalculator* m_dist;

        double m_minlogIPtoPV_Down;
        double m_minlogIPtoPV_Up;
        double m_minlogIPtoPV_Long;
        double m_minlogIPChi2toPV_Down;
        double m_minlogIPChi2toPV_Up;
        double m_minlogIPChi2toPV_Long;

        double m_JetID_MPT ;
        double m_JetID_NTRK ;
        double m_JetID_CPF;
        double m_JetID_MPF ;

        bool m_appendPileUpInfo;
        std::vector<std::string> m_banCandidatesLocations;



        // ========================================================================
    };
    // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::PFJetMaker
 *  @date  2005-03-21
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  standard execution of the algorithm
 *  @see LoKi::Algo
 *  @return status code
 */
// ===========================================================================
StatusCode LoKi::PFJetMaker::initialize ()
{
    StatusCode sc = LoKi::Algo::initialize() ;
    if ( sc.isFailure() ) { return sc ; }

    // Initialize the tool
    if ( 0 == m_maker ) { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
    // Read in the histograms for JEC
    if ( m_applyJEC ){
        const int netabins=11, ncpfbins=4, nphibins=3, npvbins=4;
        for(int inpvs=0; inpvs<npvbins; inpvs++) for(int ieta=0; ieta<netabins; ieta++)
            for(int icpf=0; icpf<ncpfbins; icpf++) for(int iphi=0; iphi<nphibins; iphi++) {
                std::string histoname =
                    "JECSYS_PV"+boost::lexical_cast<std::string>(inpvs+1)+
                    "_ETA"+boost::lexical_cast<std::string>(ieta)+
                    "_CPF"+boost::lexical_cast<std::string>(icpf)+
                    "_PHI"+boost::lexical_cast<std::string>(iphi);
                AIDA::IHistogram1D *aida =  get<AIDA::IHistogram1D> (histoSvc(), m_histo_path + histoname);
                if( 0==aida ) warning()<<"Could not find AIDA::IHistogram1D* "
                    << m_histo_path + histoname<<"."<<endmsg;
                m_histosJEC[inpvs][ieta][icpf][iphi] = Gaudi::Utils::Aida2ROOT::aida2root(aida) ;
                //m_histosJEC.push_back( Gaudi::Utils::Aida2ROOT::aida2root(aida) );
            }
    }
    // Give some default values to the types if not defined
    if (m_inputTypes.empty()){
        m_inputTypes.push_back(LHCb::PFParticle::ChargedHadron);
        m_inputTypes.push_back(LHCb::PFParticle::Muon);
        m_inputTypes.push_back(LHCb::PFParticle::Electron);
        m_inputTypes.push_back(LHCb::PFParticle::Photon);
        m_inputTypes.push_back(LHCb::PFParticle::MergedPi0);
        m_inputTypes.push_back(LHCb::PFParticle::ResolvedPi0);
        m_inputTypes.push_back(LHCb::PFParticle::NeutralHadron);
        m_inputTypes.push_back(LHCb::PFParticle::NeutralRecovery);
        m_inputTypes.push_back(LHCb::PFParticle::V0);
        m_inputTypes.push_back(LHCb::PFParticle::D);
        m_inputTypes.push_back(LHCb::PFParticle::B);
        m_inputTypes.push_back(LHCb::PFParticle::Charged0Momentum);
        m_inputTypes.push_back(LHCb::PFParticle::ChargedInfMomentum);
        m_inputTypes.push_back(LHCb::PFParticle::IsolatedPhoton);
    }

    m_dist   = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
    if ( !m_dist ) {
        fatal() << "Distance Calculator Tool could not be found" << endmsg;
        return StatusCode::FAILURE;
    }

    return sc ;
}


StatusCode LoKi::PFJetMaker::analyse   ()
{
    using namespace LoKi        ;
    using namespace LoKi::Types ;
    using namespace LoKi::Cuts ;


    std::vector<const LHCb::Particle*> compositeParticles;
    std::vector<const LHCb::Track*> tracksToBan;

    LoKi::Types::Cut isToBan = NONE;
    // Get the particles to be banned from jet inputs
    for (std::vector< std::string >::const_iterator i_location = m_banCandidatesLocations.begin() ;
            m_banCandidatesLocations.end() != i_location ; ++i_location ){
        if( !exist<LHCb::Particles*>(*i_location) )continue;
        const  LHCb::Particles* mypartsToBan = get<LHCb::Particles*>(*i_location);
        for (LHCb::Particles::const_iterator i_p = mypartsToBan->begin();mypartsToBan->end()!=i_p;i_p++)
        {
            LHCb::Particle::ConstVector particleToBan_daug = (*i_p)->daughtersVector();
            if( particleToBan_daug.size() == 0 ){
                if ((*i_p)->proto()== 0)continue;
                if ((*i_p)->proto()->track()== 0)continue;
                tracksToBan.push_back((*i_p)->proto()->track());

            }
            else{
                for(LHCb::Particle::ConstVector::const_iterator i_pd = particleToBan_daug.begin() ;
                        particleToBan_daug.end() != i_pd ; ++i_pd ){
                    if ((*i_pd)->proto()==0)continue;
                    if ((*i_pd)->proto()->track()==0)continue;
                    tracksToBan.push_back((*i_pd)->proto()->track());
                }

            }
        }

    }

    LoKi::Types::Fun PFType =  LoKi::Cuts::INFO(LHCb::PFParticle::Type,-10.);
    Range partComp = select("partComp", PALL && fabs(PFType- -10.)<1e-6 );


    // Get the particles to be banned from jet inputs
    for (Range::const_iterator i_p = partComp.begin() ; partComp.end() != i_p ; i_p++ ){
        // Store the composite one

        compositeParticles.push_back(*i_p);
        // and add its daughters to the list of particles to ban
        LHCb::Particle::ConstVector particleToBan_daug = (*i_p)->daughtersVector();
        if( particleToBan_daug.size() == 0 ){
            if ((*i_p)->proto()== 0)continue;
            if ((*i_p)->proto()->track()== 0)continue;
            tracksToBan.push_back((*i_p)->proto()->track());
        }
        else{
            for(LHCb::Particle::ConstVector::const_iterator i_pd = particleToBan_daug.begin() ;
                    particleToBan_daug.end() != i_pd ; ++i_pd ){
                if ((*i_pd)->proto()==0)continue;
                if ((*i_pd)->proto()->track()==0)continue;
                tracksToBan.push_back((*i_pd)->proto()->track());
            }
        }
    }


    // Cut to ban pfparticles from the inputs which contains tracks to ban (but are PFparticles in order not to ban the composite themselves...)
    LoKi::Types::Cut hastracksintree = LoKi::Cuts::HASTRACKSINTREE( tracksToBan.begin(),tracksToBan.end());
    LoKi::Types::Cut hastracks = LoKi::Cuts::HASTRACKS( tracksToBan.begin(),tracksToBan.end());

    LoKi::Types::Cut containsTrackToBan =( hastracksintree || hastracks ) ;

    if ( m_associate2Vertex ){


        // Some definitions for jet ID
        LoKi::Types::Fun mtf = LoKi::Cuts::INFO(LHCb::JetIDInfo::MTF,-10.);
        LoKi::Types::Fun mnf = LoKi::Cuts::INFO(LHCb::JetIDInfo::MNF,-10.);
        LoKi::Types::Fun mpt = LoKi::Cuts::INFO(LHCb::JetIDInfo::MPT,-10.);
        LoKi::Types::Fun cpf = LoKi::Cuts::INFO(LHCb::JetIDInfo::CPF,-10.);
        LoKi::Types::Fun nPVInfo = LoKi::Cuts::INFO(LHCb::JetIDInfo::NHasPV,-10.);

        // A cut to get the position of the bestPV of input particles (would be better to code a VKEY functor)
        LoKi::Types::Fun bestVertexVX = BPV(VX);
        LoKi::Types::Fun bestVertexVY = BPV(VY);
        LoKi::Types::Fun bestVertexVZ = BPV(VZ);

        // Get All particles that do not contains banned tracks.
        Range part = select("part", PALL &&  !(containsTrackToBan && fabs(PFType +10.)>1e-6) );

        VRange rangePV = vselect("rangePV", PRIMARY );

        // Some cuts to select the inputs
        LoKi::Types::Cut GoodInput = fabs( PFType - (-10.)) < 1e-6 ; // Keep any particles which are not from ParticleFlow as composite used to replace the pfpart tracks
        for ( int i = 0 ; i < (int) m_inputTypes.size() ; i++ ){
            GoodInput = GoodInput || fabs(  PFType - m_inputTypes[i] ) < 1e-6 ;
        }
        LoKi::Types::Cut PerPVinputs = ( PFType > LHCb::PFParticle::Charged && PFType < LHCb::PFParticle::Neutral && LHCb::Track::Downstream != TRTYPE )
            || ( PFType > LHCb::PFParticle::Composite  && PFType < LHCb::PFParticle::BadParticle )
            || ( PFType == LHCb::PFParticle::ChargedInfMomentum && LHCb::Track::Downstream != TRTYPE ) || ( PFType == LHCb::PFParticle::Charged0Momentum && LHCb::Track::Downstream != TRTYPE )
            || ( fabs(PFType+10)<1e-6 );
        LoKi::Types::Cut AllPVinputs = ( PFType > LHCb::PFParticle::Neutral && PFType < LHCb::PFParticle::Composite )
            || ( PFType == LHCb::PFParticle::BadPhotonMatchingT ) || ( PFType == LHCb::PFParticle::BadPhoton )  || ( PFType == LHCb::PFParticle::IsolatedPhoton )
            || ( PFType > LHCb::PFParticle::Charged && PFType < LHCb::PFParticle::Neutral &&  LHCb::Track::Downstream == TRTYPE )
            || ( PFType == LHCb::PFParticle::ChargedInfMomentum &&  LHCb::Track::Downstream == TRTYPE )
            || ( PFType == LHCb::PFParticle::Charged0Momentum &&  LHCb::Track::Downstream == TRTYPE );

        LoKi::Types::Cut PVPointingInfo = NINTREE( PerPVinputs )>0 ;

        // Loop over PV list and make jets out of appropriate inputs related to the PVs
        const LHCb::RecVertex::Range pvs = this->primaryVertices () ;
        for ( LHCb::RecVertex::Range::const_iterator i_pv = pvs.begin() ; pvs.end() != i_pv ; i_pv++ )
        {
            // Cut to get the particles pointing to the PV.
            const LHCb::RecVertex* pv = *i_pv;

            // Prepare the inputs
            IJetMaker::Input inputs;
            for (Range::const_iterator i_p = part.begin() ; part.end() != i_p ; i_p++ ){
                // General cut, if it is not a good input
                if (!GoodInput(*i_p))continue;

                // If we decideded to only do the best ipchi2 association and the input is to be associated to a pv but does not point to this pv, discard
                if ( m_minIPChi2PVassoc && PerPVinputs(*i_p))
                {
                    float pftype = PFType(*i_p);

                    if (std::fabs(pftype+10.)>0.5 && ( std::abs( bestVertexVZ(*i_p) - VZ(pv) ) > 1e-5
                                || std::abs( bestVertexVY(*i_p) - VY(pv) ) > 1e-5
                                || std::abs( bestVertexVX(*i_p) - VX(pv) ) > 1e-5 ) ){ continue;  }
                    else if( std::fabs(pftype+10.)<0.5 )
                    {
                        LoKi::Types::Fun chi2thisPV = CHI2IP ( pv , geo() ) ;
                        LoKi::Types::Fun minchi2 = MIPCHI2 ( rangePV , geo() ) ;
                        if(std::fabs( minchi2( *i_p )-chi2thisPV( *i_p ))<1e-6){inputs.push_back(*i_p);}
                        else continue;
                    }
                    else
                    {
                        inputs.push_back(*i_p);
                    }

                }

                // Then if it is of the type to ba added to all pv, just add it to all pv
                else if ( m_minIPChi2PVassoc && AllPVinputs(*i_p) ){
                    inputs.push_back(*i_p);
                }

                else if (!m_minIPChi2PVassoc && PerPVinputs(*i_p) ){
                    // First compute the ip and ipchi2 wrt this pv
                    double ip(1e8), chi2(1e8);
                    StatusCode dist = m_dist->distance ( (*i_p), pv, ip, chi2 );
                    if (ip<0. || chi2<0. ) continue;
                    double thisPVchi2 = chi2;
                    // Cut for long tracks
                    // If the ip and ipchi2 are below a maximal value, and no
                    if ( (*i_p)->proto()!= NULL && (*i_p)->proto()->track()->type()== LHCb::Track::Long && ip< TMath::Log( m_minlogIPtoPV_Long) &&  TMath::Log(chi2) <  m_minlogIPChi2toPV_Long ){
                        bool useIt = true;
                        for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
                            if (i_pv2 == i_pv) continue;
                            dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
                            if ( chi2<0. ) continue;
                            if( chi2<thisPVchi2){
                                useIt = false;
                                break;
                            }
                        }
                        if (useIt) inputs.push_back(*i_p);
                    }
                    else if( (*i_p)->proto()!= NULL && ((*i_p)->proto()->track()->type()== LHCb::Track::Upstream || (*i_p)->proto()->track()->type()== LHCb::Track::Velo) &&
                            TMath::Log(ip) < m_minlogIPtoPV_Up &&  TMath::Log(chi2) < m_minlogIPChi2toPV_Up ){
                        bool useIt = true;
                        for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
                            if (i_pv2 == i_pv) continue;
                            dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
                            if (ip<0. || chi2<0. ) continue;
                            if( chi2<thisPVchi2){
                                useIt = false;
                                break;
                            }
                        }
                        if (useIt) inputs.push_back(*i_p);
                    }
                    else{
                        bool useIt = true;
                        for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
                            if (i_pv2 == i_pv){
                                continue;
                            }
                            dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
                            if ( chi2<0. ) continue;
                            if( chi2<thisPVchi2){
                                useIt = false;
                                break;
                            }
                        }
                        if (useIt) inputs.push_back(*i_p);
                    }
                    // make the ip selection
                }
                else if (!m_minIPChi2PVassoc && AllPVinputs(*i_p)){
                    // make the ip selection for downstream
                    if (  ( PFType(*i_p) > LHCb::PFParticle::Charged && PFType(*i_p) < LHCb::PFParticle::Neutral )
                            || ( PFType(*i_p) == LHCb::PFParticle::ChargedInfMomentum )
                            || ( PFType(*i_p) == LHCb::PFParticle::Charged0Momentum )){

                        if( (*i_p)->proto()->track()->type()== LHCb::Track::Downstream){
                            double ip(1e8), chi2(1e8);
                            StatusCode dist = m_dist->distance ( (*i_p), pv , ip, chi2 );
                            double thisPVchi2 = chi2;
                            if ( chi2<0. ) continue;
                            if( TMath::Log(ip) < m_minlogIPtoPV_Down &&  TMath::Log(chi2) < m_minlogIPChi2toPV_Down ){
                                bool useIt = true;
                                for ( LHCb::RecVertex::Range::const_iterator i_pv2 = pvs.begin() ; pvs.end() != i_pv2 ; i_pv2++ ){
                                    if (i_pv2 == i_pv) continue;
                                    dist = m_dist->distance ( (*i_p), (*i_pv2), ip, chi2 );
                                    if (ip<0. || chi2<0. ) continue;
                                    if( chi2<thisPVchi2){
                                        useIt = false;
                                        break;
                                    }
                                }
                                if (useIt) inputs.push_back(*i_p);
                            }
                        }
                    }
                    else{
                        inputs.push_back(*i_p);
                    }
                }
                else{
                    continue;
                }
            }

            // ouput container
            IJetMaker::Jets jets ;
            // make the jets
            StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;

            if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }

            // save all jets
            while ( !jets.empty() )
            {
                LHCb::Particle* jet = jets.back() ;
                this->appendJetIDInfo(jet);
                if(m_applyJEC) this->JEC(jet);
                // If the jet contain info on PV, assign a PV and update the P2PV relation table
                if ( PVPointingInfo(jet) ){

                    jet->setReferencePoint( Gaudi::XYZPoint((*i_pv)->position ()) );
                    LHCb::Vertex* vJet = new LHCb::Vertex();
                    vJet->setPosition((*i_pv)->position());
                    vJet->setCovMatrix((*i_pv)->covMatrix());
                    vJet->setChi2((*i_pv)->chi2());
                    vJet->setNDoF((*i_pv)->nDoF());
                    vJet->setOutgoingParticles(jet->daughters());
                    jet->setEndVertex(vJet);
                    this->relate ( jet , *i_pv );
                    if(m_appendPileUpInfo)
                        this->PileUpInfo(jet);


                }

                if ( !( m_applyJetID && ( mpt(jet)< m_JetID_MPT  || nPVInfo(jet)<   m_JetID_NTRK ||  cpf(jet) <  m_JetID_CPF || std::max(mtf(jet),mnf(jet))  >  m_JetID_MPF  ) ) )
                {

                    if (!( m_onlysavewithB && jet->info(LHCb::JetEnergyFractionInfo::B,-100.)<1.e-6)){
                        verbose()<<PT(jet)<<" "<<jet->info(LHCb::JetEnergyFractionInfo::B,-100.)<<" "<<ID(jet)<<endmsg;
                        sc = save ( "jets" , jet ) ;
                        if ( sc.isFailure() ) { return Error ( "Error from save function in jet maker" , sc ) ; }
                    }
                }
                jets.pop_back() ;
                unRelatePV(jet);
                delete jet->endVertex();
                delete jet;


            }
        }
    }
    else{
        // Some definitions for jet ID
        LoKi::Types::Fun mtf = LoKi::Cuts::INFO(LHCb::JetIDInfo::MTF,-10.);
        LoKi::Types::Fun mnf = LoKi::Cuts::INFO(LHCb::JetIDInfo::MNF,-10.);
        LoKi::Types::Fun mpt = LoKi::Cuts::INFO(LHCb::JetIDInfo::MPT,-10.);
        LoKi::Types::Fun cpf = LoKi::Cuts::INFO(LHCb::JetIDInfo::CPF,-10.);
        LoKi::Types::Fun nPVInfo = LoKi::Cuts::INFO(LHCb::JetIDInfo::NHasPV,-10.);
        LoKi::Types::Fun PFType =  LoKi::Cuts::INFO(LHCb::ParticleFlowType::KEY,-10.);
        LoKi::Types::Cut GoodInput = fabs( PFType + 10. ) < 1e-6 ;
        for ( int i = 0 ; i < (int) m_inputTypes.size() ; i++ ){
            GoodInput = GoodInput || fabs(  PFType - m_inputTypes[i] ) < 1e-6 ;
        }
        // The inputs range
        Range part = select("input4jets",PALL && GoodInput&& !containsTrackToBan  );
        IJetMaker::Input inputs;
        for (Range::const_iterator i_p = part.begin() ; part.end() != i_p ; i_p++ ){ inputs.push_back(*i_p); }
        // ouput container
        IJetMaker::Jets jets ;
        // make the jets
        StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;
        if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }
        // save all jets
        while ( !jets.empty() )
        {
            LHCb::Particle* jet = jets.back() ;
            this->appendJetIDInfo(jet);


            if ( !( m_applyJetID && ( mpt(jet)< m_JetID_MPT  || nPVInfo(jet)<   m_JetID_NTRK ||  cpf(jet) <  m_JetID_CPF || std::max(mtf(jet),mnf(jet))  >  m_JetID_MPF  ) ) )
            {

                if (!( m_onlysavewithB && jet->info(LHCb::JetEnergyFractionInfo::B,-100.)<1.e-6)){
                    sc = save ( "jets" , jet ) ;
                    if ( sc.isFailure() ) { return Error ( "Error from save function in jet maker" , sc ) ; }
                }
            }
            jets.pop_back() ;
            delete jet;


        }
    }
    if ( statPrint() || msgLevel ( MSG::DEBUG ) )
    { counter ( "#jets" ) += selected ("jets").size() ; }
    setFilterPassed ( true ) ;

    return StatusCode::SUCCESS ;
}

StatusCode LoKi::PFJetMaker::appendJetIDInfo( LHCb::Particle* jet )
{
    // Get Jet Daughters
    std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
    std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin();

    double mtf;    /// Highest pT track / Jet pT
    double mnf;    /// Highest pT neutral part / Jet pT
    double mpt;    /// Highest pT track
    double cpf;    /// charged pT fraction - V0s are not included
    double width;  /// jet width
    int    n90;    /// Number of items responsible for at least 90% of the jet momentum
    int    ntrk;   /// Number of tracks

    float auxptmax=-1, auxptPmax=-1,sumpt=0;
    int iitems=0;
    double tpx=0, tpy=0;
    std::vector<float> itemspt;
    ntrk=n90=width=0;


    for (;idaughter != daughtersvector.end() ; ++idaughter){
        const LHCb::Particle * daughter = *idaughter;
        if(daughter->particleID().threeCharge()!=0) {
            ntrk++; auxptmax = daughter->momentum().Pt() > auxptmax ? daughter->momentum().Pt() : auxptmax;
            tpx+=daughter->momentum().Px(); tpy+=daughter->momentum().Py();
        }else{
            auxptPmax = daughter->momentum().Pt() > auxptPmax ? daughter->momentum().Pt() : auxptPmax;
        }
        iitems++; float pt = daughter->momentum().Pt(); sumpt+=pt;
        itemspt.push_back(pt);
        for(int ii=0; ii<iitems; ii++) if(itemspt[ii]<pt) {
            float aux = itemspt[ii]; itemspt[ii]=pt; pt = aux;}
        width += ROOT::Math::VectorUtil::DeltaR(daughter->momentum(),jet->momentum()) * daughter->momentum().Pt();
    }

    mtf = auxptmax / jet->momentum().Pt(); mtf = 0 > mtf ? 0 : mtf; mtf = 1 < mtf ? 1 : mtf;
    mnf = auxptPmax / jet->momentum().Pt(); mnf = 0 > mnf ? 0 : mnf; mnf = 1 < mnf ? 1 : mnf;

    mpt = auxptmax;
    cpf = TMath::Sqrt(tpx*tpx+tpy*tpy)/jet->momentum().Pt();
    width /= sumpt;

    sort (itemspt.begin(), itemspt.end());
    float auxptsum = 0; n90=0;
    for(int ii=iitems-1; ii>=0; ii--) {auxptsum+=itemspt[ii]; n90++; if(auxptsum/sumpt>0.9) break; }

    LoKi::Types::Fun PFType =  LoKi::Cuts::INFO(LHCb::PFParticle::Type,-10.);
    // Function to evaluate the saturated cells Number
LoKi::Types::Fun NECALsatCells = LoKi::Cuts::SUMTREE( LoKi::Cuts::INFO(LHCb::JetIDInfo::NSatECAL,0.),LoKi::Cuts::INFO(LHCb::JetIDInfo::NSatECAL,-10.)>0., 0.);
    LoKi::Types::Fun NHCALsatCells = LoKi::Cuts::SUMTREE( LoKi::Cuts::INFO(LHCb::JetIDInfo::NSatHCAL,0.),LoKi::Cuts::INFO(LHCb::JetIDInfo::NSatHCAL,-10.)>0., 0.);
    LoKi::Types::Fun NsatCells = NECALsatCells + NHCALsatCells ;
    // Function to get the number of particles with PV information
    LoKi::Types::Fun BestPVIPChi2  = LoKi::Cuts::BPVIPCHI2 ("")  ;
    LoKi::Types::Fun N_HasPVInfo   = LoKi::Cuts::NINTREE( ( PFType > LHCb::PFParticle::Composite  && PFType < LHCb::PFParticle::BadParticle ) ||
            ( LoKi::Cuts::HASTRACK && LHCb::Track::Downstream != LoKi::Cuts::TRTYPE ) );
    LoKi::Types::Fun N_HasPVInfoChi24   = LoKi::Cuts::NINTREE(( ( PFType > LHCb::PFParticle::Composite  && PFType < LHCb::PFParticle::BadParticle ) ||
                ( LoKi::Cuts::HASTRACK && LHCb::Track::Downstream != LoKi::Cuts::TRTYPE )) && BestPVIPChi2 < 9 );

    jet->addInfo ( LHCb::JetIDInfo::Ntracks , ntrk );
    jet->addInfo ( LHCb::JetIDInfo::N90 , n90 );
    jet->addInfo ( LHCb::JetIDInfo::MTF , mtf );
    jet->addInfo ( LHCb::JetIDInfo::MNF , mnf );
    jet->addInfo ( LHCb::JetIDInfo::MPT , mpt );
    jet->addInfo ( LHCb::JetIDInfo::NSatCalo , NsatCells(jet) );
    jet->addInfo ( LHCb::JetIDInfo::NHasPV   , N_HasPVInfo(jet) );
    jet->addInfo ( LHCb::JetIDInfo::CPF , cpf );
    jet->addInfo ( LHCb::JetIDInfo::JetWidth , width );
    jet->addInfo ( LHCb::JetIDInfo::NSatECAL , NECALsatCells(jet) );
    jet->addInfo ( LHCb::JetIDInfo::NSatHCAL , NHCALsatCells(jet) );
    jet->addInfo ( LHCb::JetIDInfo::NIPChi2Inf4 , N_HasPVInfoChi24(jet) );

    LoKi::Types::Fun FractionChargedHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::ChargedHadron ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::ChargedHadron  , FractionChargedHadron(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionMuon = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Muon ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Muon  , FractionMuon(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionElectron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Electron ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Electron  , FractionElectron(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionCharged = FractionChargedHadron + FractionMuon + FractionElectron;
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Charged  , FractionCharged(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Photon ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Photon , FractionPhoton(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Pi0 ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Pi0  , FractionPi0(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionMergedPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::MergedPi0 ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::MergedPi0  , FractionMergedPi0(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionResolvedPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::ResolvedPi0 ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::ResolvedPi0  , FractionResolvedPi0(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionNeutralHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::NeutralHadron ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::NeutralHadron , FractionNeutralHadron(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionNeutralRecovery = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::NeutralRecovery ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::NeutralRecovery  , FractionNeutralRecovery(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionIsolatedPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::IsolatedPhoton ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::IsolatedPhoton , FractionIsolatedPhoton(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionNeutral = FractionPhoton + FractionPi0 + FractionMergedPi0 + FractionResolvedPi0 + FractionNeutralHadron + FractionNeutralRecovery + FractionIsolatedPhoton;
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Neutral  , FractionNeutral(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionV0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::V0 ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::V0  , FractionV0(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionD = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::D ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::D  , FractionD(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionB = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::B ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::B  , FractionB(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionComposite = FractionV0 + FractionD + FractionB ;
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Composite  , FractionComposite(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionCharged0Momentum = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::Charged0Momentum ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::Charged0Momentum  , FractionCharged0Momentum(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionChargedInfMomentum = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::ChargedInfMomentum ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::ChargedInfMomentum  , FractionChargedInfMomentum(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionBadPhotonMatchingT = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::BadPhotonMatchingT ) < 1e-6 , 0. );
    jet->addInfo ( LHCb::JetEnergyFractionInfo::BadPhotonMatchingT  , FractionBadPhotonMatchingT(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionBadPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , fabs( PFType -  LHCb::PFParticle::BadPhoton ) < 1e-6 , 0. );
    jet->addInfo (  LHCb::JetEnergyFractionInfo::BadPhoton , FractionBadPhoton(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionBadParticle = FractionCharged0Momentum + FractionChargedInfMomentum + FractionBadPhotonMatchingT + FractionBadPhoton;
    jet->addInfo ( LHCb::JetEnergyFractionInfo::BadParticle  , FractionBadParticle(jet)/LoKi::Cuts::E(jet) );






    LoKi::Types::Cut NoV0s = ((fabs( PFType -  LHCb::PFParticle::ChargedHadron ) < 1e-6 ) || (fabs( PFType -  LHCb::PFParticle::Electron ) < 1e-6 ) || (fabs( PFType -  LHCb::PFParticle::Muon ) < 1e-6 ));

    LoKi::Types::Fun N_LongTrk   = LoKi::Cuts::NINTREE( LoKi::Cuts::HASTRACK && LHCb::Track::Long == LoKi::Cuts::TRTYPE && NoV0s )       ;
    LoKi::Types::Fun Ef_LongTrk  = LoKi::Cuts::SUMTREE(LoKi::Cuts::E, LoKi::Cuts::HASTRACK && LHCb::Track::Long == LoKi::Cuts::TRTYPE && NoV0s , 0.)       ;

    jet->addInfo ( LHCb::JetEnergyFractionInfo::NLongTrk , N_LongTrk(jet));
    jet->addInfo ( LHCb::JetEnergyFractionInfo::EfLongTrk , Ef_LongTrk(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun N_DownTrk   = LoKi::Cuts::NINTREE( LoKi::Cuts::HASTRACK && LHCb::Track::Downstream == LoKi::Cuts::TRTYPE && NoV0s)       ;
    LoKi::Types::Fun Ef_DownTrk  = LoKi::Cuts::SUMTREE(LoKi::Cuts::E, LoKi::Cuts::HASTRACK && LHCb::Track::Downstream == LoKi::Cuts::TRTYPE && NoV0s , 0.)       ;

    jet->addInfo ( LHCb::JetEnergyFractionInfo::NDownTrk , N_DownTrk(jet));
    jet->addInfo ( LHCb::JetEnergyFractionInfo::EfDownTrk , Ef_DownTrk(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun N_UpTrk   = LoKi::Cuts::NINTREE( LoKi::Cuts::HASTRACK && LHCb::Track::Upstream == LoKi::Cuts::TRTYPE && NoV0s )       ;
    LoKi::Types::Fun Ef_UpTrk  = LoKi::Cuts::SUMTREE(LoKi::Cuts::E, LoKi::Cuts::HASTRACK && LHCb::Track::Upstream == LoKi::Cuts::TRTYPE && NoV0s , 0.)       ;

    jet->addInfo ( LHCb::JetEnergyFractionInfo::NUpTrk , N_UpTrk(jet));
    jet->addInfo ( LHCb::JetEnergyFractionInfo::EfUpTrk , Ef_UpTrk(jet)/LoKi::Cuts::E(jet) );



    return SUCCESS;
}


StatusCode LoKi::PFJetMaker::JEC( LHCb::Particle* jet )
{


    int PV = this->primaryVertices().size();
    int usePV = PV;
    const int netabins=11, ncpfbins=4, nphibins=3, npvbins=4;
    if (PV > npvbins-1) usePV = npvbins;
    float jetpt = LoKi::Cuts::PT(jet)/1000.;
    float jeteta = LoKi::Cuts::ETA(jet);
    float jetphi = LoKi::Cuts::PHI(jet);
    float jetcpf = jet->info(LHCb::JetIDInfo::CPF,-1.);

    if(jetpt<5)    jetpt=5;
    if(jetpt>299)  jetpt=298;
    if(jeteta<2.05) jeteta=2.05;
    if(jeteta>4.4) jeteta=4.45;
    if(jetcpf<0.2) jetcpf = 0.2;
    if(jetcpf>0.99) jetcpf = 0.99;



    float auxjetphi = TMath::Abs(TMath::Abs(jetphi)-TMath::Pi()/2.);
    float jetetalimits[netabins+1]={2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.8,3.0,3.2,3.8,4.5};
    float jetcpflimits[ncpfbins+1]={0.1,0.3,0.5,0.7,1.0};
    float pi = TMath::Pi();
    float jetphilimits[nphibins+1]={0,((float)(1.0/3.0*(0.5*(pi)))),((float)(2.0/3.0*(0.5*(pi)))),((float)(0.5*(pi)))};
    int ijeteta=0; while(jeteta>jetetalimits[ijeteta]) ijeteta++; ijeteta--;
    int ijetcpf=0; while(jetcpf>jetcpflimits[ijetcpf]) ijetcpf++; ijetcpf--;
    int ijetphi=0; while(auxjetphi>jetphilimits[ijetphi]) ijetphi++; ijetphi--;

    TH1D* histo = m_histosJEC[usePV-1][ijeteta][ijetcpf][ijetphi];

    double cor = 1.;
    cor = histo->Interpolate(jetpt);
    double corerr = histo->GetBinError(histo->FindBin(jetpt));
    cor+=m_shiftJEC*corerr;
    // Store the uncorrected kinematics

    jet->addInfo ( LHCb::JECInfo::JEC , cor );
    jet->addInfo ( LHCb::JECInfo::NPVsForJEC , PV);
    jet->addInfo ( LHCb::JECInfo::JECError , corerr );

    Gaudi::LorentzVector newMom(cor*LoKi::Cuts::PX(jet),cor*LoKi::Cuts::PY(jet),cor*LoKi::Cuts::PZ(jet),cor*LoKi::Cuts::E(jet));
    jet->setMomentum(newMom);

    return SUCCESS;
}

StatusCode LoKi::PFJetMaker::PileUpInfo( LHCb::Particle* jet )
{


    using namespace LoKi        ;
    using namespace LoKi::Types ;
    using namespace LoKi::Cuts ;



    int Ntrk06 = 0;
    int NOtrk06 = 0;
    double ENtrk06 = 0;
    double PTNtrk06 = 0;

    int Ntrk1 = 0;
    int NOtrk1 = 0;
    double ENtrk1 = 0;
    double PTNtrk1 = 0;

    int Ntrk15 = 0;
    int NOtrk15 = 0;
    double ENtrk15 = 0;
    double PTNtrk15 = 0;


    int PV = this->primaryVertices().size();

    if(PV>1){


        LHCb::Particle::Range part = select("part2", PALL);

        LoKi::PhysTypes::Fun dr2 = LoKi::Cuts::DR2 ( jet->momentum() ) ;
        const LHCb::VertexBase* originVtx = bestVertex( jet );


        LHCb::Particle::Range::iterator iTrk;
        for(iTrk = part.begin(); iTrk!= part.end(); iTrk++){
            const LHCb::Particle* testTrk = *iTrk;
            if(originVtx->key() == (bestVertex(testTrk))->key()) continue;

            const LHCb::ProtoParticle* ch_pp = testTrk->proto();
            if(ch_pp==NULL) continue;
            const LHCb::Track* ch_trk = ch_pp->track();
            if(ch_trk==NULL) continue;
            if(ch_trk->type() == LHCb::Track::Downstream ) continue;
            double dR = std::sqrt(dr2(testTrk));
            if(dR > 1.5) continue;


            if(ch_trk->type() == LHCb::Track::Long || ch_trk->type() == LHCb::Track::Upstream ){
                Ntrk15++;
                ENtrk15+=testTrk->momentum().E();
                PTNtrk15+=testTrk->pt();
            }else{
                NOtrk15++;
            }
            if(dR > 1.0) continue;


            if(ch_trk->type() == LHCb::Track::Long || ch_trk->type() == LHCb::Track::Upstream ){
                Ntrk1++;
                ENtrk1+=testTrk->momentum().E();
                PTNtrk1+=testTrk->pt();
            }else{
                NOtrk1++;
            }
            if(dR > 0.6) continue;


            if(ch_trk->type() == LHCb::Track::Long || ch_trk->type() == LHCb::Track::Upstream ){
                Ntrk06++;
                ENtrk06+=testTrk->momentum().E();
                PTNtrk06+=testTrk->pt();
            }else{
                NOtrk06++;
            }
        }

    }
    jet->addInfo ( LHCb::JetPileUpInfo::PUnPVs , PV);
    jet->addInfo ( LHCb::JetPileUpInfo::PUSumPTLongTrkCone06 , PTNtrk06);
    jet->addInfo ( LHCb::JetPileUpInfo::PUSumELongTrkCone06 , ENtrk06);
    jet->addInfo ( LHCb::JetPileUpInfo::PUNLongTrkCone06 , Ntrk06);
    jet->addInfo ( LHCb::JetPileUpInfo::PUNUpVeloTrkCone06 , NOtrk06);
    jet->addInfo ( LHCb::JetPileUpInfo::PUSumPTLongTrkCone10 , PTNtrk1);
    jet->addInfo ( LHCb::JetPileUpInfo::PUSumELongTrkCone10 , ENtrk1);
    jet->addInfo ( LHCb::JetPileUpInfo::PUNLongTrkCone10 , Ntrk1);
    jet->addInfo ( LHCb::JetPileUpInfo::PUNUpVeloTrkCone10 , NOtrk1);
    jet->addInfo ( LHCb::JetPileUpInfo::PUSumPTLongTrkCone15 , PTNtrk15);
    jet->addInfo ( LHCb::JetPileUpInfo::PUSumELongTrkCone15 , ENtrk15);
    jet->addInfo ( LHCb::JetPileUpInfo::PUNLongTrkCone15 , Ntrk15);
    jet->addInfo ( LHCb::JetPileUpInfo::PUNUpVeloTrkCone15 , NOtrk15);
    return SUCCESS;
}



// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,PFJetMaker)
    // ============================================================================
    // The END
    // ============================================================================
