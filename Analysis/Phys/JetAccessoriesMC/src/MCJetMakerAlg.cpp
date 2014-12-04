// $Id: LoKiJetMakerAlg.cpp,v 1.6 2010-01-11 08:37:42 cocov Exp $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "LoKi/IGenHybridFactory.h"
#include "LoKi/IMCHybridFactory.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/AlgoMC.h"
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
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"

#include "LoKi/GenParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/GenExtract.h"
#include "Event/HepMCEvent.h"

#include "GaudiKernel/IHistogramSvc.h"
#include <GaudiUtils/Aida2ROOT.h>
#include "AIDA/IHistogram3D.h"
#include "boost/lexical_cast.hpp"

#include "TMath.h"
#include "Math/VectorUtil.h"

namespace
{
    /// local "none"
    typedef LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant       _PBOOL ;
    typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant       _PBOOLMC ;
    // ==========================================================================
}
// ============================================================================
namespace LoKi
{
    // ==========================================================================
    /** @class MCJetMaker
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
    class MCJetMaker : public LoKi::AlgoMC
    {
        // ========================================================================
        /// the friend factory for instantiation
        friend class AlgFactory<LoKi::MCJetMaker> ;
        // ========================================================================
        protected:
        // ========================================================================
        /** Standard constructor
         *  @param name instance name
         *  @param pSvc pointer to Service Locator
         */
        MCJetMaker
            ( const std::string& name ,
              ISvcLocator*       pSvc )
            : LoKi::AlgoMC ( name , pSvc )
              //
              , m_makerName    ( "LoKi::FastJetMaker"   )
              , m_maker        ( 0                      )
              , m_inputTypes   (                        )
              , m_codeForMotherMC( "MCNONE"                )
              , m_codeForBannedMC( "MCNONE"                )
              , m_preambuloMC   (                        )
              , m_factoryMC    ( "LoKi::Hybrid::MCTool/MCFactory:PUBLIC" )
              , m_cutForMotherMC ( _PBOOLMC ( false )       )
              , m_cutForBannedMC ( _PBOOLMC ( false )       )
              , m_simpleAcceptance ( false )
              , m_outputTableMC ("Relations/Phys/MCJets2MCParticles")
              , m_jetsFromMotherOnly (false)
              , m_rejectNeutrinos(true)
        {
            //
            declareProperty  ( "JetMaker"  ,
                    m_makerName ,
                    "Type type/name of jet-maker tool (IJetMaker interface)") ;

            declareProperty  ( "CodeForMotherSelectionMC" ,
                    m_codeForMotherMC ,
                    "The functor for mother selection" );
            declareProperty  ( "CodeForBannedSelectionMC" ,
                    m_codeForBannedMC ,
                    "The functor for selecting the particle from which to ban the decendant" );

            declareProperty  ( "OutputTableMC" ,
                    m_outputTableMC ,
                    "Location of the Jet 2 MCParticles relation table" );
            declareProperty  ( "SimpleAcceptance" ,
                    m_simpleAcceptance ,
                    "A simple theta cut on the acceptance" );
            declareProperty  ( "RejectNeutrinos" ,
                    m_rejectNeutrinos ,
                    "to reject neutrinos from the final particle" );
            declareProperty  ( "SaveMCJetsFromMotherOnly" ,
                    m_jetsFromMotherOnly ,
                    "Only save the jets from mother" );
            //
        }
        /// destructor
        virtual ~MCJetMaker( ){}
        // ========================================================================
        public:
        // ========================================================================
        /** standard execution of the algorithm
         *  @see LoKi::AlgoMC
         *  @return status code
         */

