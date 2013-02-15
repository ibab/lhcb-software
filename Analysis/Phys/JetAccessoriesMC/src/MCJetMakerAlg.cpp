// $Id: LoKiJetMakerAlg.cpp,v 1.6 2010-01-11 08:37:42 cocov Exp $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "LoKi/IGenHybridFactory.h"
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

#include "LoKi/GenParticleCuts.h" 
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
      , m_codeForMother( "GNONE"                )
      , m_codeForBanned( "GNONE"                )
      , m_preambulo    (                        )
      , m_factory      ( "LoKi::Hybrid::GenTool/GenFactory:PUBLIC" )
      , m_cutForMother ( _PBOOL ( false )       )
      , m_cutForBanned ( _PBOOL ( false )       )
      , m_simpleAcceptance ( false )
      , m_outputTable ("Relations/Phys/R2HepMC")
    { 
      // 
      declareProperty  ( "JetMaker"  , 
			 m_makerName , 
			 "Type type/name of jet-maker tool (IJetMaker interface)") ;  
      declareProperty  ( "MCParticleTypes"  , 
			 m_inputTypes  ,
			 "Type of particles to consider");
      declareProperty  ( "CodeForMotherSelection" ,
			 m_codeForMother ,
			 "The functor for mother selection" );
      declareProperty  ( "CodeForBannedSelection" ,
			 m_codeForBanned ,
			 "The functor for selecting the particle from which to ban the decendant" );
      declareProperty  ( "SimpleAcceptance" ,
			 m_simpleAcceptance ,
			 "A simple theta cut on the acceptance" );
      declareProperty  ( "OutputTable" ,
			 m_outputTable ,
			 "Location of the " );
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
    std::string preambulo ( ) const;
    

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
    std::string m_codeForMother; 
    /// Code for the predicate for the selection of Banned particles
    std::string m_codeForBanned;
    /// the preambulo 
    std::vector<std::string> m_preambulo ; // the preambulo 
    /// LoKi/Bender "hybrid" factory name 
    std::string m_factory ; // LoKi/Bender "hybrid" factory name 
    /// the predicate for the selection of Mother 
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate m_cutForMother ;
    /// the predicate for the selection of banned particles
    LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate m_cutForBanned ;
    /// apply a simple acceptance cut
    bool m_simpleAcceptance ;
    /// Location of the HepMC to LHCb::Particle table
    std::string m_outputTable;
    
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
  // Give some default values to the types if not defined
  if (m_inputTypes.empty()){
    m_inputTypes.push_back("pi+");
    m_inputTypes.push_back("K+");
    m_inputTypes.push_back("p+");
    m_inputTypes.push_back("e+");
    m_inputTypes.push_back("mu+");
    m_inputTypes.push_back("pi0");
    m_inputTypes.push_back("gamma");
    m_inputTypes.push_back("KL0");
    m_inputTypes.push_back("n0");
    //m_inputTypes.push_back("nu");
    //m_inputTypes.push_back("nu_mu");
    //m_inputTypes.push_back("nu_tau");
  }
  

  return StatusCode::SUCCESS ;
}

// ============================================================================
// construct the preambulo string
// ============================================================================
std::string LoKi::MCJetMaker::preambulo() const
{
  // Put some default to the preambulo so that it is not necessary to specify each time
  std::string result = "from LoKiGen.decorators import *\nfrom LoKiNumbers.decorators import *\nfrom LoKiCore.functions import *\nfrom LoKiCore.math import *";

  for ( std::vector<std::string>::const_iterator iline =
          m_preambulo.begin() ; m_preambulo.end() != iline ; ++iline )
  {
    if ( m_preambulo.begin() != iline ) { result += "\n" ; }
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
  LoKi::IGenHybridFactory* factory = tool<LoKi::IGenHybridFactory> ( m_factory , this ) ;

  // release the factory (not needed anymore)
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;
}


// ============================================================================
// decode the code
// ============================================================================
StatusCode LoKi::MCJetMaker::decodeCode ()
{
  // locate the factory
  LoKi::IGenHybridFactory* factory_ = tool<LoKi::IGenHybridFactory> ( m_factory , this ) ;
  //
  // use the factory
  StatusCode sc = factory_ -> get ( m_codeForMother , m_cutForMother , preambulo() ) ;
  if ( sc.isFailure() )
  { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='"
                   + m_codeForMother + "'" , sc )  ; }
  sc = factory_ -> get ( m_codeForBanned , m_cutForBanned , preambulo() ) ;
  if ( sc.isFailure() )
  { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='"
                   + m_codeForBanned + "'" , sc )  ; }
  //
  release ( factory_ ) ;
  //
  return sc ;
}