        enum ParticleExtraInfo{ StartJetExtraInfo =9000,
            Ntracks = 9001 ,
            N90 = 9002 ,
            MTF = 9003 ,
            NSatCalo = 9004,
            NHasPV = 9005,
            CPF = 9006,
            JetWidth = 9007,
            NSatECAL = 9008,
            NSatHCAL = 9009,
            NIPChi2Inf4 = 9010,
            MPT = 9011,
            Charged = 9201,
            ChargedHadron = 9202,
            Muon  = 9203,
            Electron = 9204,
            Neutral  = 9205,
            Photon = 9206,
            Pi0 = 9207,
            MergedPi0 = 9208,
            ResolvedPi0 = 9209,
            NeutralHadron = 9210,
            NeutralRecovery = 9211 ,
            Composite  = 9212  ,
            V0 = 9213 ,
            D = 9214 ,
            B = 9215 ,
            BadParticle = 9216  ,
            Charged0Momentum = 9217  ,
            ChargedInfMomentum = 9218 ,
            BadPhotonMatchingT = 9219 ,
            BadPhoton = 9220 ,
            IsolatedPhoton = 9221,
            LLBeforeTT = 9222,
            LLAfterTT = 9223,
            LLNoVertex = 9224,
            FromInteractionN =9300,
            FromInteractionWithMother =9301,
            HasS = 9302 ,
            HasC = 9303 ,
            HasB = 9304 ,
            HasV0 = 9305 ,
            Hass = 9306 ,
            Hasc = 9307 ,
            Hasb = 9308 ,
            FromMother = 9309 ,
            MotherID  = 9310,
            ProcessID = 9311,
            GBarCode = 9312,
            GMotherBarCode = 9313,
            FracOfOtherMother = 9314
        };



        virtual StatusCode initialize   () ;
        virtual StatusCode analyse   () ;
        // Append the Jet ID variables to the jet extra infos
        StatusCode appendJetIDInfo   ( LHCb::Particle * jet ) ;
        StatusCode updateMajor( ) ;
        StatusCode decodeCode ( ) ;
        std::string preambuloMC ( ) const;

        // ========================================================================
        private:
        // ========================================================================
        // the default constructor is disabled
        MCJetMaker () ;
        // the copy constructor is disabled
        MCJetMaker ( const MCJetMaker& )  ;
        // the assignement operator is disabled
        MCJetMaker& operator=( const  MCJetMaker& )  ;
        // ========================================================================
        private:
        // ========================================================================
        /// maker name
        std::string      m_makerName ; // jet maker name
        /// maker
        const IJetMaker* m_maker     ; // jet maker to be used
        /// List of inputs types to consider
        std::vector< std::string > m_inputTypes ; //inputs types to consider
        /// Particle Service
        LHCb::IParticlePropertySvc* m_ppSvc; //ParticleSvc
        /// Code for the predicate for the selection of Mother
        std::string m_codeForMotherMC;
        /// Code for the predicate for the selection of Banned particles
        std::string m_codeForBannedMC;
        /// the preambulo
        std::vector<std::string> m_preambuloMC ; // the preambulo
        /// LoKi/Bender "hybrid" factory name
        std::string m_factoryMC ; // LoKi/Bender "hybrid" factory name
        /// the predicate for the selection of Motherc
        LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate m_cutForMotherMC ;
        /// the predicate for the selection of banned particles
        LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate m_cutForBannedMC ;
        /// apply a simple acceptance cut
        bool m_simpleAcceptance ;
        /// Location of the HepMC to LHCb::Particle table
        std::string m_outputTableMC;
        /// Only save the jets from mother
        bool m_jetsFromMotherOnly;
        /// List of Long lvied part
        std::vector<int> m_listOfLongLived;
        //reject neutrino flag
        bool m_rejectNeutrinos;
        std::vector<int> m_listofNeutrinos;


        std::string m_daughtersLocation;
        std::string m_jetsLocation;

        bool isALongLivedDecay( const LHCb::MCParticle *mcparticle );
        bool isFromPV(const LHCb::MCParticle *mcparticle );
        // ========================================================================
    };
    // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::MCJetMaker
 *  @date  2005-03-21
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  standard execution of the algorithm
 *  @see LoKi::AlgoMC
 *  @return status code
 */
// ===========================================================================
StatusCode LoKi::MCJetMaker::initialize ()
{
    StatusCode sc = LoKi::AlgoMC::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    // decode the cut:
    sc = updateMajor () ;
    if ( sc.isFailure () )
    { return Error ("The error from updateMajor" , sc ) ; }

    // Initialize the tool
    if ( 0 == m_maker ) { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
    sc = service("LHCb::ParticlePropertySvc", m_ppSvc);
    if ( sc.isFailure() ) { return sc ; }

    m_daughtersLocation = "Phys/";
    m_daughtersLocation.append(this->name());
    m_daughtersLocation.append("Particles/Particles");
    m_jetsLocation  = "Phys/";
    m_jetsLocation.append(this->name());
    m_jetsLocation.append("Jets/Particles");


    m_listofNeutrinos.push_back(12);
    m_listofNeutrinos.push_back(14);
    m_listofNeutrinos.push_back(16);

    m_listOfLongLived.push_back(321);
    m_listOfLongLived.push_back(211);
    m_listOfLongLived.push_back(130);
    m_listOfLongLived.push_back(3222);
    m_listOfLongLived.push_back(310);
    m_listOfLongLived.push_back(3122);
    m_listOfLongLived.push_back(3112);
    m_listOfLongLived.push_back(3312);
    m_listOfLongLived.push_back(3322);// f_0
    // m_listOfLongLived.push_back(30221);
    // m_listOfLongLived.push_back(9010221);

    return StatusCode::SUCCESS ;
}

// ============================================================================
// construct the preambulo string
// ============================================================================
std::string LoKi::MCJetMaker::preambuloMC() const
{
    // Put some default to the preambulo so that it is not necessary to specify each time
    std::string result = "from LoKiMC.decorators import *\nfrom LoKiNumbers.decorators import *\nfrom LoKiCore.functions import *\nfrom LoKiCore.math import *";

    for ( std::vector<std::string>::const_iterator iline =
            m_preambuloMC.begin() ; m_preambuloMC.end() != iline ; ++iline )
    {
        if ( m_preambuloMC.begin() != iline ) { result += "\n" ; }
        result += (*iline) ;
    }
    return result ;
}
// ============================================================================
// update the major properties
// ============================================================================
StatusCode LoKi::MCJetMaker::updateMajor ()
{
    // decode the code
    StatusCode sc = decodeCode () ;
    if ( sc.isFailure() ) { return Error ( "Error from decodeCode()'" ) ; }

    // locate the factory
    LoKi::IMCHybridFactory* factoryMC = tool<LoKi::IMCHybridFactory> ( m_factoryMC , this ) ;

    // release the factory (not needed anymore)
    release ( factoryMC ) ;
    //
    return StatusCode::SUCCESS ;
}


// ============================================================================
// decode the code
// ============================================================================
StatusCode LoKi::MCJetMaker::decodeCode ()
{

    // locate the factory
    LoKi::IMCHybridFactory* MCfactory_ = tool<LoKi::IMCHybridFactory> ( m_factoryMC , this ) ;
    //
    // use the factory
    StatusCode  sc = MCfactory_ -> get ( m_codeForMotherMC , m_cutForMotherMC , preambuloMC() ) ;
    if ( sc.isFailure() )
    { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='"
            + m_codeForMotherMC + "'" , sc )  ; }
    sc = MCfactory_ -> get ( m_codeForBannedMC , m_cutForBannedMC , preambuloMC() ) ;
    if ( sc.isFailure() )
    { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='"
            + m_codeForBannedMC + "'" , sc )  ; }
    //
    release ( MCfactory_ ) ;
    //



    return sc ;
}