StatusCode LoKi::MCJetMaker::analyse   () 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts ; 

  // Create the relation table between Particles and GenParticles
  /*typedef LHCb::Relation2D<LHCb::Particle,HepMC::GenParticle*> Table ;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::RC2HepMC2D));
  Table* table = 0 ;
  if ( !m_outputTable.empty() ){
    table = new Table(100) ;
    put ( table , m_outputTable ) ;
  }
  */
  typedef LoKi::GenTypes::GenContainer Container  ;
  typedef LoKi::GenTypes::GenVContainer VContainer;

  // Initialize the tool
  if ( 0 == m_maker ) { m_maker = tool<IJetMaker> ( m_makerName ,m_makerName, this ) ; }
  
  // Give some default values to the types if not defined
  if (m_inputTypes.empty()){
    m_inputTypes.push_back("pi+");
    m_inputTypes.push_back("K+");
    m_inputTypes.push_back("p+");
    m_inputTypes.push_back("e+");
    m_inputTypes.push_back("mu+");
    m_inputTypes.push_back("pi0");
    m_inputTypes.push_back("gamma");
    m_inputTypes.push_back("KL0");
    m_inputTypes.push_back("n0");
    m_inputTypes.push_back("nu");
    m_inputTypes.push_back("nu_mu");
    m_inputTypes.push_back("nu_tau");
  }
  
  // PID and stable particles cut
  //GCut isNotFromBannedMother = LoKi::Cuts::GNINTREE( LoKi::Cuts::GABSID == m_motherIDToBan , HepMC::parent ) < 1 ;
  LoKi::Types::GCut cut = GVALID ;
  for ( std::vector< std::string >::iterator pid = m_inputTypes.begin() ; m_inputTypes.end() != pid ; ++ pid ){
    if( pid == m_inputTypes.begin()) cut =  ( (*pid) == LoKi::Cuts::GABSID );
    else cut =  ( cut || ( (*pid) == LoKi::Cuts::GABSID ) );
  }
  if ( m_simpleAcceptance ) cut = cut && ( LoKi::Cuts::GTHETA < 0.4 );
  GCut isToBan = ( LoKi::Cuts::GNINTREE( m_cutForBanned , HepMC::ancestors ) > 0 ) || ( m_cutForBanned );
  cut = cut && ( ( LoKi::Cuts::GSTATUS == 1 ) || ( LoKi::Cuts::GSTATUS == 999 ) ) && ( !isToBan );
  GCut containsB = LoKi::Cuts::GQUARK( LHCb::ParticleID::bottom ) ;
  GCut containsC = LoKi::Cuts::GQUARK( LHCb::ParticleID::charm ) ;
  GCut containsS = LoKi::Cuts::GQUARK( LHCb::ParticleID::strange ) ;
  GCut isFromB = LoKi::Cuts::GNINTREE( ( containsB && GHADRON ) ,  HepMC::ancestors ) > 0 ; // the hadron contains b
  GCut isFromC = LoKi::Cuts::GNINTREE( ( containsC && !containsB && GHADRON ) , HepMC::ancestors ) > 0 ; // the hadron contains c but no b
  GCut isFromS = LoKi::Cuts::GNINTREE( ( containsS && !containsC && !containsB && GHADRON ) , HepMC::ancestors ) > 0 ; // the hadron contains s but no c nor b
  GCut isFromV0 = LoKi::Cuts::GNINTREE( ( LoKi::Cuts::GABSID == 310 || LoKi::Cuts::GABSID == 3122 ) , HepMC::ancestors ) > 0 ;
  GCut isFromb = LoKi::Cuts::GNINTREE( LoKi::Cuts::GABSID == "b" , HepMC::ancestors ) > 0 ;
  GCut isFromc = LoKi::Cuts::GNINTREE( LoKi::Cuts::GABSID == "c" , HepMC::ancestors ) > 0 ;
  GCut isFroms = LoKi::Cuts::GNINTREE( LoKi::Cuts::GABSID == "s" , HepMC::ancestors ) > 0 ;
  GCut isFromMother = LoKi::Cuts::GNINTREE( m_cutForMother , HepMC::ancestors ) > 0 ;
  
  // Get the HepMCEvents
  const LHCb::HepMCEvent::Container* events = get<LHCb::HepMCEvent::Container> (  LHCb::HepMCEventLocation::Default ) ;
  std::vector< std::vector< const LHCb::Particle * > > particles;
  std::vector< const HepMC::GenVertex *> vertices;
  for ( LHCb::HepMCEvent::Container::const_iterator i_event = events->begin() ; events->end() != i_event ; ++i_event ){
    const HepMC::GenEvent* event = ( *i_event )->pGenEvt () ;
    // Get the signal process vertex
    std::pair< HepMC::GenParticle *, HepMC::GenParticle * > beamPart = event->beam_particles( );
    const HepMC::GenVertex * primary = beamPart.first->end_vertex ();
    Gaudi::XYZPoint vertexPosition(primary->point3d().x(), primary->point3d().y(),primary->point3d().z());
    int processID = event->signal_process_id ();
    // Get the list of good particles and make jets out of it
    IJetMaker::Input inputs;
    std::vector< const HepMC::GenParticle* > mothers;
    // Get the mothers
    for ( HepMC::GenEvent::particle_const_iterator p = event->particles_begin ()  ;  event->particles_end () != p ; ++p ){
      if ( !m_cutForMother(*p) ) continue;
      mothers.push_back(*p);
      LHCb::Particle* newPart = new LHCb::Particle();
      Gaudi::LorentzVector mom(GPX(*p),GPY(*p),GPZ(*p),GE(*p));
      newPart->setMomentum(mom);
      LHCb::ParticleID pid;
      pid.setPid(GID(*p));
      newPart->setParticleID(pid);
      if ((*p)->production_vertex ()){
	Gaudi::XYZPoint p_vertexPosition ((*p)->production_vertex ()->point3d().x(),(*p)->production_vertex ()->point3d().y(),(*p)->production_vertex ()->point3d().z());
	newPart->setReferencePoint (p_vertexPosition);
      }
      if((*p)->end_vertex ()){
	Gaudi::XYZPoint e_vertexPosition ((*p)->end_vertex ()->point3d().x(),(*p)->end_vertex ()->point3d().y(),(*p)->end_vertex ()->point3d().z());
	LHCb::Vertex  vJet;
	vJet.setPosition( e_vertexPosition );
	newPart->setEndVertex(vJet.clone());
      }
      newPart->addInfo ( ProcessID , processID );
      newPart->addInfo ( GBarCode , GBAR(*p) );
      this->markTree(newPart);
    }
    for ( HepMC::GenEvent::particle_const_iterator p = event->particles_begin ()  ;  event->particles_end () != p ; ++p ){
      if ( !cut(*p) )continue;
      LHCb::Particle* newPart = new LHCb::Particle();
      Gaudi::LorentzVector mom(GPX(*p),GPY(*p),GPZ(*p),GE(*p));
      newPart->setMomentum(mom);
      LHCb::ParticleID pid;
      pid.setPid(GID(*p));
      newPart->setParticleID(pid);
      Gaudi::XYZPoint p_vertexPosition ((*p)->production_vertex ()->point3d().x(),(*p)->production_vertex ()->point3d().y(),(*p)->production_vertex ()->point3d().z());
      newPart->setReferencePoint (p_vertexPosition);
      newPart->addInfo ( ProcessID , processID );
      newPart->addInfo ( HasS , isFromS(*p) );
      newPart->addInfo ( HasC , isFromC(*p) );
      newPart->addInfo ( HasB , isFromB(*p) );
      newPart->addInfo ( HasV0, isFromV0(*p) );
      newPart->addInfo ( Hass , isFroms(*p) );
      newPart->addInfo ( Hasc , isFromc(*p) );
      newPart->addInfo ( Hasb , isFromb(*p) );
      newPart->addInfo ( FromMother , isFromMother(*p) );
      newPart->addInfo ( GBarCode , GBAR(*p) );
      int motherBarCode(-1),motherID(0);
      for(std::vector< const HepMC::GenParticle* >::const_iterator imother = mothers.begin();
	  mothers.end()!=imother ; ++ imother){
	GCut isFromthisMother = GFROMTREE(*imother);
	if (isFromthisMother(*p)){
	  motherBarCode = GBAR(*imother);
	  motherID = GID(*imother);
	}
      }
      newPart->addInfo ( GMotherBarCode , motherBarCode  );
      newPart->addInfo ( MotherID , motherID );
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
      for ( SmartRefVector< LHCb::Particle >::const_iterator ip = jet->daughters().begin();jet->daughters().end()!=ip;++ip ){
	usedBarCode.push_back((int)(*ip)->info(GBarCode,-1.));
      }
      vJet.setPosition( vertexPosition );
      vJet.setOutgoingParticles(jet->daughters());
      jet->setEndVertex(vJet.clone());
      this->appendJetIDInfo(jet);
      this->markTree( jet );
      jets.pop_back() ;
      delete jet ;
    }
    while ( !inputs.empty())
    {
      const LHCb::Particle* p = inputs.back() ;
      bool used = false;
      for ( int i = 0 ; i < (int)usedBarCode.size(); ++i){
	if ((int)p->info(GBarCode,-1.) == usedBarCode[i]){
	  used = true;
	  break;
	}
      }
      if (used) this->markTree( p );
      inputs.pop_back();
      delete( p );
      
    }
  }
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#jets" ) += selected ("jets").size() ; }

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

  LoKi::Types::Fun isV0 =  LoKi::Cuts::INFO( HasV0 , -10.);
  LoKi::Types::Fun FractionChargedHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ( ABSID == "pi+" || ABSID == "K+" || ABSID == "p+" ) && ( isV0 < 0. ) , 0. );
  jet->addInfo ( ChargedHadron  , FractionChargedHadron(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionMuon = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ABSID == "mu+" , 0. );
  jet->addInfo ( Muon  , FractionMuon(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionElectron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,  ABSID == "e+" , 0. );
  jet->addInfo ( Electron  , FractionElectron(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionCharged = FractionChargedHadron + FractionMuon + FractionElectron;
  jet->addInfo ( Charged  , FractionCharged(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionPhoton = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ABSID == "gamma" , 0. );
  jet->addInfo (  Photon , FractionPhoton(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionPi0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , ABSID == "pi0" , 0. );
  jet->addInfo ( Pi0  , FractionPi0(jet)/LoKi::Cuts::E(jet) );
  LoKi::Types::Fun FractionNeutralHadron = LoKi::Cuts::SUMTREE( LoKi::Cuts::E ,( ABSID == "KL0" || ABSID == "n0")  , 0. );
  jet->addInfo (  NeutralHadron , FractionNeutralHadron(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionNeutral = FractionPhoton + FractionPi0 +  FractionNeutralHadron ;
  jet->addInfo ( Neutral  , FractionNeutral(jet)/LoKi::Cuts::E(jet) );

  LoKi::Types::Fun FractionV0 = LoKi::Cuts::SUMTREE( LoKi::Cuts::E , isV0 > 0. , 0. );
  jet->addInfo ( HasV0 , FractionV0(jet)/LoKi::Cuts::E(jet)  );
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

/*
StatusCode LoKi::MCJetMaker::associateJet( LHCb::Particle* jet )
{
  LoKi::Types::GCut cut = GNONE ;
  // Create a cut coresponding to the HepMC particles bar codes in this jet
  for ( SmartRefVector< LHCb::Particle >::const_iterator ip = jet->daughters().begin();jet->daughters().end()!=ip;++ip ){
    cut = ( cut || ( (*ip)->info(GBarCode,-1.) == GBAR ) );
  }
  // Get all the HepMC particles of this jet
  LoKi::GenTypes::GenContainer TheHepMCVect = gselect("theJetdaug","theJetdaug",cut );
  // The cut which is true for every MC particles from those HepMC
  Cut StdMC_fromGen = GMCTRUTH ( TheHepMCVect , tableHepMC2MC , mc ) ;
      
  //Loop over the StdHepMC Particles of the HepMCjet
  for(Parts::iterator idaug_primjet = .begin() ; daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
  {
      Table2DHepMC2Part::Range links = table_togetHepMC ->relations (*idaug_primjet ) ;
      for ( Table2DHepMC2Part::iterator ilink = links.begin() ;links.end() != ilink ; ++ilink )
        {
          const HepMC::GenParticle* gp  = ilink->to() ;
          if ( 0 == gp ) { continue ;}
          TheHepMCVect.push_back(gp);
        }
      }
      Cut StdMC_fromGen = GMCTRUTH ( TheHepMCVect , tableHepMC2MC , mc ) ;
*/
// ===========================================================================
/// The factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,MCJetMaker)
// ============================================================================
// The END 
// ============================================================================