StatusCode LoKi::MCJetMaker::analyse   ()
{
    using namespace LoKi        ;
    using namespace LoKi::Types ;
    using namespace LoKi::Cuts  ;




    LHCb::MCParticles* mcparts = get< LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
    LHCb::MCVertex::Container* mcv = get< LHCb::MCVertex::Container>( LHCb::MCVertexLocation::Default );

    std::vector<const LHCb::MCVertex*>  primaries;
    for ( LHCb::MCVertex::Container::const_iterator imc = mcv->begin() ; mcv->end() != imc ; ++imc ) {

        const LHCb::MCVertex* mcv = *imc ;
        if ( 0 == mcv || !mcv->isPrimary() ) { continue ; }
        primaries.push_back ( mcv ) ;


    }


    LHCb::MCParticle::ConstVector StableParts;
    LHCb::MCParticle::Vector::iterator  mcIt;


    std::vector< const LHCb::MCParticle* > mothers;



    typedef LHCb::Relation1D< LHCb::Particle, LHCb::MCParticle > Table ;

    LHCb::Particles* daughterParticles = new LHCb::Particles();
    LHCb::Particles* jetParticles = new LHCb::Particles();

    put(daughterParticles,m_daughtersLocation);
    put(jetParticles,m_jetsLocation);


    //Cb::HepMC2MC2D* tableHepMC2MC = 0 ;
    Table* table = 0 ;
    if ( !m_outputTableMC.empty() ){
        table = new Table(100) ;
        put ( table , m_outputTableMC ) ;
        //hleHepMC2MC = get<LHCb::HepMC2MC2D> ( LHCb::HepMC2MCLocation::Default ) ;
    }

    // Initialize the tool
    if ( 0 == m_maker ) { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }

    // PID and stable particles cut
    LoKi::Types::MCCut cut = MCVALID ;
    //  for ( std::vector< std::string >::iterator pid = m_inputTypes.begin() ; m_inputTypes.end() != pid ; ++ pid ){
    //    if( pid == m_inputTypes.begin() ) cut =  ( (*pid) == LoKi::Cuts::MCABSID );
    //    else cut =  ( cut || ( (*pid) == LoKi::Cuts::MCABSID ) );
    //  }
    if ( m_simpleAcceptance ) cut = cut && ( LoKi::Cuts::MCTHETA < 0.4 );
    //  MCCut isToBan = ( LoKi::Cuts::MCNINTREE( m_cutForBanned , HepMC::ancestors ) > 0 ) || ( m_cutForBanned );
    //  cut = cut && ( ( LoKi::Cuts::MCSTATUS == 1 ) || ( LoKi::Cuts::MCSTATUS == 999 ) ) // && ( !isToBan );
    // Some cuts to identify the type of jet

    MCCut containsB = LoKi::Cuts::MCQUARK( LHCb::ParticleID::bottom ) ;
    MCCut containsC = LoKi::Cuts::MCQUARK( LHCb::ParticleID::charm ) ;
    MCCut containsS = LoKi::Cuts::MCQUARK( LHCb::ParticleID::strange ) ;
    //  MCCut containsSV02 = ((LoKi::Cuts::MCABSID) == 3122);
    MCCut isFromB = LoKi::Cuts::MCINANCESTORS( containsB && HADRON )  ; // the hadron contains b
    MCCut isFromC = LoKi::Cuts::MCINANCESTORS( containsC && !isFromB && HADRON ); // the hadron contains c but no b
    MCCut isFromS = LoKi::Cuts::MCINANCESTORS( containsS && !isFromC && !isFromB && HADRON ); // the hadron contains s but no c nor b
    //  MCCut isFromV0 = LoKi::Cuts::MCINANCESTORS( ((LoKi::Cuts::MCABSID) == 310) || ((LoKi::Cuts::MCABSID) == 3122) ) ;
    //MCCut isFromb = LoKi::Cuts::MCNINTREE( LoKi::Cuts::MCABSID == "b" , HepMC::ancestors ) > 0 ;
    //MCCut isFromc = LoKi::Cuts::MCNINTREE( LoKi::Cuts::MCABSID == "c" , HepMC::ancestors ) > 0 ;
    //MCCut isFroms = LoKi::Cuts::MCNINTREE( LoKi::Cuts::MCABSID == "s" , HepMC::ancestors ) > 0 ;
    MCCut isFromMother = LoKi::Cuts::MCINANCESTORS( m_cutForMotherMC  ) ;
    // Some map to ease the jet 2 MCParticle relation table making
    std::map< int ,const LHCb::MCParticle* > mapBarCode2MCPs ;
    // Get the HepMCEvents









    double tote = 0;
    // Loop over all MCs
    for (mcIt = mcparts->begin();mcIt!=mcparts->end();mcIt++){

        const LHCb::MCParticle *mcparticle = (*mcIt);
        //remove neutrino is requested
        if(m_rejectNeutrinos)
            if(std::find(m_listofNeutrinos.begin(), m_listofNeutrinos.end(), mcparticle->particleID().abspid()) != m_listofNeutrinos.end())
                continue;
        // Check if the particle come from a decay of a particle from the PV
        if(isFromPV(mcparticle)){
            const SmartRefVector< LHCb::MCVertex >  evs = mcparticle->endVertices();


            if ( m_cutForMotherMC(mcparticle) ){
                mothers.push_back(mcparticle);
                LHCb::Particle* newPart = new LHCb::Particle();
                newPart->setMomentum(mcparticle->momentum());
                LHCb::ParticleID pid;
                pid.setPid(MCID(mcparticle));
                newPart->setParticleID(pid);
                if ((mcparticle)->primaryVertex () != NULL){

                    Gaudi::XYZPoint p_vertexPosition ((mcparticle)->primaryVertex()->position());
                    newPart->setReferencePoint (p_vertexPosition);
                }
                if(((mcparticle)->endVertices()).size() > 0){
                    SmartRefVector< LHCb::MCVertex >::const_iterator itv;
                    LHCb::Vertex  vJet;
                    Gaudi::XYZPoint e_vertexPosition (0.,0.,0.);
                    for(itv = evs.begin(); itv!= evs.end(); itv++){
                        if (((*itv).target()->type()  == 2) || ((*itv).target()->type()  == 3)){
                            Gaudi::XYZPoint e_vertexPosition ((*itv).target()->position());
                            break;
                        }
                    }
                    vJet.setPosition( e_vertexPosition );
                    newPart->setEndVertex(vJet.clone());
                }
                newPart->addInfo ( GBarCode , (mcparticle)->key() );
                jetParticles->insert(newPart);


            }


            bool isGenerator = false;
            // If is a long lived or do not has a decay at all, particle is kept
            if (evs.size()==0 || isALongLivedDecay( mcparticle )){
                isGenerator = true;
            }
            // Otherwise only keep if its end end vwertex is not a decay
            else{
                bool Decay = false;
                SmartRefVector< LHCb::MCVertex >::const_iterator itv;
                for(itv = evs.begin(); itv!= evs.end(); itv++){
                    if ( ((*itv).target()->type() != 1) &&  ((*itv).target()->type() != 2) &&  ((*itv).target()->type() != 3) &&  ((*itv).target()->type() != 4) ){
                        isGenerator = true;
                    }
                    if (((*itv).target()->type()  == 2) || ((*itv).target()->type()  == 3)){
                        Decay = true;
                    }
                }
                if(Decay){
                    isGenerator = false;
                }
            }
            // Append the particle to the list.
            if (isGenerator){
                tote += mcparticle->momentum().E();
                StableParts.push_back(mcparticle);
            }
        }
    }


   // std::cout<<"tot e :"<<tote<<std::endl;
   // std::cout<<"tot e per PV: "<<tote/((double) primaries.size())<<std::endl;
    double totee = 0;

    std::vector<const LHCb::MCVertex*>::iterator itr;
    for ( itr = primaries.begin(); itr != primaries.end(); ++itr ){
        const LHCb::MCVertex* primary = *itr;

        Gaudi::XYZPoint vertexPosition(primary->position().X(), primary->position().Y(),primary->position().Z());
        //    int processID = event->signal_process_id ();
        // Get the list of good particles and make jets out of it
        IJetMaker::Input inputs;



        for ( LHCb::MCParticle::ConstVector::iterator p = StableParts.begin ()  ;  StableParts.end () != p ; ++p ){
            if((*p)->primaryVertex()->key() != primary->key()) continue;
            if ( !cut(*p) )continue;
            LHCb::Particle* newPart = new LHCb::Particle();
            newPart->setMomentum((*p)->momentum());
            LHCb::ParticleID pid;
            pid.setPid(MCID(*p));
            newPart->setParticleID(pid);
            totee+=newPart->momentum().E();
            Gaudi::XYZPoint p_vertexPosition (primary->position());
            newPart->setReferencePoint (p_vertexPosition);
            //       newPart->addInfo ( ProcessID , processID );
            newPart->addInfo ( HasS , isFromS(*p) );
            newPart->addInfo ( HasC , isFromC(*p) );
            newPart->addInfo ( HasB , isFromB(*p) );
            //newPart->addInfo ( HasV0, isFromV0(*p) );
            //newPart->addInfo ( Hass , isFroms(*p) );
            //newPart->addInfo ( Hasc , isFromc(*p) );
            //newPart->addInfo ( Hasb , isFromb(*p) );
            newPart->addInfo ( FromMother , isFromMother(*p) );
            newPart->addInfo ( GBarCode , (*p)->key() );

            if ( !m_outputTableMC.empty() ){
                //LHCb::HepMC2MC2D::Range range = tableHepMC2MC->relations(*p);
                //if (range.size()>0){
                //LHCb::MCParticle*  mcp = range[0];
                mapBarCode2MCPs[(*p)->key()]=  (*p) ;
            }
            // }
            //int motherBarCode(-1),motherID(0);
            //for(std::vector< const HepMC::GenParticle* >::const_iterator imother = mothers.begin();
            //mothers.end()!=imother ; ++ imother){
            //GCut isFromthisMother = GFROMTREE(*imother);
            //if (isFromthisMother(*p)){
            //motherBarCode = GBAR(*imother);
            //motherID = GID(*imother);
            // }
            // }
            // newPart->addInfo ( GMotherBarCode , motherBarCode  );
            // newPart->addInfo ( MotherID , motherID );
            if(isALongLivedDecay(*p)){
                if((*p)->endVertices().size() != 0){
                    const SmartRefVector<LHCb::MCVertex>& decays = (*p)->endVertices();
                    SmartRefVector<LHCb::MCVertex>::const_iterator ivtx = decays.begin();

                    LHCb::Vertex* endV = new LHCb::Vertex((*ivtx)->position());
                    newPart->setEndVertex(endV);

                }
            }
            daughterParticles->insert(newPart);
            inputs.push_back(newPart);
        }
        // Now make the jets
        IJetMaker::Jets jets ;
        // make the jets
        StatusCode sc = m_maker->makeJets ( inputs.begin () , inputs.end   () , jets  ) ;
        if ( sc.isFailure() ) { return Error ( "Error from jet maker" , sc ) ; }
        // save all jets
        std::vector <int> usedBarCode ;
        while ( !jets.empty() )
        {
            LHCb::Particle* jet = jets.back() ;
            jet->setReferencePoint( vertexPosition );
            LHCb::Vertex  vJet;
            vJet.setPosition( vertexPosition );
            vJet.setOutgoingParticles(jet->daughters());
            jet->setEndVertex(vJet.clone());
            this->appendJetIDInfo(jet);
            if (m_jetsFromMotherOnly && jet->info(FromMother,-1.)<0.){
                jets.pop_back() ;
                delete jet ;
            }
            // store the hepmc particles that are used
            for ( SmartRefVector< LHCb::Particle >::const_iterator ip = jet->daughters().begin();jet->daughters().end()!=ip;++ip ){
                usedBarCode.push_back((int)(*ip)->info(GBarCode,-1.));

            }
            // if requested, store the MCParticles used in this jet in a relation table
            double totE(0.);
            if ( !m_outputTableMC.empty() ){

                for ( SmartRefVector< LHCb::Particle >::const_iterator ip = jet->daughters().begin();jet->daughters().end()!=ip;++ip ){
                    if (! mapBarCode2MCPs[int((*ip)->info(GBarCode,-1.))])continue;
                    totE+=MCE(mapBarCode2MCPs[int((*ip)->info(GBarCode,-1.))]);
                    table -> relate(jet , mapBarCode2MCPs[int((*ip)->info(GBarCode,-1.))] ); // relie le jet a chaque MCParticle qui correspond a ses filles.
                }
            }

            jetParticles->insert(jet);
            //this->markTree( jet );
            jets.pop_back() ;
            //delete jet ;
        }
     //   std::cout<<"tot ee :"<<totee<<std::endl;
        /*while ( !inputs.empty())
          {
          LHCb::Particle* p = inputs.back() ;
          bool used = false;
          for ( int i = 0 ; i < (int)usedBarCode.size(); ++i){
          if ((int)p->info(GBarCode,-1.) == usedBarCode[i]){
          used = true;
          break;
          }
          }
          if (used) daughterParticles->insert(p); //this->markTree( p );
          inputs.pop_back();
          delete( p );

          }*/
    }
    if ( statPrint() || msgLevel ( MSG::DEBUG ) )
    { counter ( "#jets" ) += selected ("jets").size() ; }

    if ( m_outputTableMC.empty() ){
        delete(table);
    }
    setFilterPassed ( true ) ;

    return StatusCode::SUCCESS ;
}

StatusCode LoKi::MCJetMaker::appendJetIDInfo( LHCb::Particle* jet )
{
    using namespace LoKi        ;
    using namespace LoKi::Types ;
    using namespace LoKi::Cuts  ;

    // Get Jet Daughters
    std::vector<const LHCb::Particle *> daughtersvector = jet->daughtersVector();
    std::vector<const LHCb::Particle *>::iterator idaughter = daughtersvector.begin();

    double mtf;    /// Highest pT track / Jet pT
    double cpf;    /// charged pT fraction - V0s are not included
    double width;  /// jet width
    int    n90;    /// Number of items responsible for at least 90% of the jet momentum
    int    ntrk;   /// Number of tracks

    float auxptmax=-1, sumpt=0; int iitems=0;
    double tpx=0, tpy=0;
    std::vector<float> itemspt;
    ntrk=n90=width=0;
    std::map<int,double> barCodeMotherFraction;

    for (;idaughter != daughtersvector.end() ; ++idaughter){
        const LHCb::Particle * daughter = *idaughter;
        barCodeMotherFraction[(int)daughter->info(GMotherBarCode,-1.)]=E(daughter)/E(jet);
        if(daughter->particleID().threeCharge()!=0) {
            ntrk++; auxptmax = daughter->momentum().Pt() > auxptmax ? daughter->momentum().Pt() : auxptmax;
            tpx+=daughter->momentum().Px(); tpy+=daughter->momentum().Py();
        }
        iitems++; float pt = daughter->momentum().Pt(); sumpt+=pt;
        itemspt.push_back(pt);
        for(int ii=0; ii<iitems; ii++) if(itemspt[ii]<pt) {
            float aux = itemspt[ii]; itemspt[ii]=pt; pt = aux;}
        width += ROOT::Math::VectorUtil::DeltaR(daughter->momentum(),jet->momentum()) * daughter->momentum().Pt();
    }

    mtf = auxptmax / jet->momentum().Pt(); mtf = 0 > mtf ? 0 : mtf; mtf = 1 < mtf ? 1 : mtf;
    cpf = TMath::Sqrt(tpx*tpx+tpy*tpy)/jet->momentum().Pt();
    width /= sumpt;

    sort (itemspt.begin(), itemspt.end());
    float auxptsum = 0; n90=0;
    for(int ii=iitems-1; ii>=0; ii--) {auxptsum+=itemspt[ii]; n90++; if(auxptsum/sumpt>0.9) break; }

    jet->addInfo ( Ntracks , ntrk );
    jet->addInfo ( N90 , n90 );
    jet->addInfo ( MTF , mtf );
    jet->addInfo ( CPF , cpf );
    jet->addInfo ( JetWidth , width );


    LoKi::Types::Cut isLongLived = (ABSID == 3222 || ABSID == 310 || ABSID == 3122 || ABSID == 3112 || ABSID == 3312 || ABSID == 3322);

    LoKi::Types::Fun isV0 =  LoKi::Cuts::INFO( HasV0 , -10.);
    LoKi::Types::Fun FractionChargedHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ( ( isV0 < 0.0001 ) && ( ABSID == "pi+" || ABSID == "K+" || ABSID == "p+" ) ) , 0. );
    jet->addInfo ( ChargedHadron  , FractionChargedHadron(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionMuon = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ABSID == "mu+" , 0. );
    jet->addInfo ( Muon  , FractionMuon(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionElectron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,  ABSID == "e+" , 0. );
    jet->addInfo ( Electron  , FractionElectron(jet)/LoKi::Cuts::E(jet) );


    LoKi::Types::Fun FractionCharged = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ( ABSID == "pi+" || ABSID == "K+" || ABSID == "p+" || ABSID == "mu+" || ABSID == "e+" ) , 0. );
    jet->addInfo ( Charged  , FractionCharged(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ABSID == "gamma" , 0. );
    jet->addInfo (  Photon , FractionPhoton(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ABSID == "pi0" , 0. );
    jet->addInfo ( Pi0  , FractionPi0(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionNeutralHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,( ABSID == "KL0" || ABSID == "n0")  , 0. );
    jet->addInfo (  NeutralHadron , FractionNeutralHadron(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionNeutral =  LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ( ABSID == "gamma" || ABSID == "KL0" || ABSID == "n0" || ABSID == "pi0" ) , 0. ); ;
    jet->addInfo ( Neutral  , FractionNeutral(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun FractionV0AfterTT = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,HASVERTEX && isLongLived && VFASPF(VZ)>2200, 0. );
    LoKi::Types::Fun FractionV0BeforeTT = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,HASVERTEX && isLongLived && VFASPF(VZ)<=2200, 0. );
    LoKi::Types::Fun FractionV0NoVertex = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,!HASVERTEX && isLongLived, 0. );
    LoKi::Types::Fun FractionV0  = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,isLongLived, 0. );
    jet->addInfo ( HasV0 , FractionV0(jet)/LoKi::Cuts::E(jet)  );
    jet->addInfo ( V0  , FractionV0(jet)/LoKi::Cuts::E(jet) );
    jet->addInfo ( LLBeforeTT  , FractionV0BeforeTT(jet)/LoKi::Cuts::E(jet) );
    jet->addInfo ( LLAfterTT  , FractionV0AfterTT(jet)/LoKi::Cuts::E(jet) );
    jet->addInfo ( LLNoVertex  , FractionV0NoVertex(jet)/LoKi::Cuts::E(jet) );
    jet->addInfo ( V0  , FractionV0(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun isC =  LoKi::Cuts::INFO( HasC , -10.);
    LoKi::Types::Fun FractionD = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isC > 0. , 0. );
    jet->addInfo ( HasC , FractionD(jet)/LoKi::Cuts::E(jet)  );
    jet->addInfo ( D  , FractionD(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun isB =  LoKi::Cuts::INFO( HasB , -10.);
    LoKi::Types::Fun FractionB = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isB > 0. , 0. );
    jet->addInfo ( HasB , FractionB(jet)/LoKi::Cuts::E(jet)  );
    jet->addInfo ( B  , FractionB(jet)/LoKi::Cuts::E(jet) );
    LoKi::Types::Fun FractionComposite = FractionV0 + FractionD + FractionB ;
    jet->addInfo ( Composite  , FractionComposite(jet)/LoKi::Cuts::E(jet) );

    LoKi::Types::Fun isS =  LoKi::Cuts::INFO( HasS , -10.);
    LoKi::Types::Fun FractionS = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isS > 0. , 0. );
    jet->addInfo ( HasS , FractionS(jet)/LoKi::Cuts::E(jet)  );

    LoKi::Types::Fun iss =  LoKi::Cuts::INFO( Hass , -10.);
    LoKi::Types::Fun Fractions = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , iss > 0. , 0. );
    jet->addInfo ( Hass , Fractions(jet)/LoKi::Cuts::E(jet)  );
    LoKi::Types::Fun isc =  LoKi::Cuts::INFO( Hasc , -10.);
    LoKi::Types::Fun Fractionc = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isc > 0. , 0. );
    jet->addInfo ( Hasc , Fractionc(jet)/LoKi::Cuts::E(jet)  );
    LoKi::Types::Fun isb =  LoKi::Cuts::INFO( Hasb , -10.);
    LoKi::Types::Fun Fractionb = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isb > 0. , 0. );
    jet->addInfo ( Hasb , Fractionb(jet)/LoKi::Cuts::E(jet)  );

    LoKi::Types::Fun isFromMother =  LoKi::Cuts::INFO( FromMother , -10.);
    LoKi::Types::Fun FractionMother = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isFromMother > 0. , 0. );
    jet->addInfo ( FromMother , FractionMother(jet)/LoKi::Cuts::E(jet)  );

    //fraction of mother from a given barcode...
    int maxMotherBar(-1);
    double maxMotherFraction(0.),otherMotherFraction(0.);
    for (std::map<int,double>::iterator i_motherBar = barCodeMotherFraction.begin();
            barCodeMotherFraction.end()!=i_motherBar;
            ++i_motherBar)
    {
        otherMotherFraction = (*i_motherBar).second;
        if ((*i_motherBar).second>maxMotherFraction){
            maxMotherBar=(*i_motherBar).first;
            otherMotherFraction -= (*i_motherBar).second + maxMotherFraction;
            maxMotherFraction = (*i_motherBar).second;
        }
    }
    jet->addInfo ( GMotherBarCode , maxMotherBar  );
    jet->addInfo ( FracOfOtherMother , otherMotherFraction  );

    return SUCCESS;
}







bool  LoKi::MCJetMaker::isALongLivedDecay( const LHCb::MCParticle *mcparticle ){
    if(std::find( m_listOfLongLived.begin(),  m_listOfLongLived.end(), mcparticle->particleID().abspid()) !=  m_listOfLongLived.end()) {
        return true;
    } else {
        return false;
    }
}


bool LoKi::MCJetMaker::isFromPV( const LHCb::MCParticle *mcparticle ){
    const LHCb::MCVertex * ov = mcparticle->originVertex();
    if (ov->isPrimary())
        return true;
    if ((ov->type() != 1) && (ov->type() != 2) && (ov->type() != 3) && (ov->type() != 4))
        return false;
    if(std::find(m_listOfLongLived.begin(), m_listOfLongLived.end(), mcparticle->mother()->particleID().abspid()) != m_listOfLongLived.end()) {
        return false;
    }
    return isFromPV( ov->mother() );
}







// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,MCJetMaker)
    // ============================================================================
    // The END
    // ============================================================================
