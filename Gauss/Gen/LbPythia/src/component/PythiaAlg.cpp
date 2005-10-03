// $Id: PythiaAlg.cpp,v 1.3 2005-10-03 10:34:23 robbep Exp $
// ------------------------------------------------------------- 
// GeneratorModules/PythiaAlg.cxx Description: Allows the user
// to generate Pythia events and store the result in the
// Transient Store.
//
// initial code by Ian Hinchliffe
//
// LHCb version: Witek Pokorski

// This class

#include "LbPythia/PythiaAlg.h"

#include <cstdlib>
#include <fstream>

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Kernel
#include "Kernel/ParticleID.h"

// Event
#include "Event/HepMCEvent.h"
#include "Event/HardInfo.h"

// HepMC
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"

// From EvtGen
#include "EvtGenBase/EvtPDL.hh"

// local
#include "Generators/StringParse.h"
#include "Generators/HepMCUtils.h"
#include "Generators/IEvtGenTool.h"

//-------------------------------
// calls to Fortran routines
#ifdef WIN32

extern "C" {
  void __stdcall INITPYBLOCK(int*, const char*, int);
  void __stdcall PYINIT(const char*,int, const char*,int, const char*,int,
                        double* );
  void __stdcall PYEVNT();
  void __stdcall PYSTAT(int*);
  void __stdcall PYLIST(int*);
  int  __stdcall LUNHEP(int*); // STDHEP routine for PYJETS->HEPEVT
  void __stdcall PYHEPC(int*);
  int  __stdcall PYCOMP(int*);
  void __stdcall SETBEAM(double*,double*,double*,double*,double*,double*);  
  int  __stdcall CONTAINSB( ) ;
  void __stdcall PYEDIT(int*);
  void __stdcall PYEXEC();
  void __stdcall PDFLIB_INIT() ;
  void __stdcall PDFLIB_END() ;
  void __stdcall SETUSERPROCESS( int * ) ;
}

#else

extern "C" {
  void initpyblock_(int*, const char*, int);
  void pyinit_(const char*, const char*, const char*, double* ,  int, 
               int, int );
  void pyevnt_();
  void pystat_(int*);
  void pylist_(int*);
  int  lunhep_(int*); // STDHEP routine for PYJETS->HEPEVT
  int  pycomp_(int*);
  void setbeam_(double*,double*,double*,double*,double*,double*);  
  int  containsb_( ) ;
  void pyedit_(int*);
  void pyexec_();
  void pdflib_init_() ;
  void pdflib_end_() ;
  void setuserprocess_( int * ) ;
}

#endif

static const AlgFactory<PythiaAlg>    Factory;
const IAlgFactory& PythiaAlgFactory = Factory;

//============================================================================
// Constructor
//============================================================================
PythiaAlg::PythiaAlg(const std::string& name, 
                     ISvcLocator* pSvcLocator): 
  GenBaseAlg(name,pSvcLocator),
  m_frame( "3MOM" )           ,
  m_beam( "p+" )              ,
  m_target( "p+" )            ,
  m_win( 0. )                 ,
  m_anglesmear( 0. )          ,
  m_eventlistlevel( -1 )      ,
  m_initlistlevel( 1 )        ,
  m_envval1( "" )             ,
  m_fortout( 0 )              ,
  m_pystatlevel( 1 )          ,
  m_events( 0 )               ,
  m_evtTool( 0 )              ,
  m_minBMass( 20 * TeV )      ,
  m_targetIdMinBMass( -1 )    ,
  m_obtainedSignalType( 0 )   ,
  m_signalBr( 0. )
{
  m_basePythiaCommandVector . push_back( "pysubs msel 0" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 11 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 12 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 13 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 28 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 53 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 68 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 91 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 92 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 93 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 94 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 95 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 86 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 87 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 88 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 89 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 106 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 107 1" ) ;
  m_basePythiaCommandVector . push_back( "pysubs msub 108 1" ) ;
  m_basePythiaCommandVector . push_back( "pypars mstp 2 2" ) ;
  m_basePythiaCommandVector . push_back( "pypars mstp 33 3" ) ;
  m_basePythiaCommandVector . push_back( "pypars mstp 128 2" ) ;
  m_basePythiaCommandVector . push_back( "pypars mstp 82 3" ) ;
  m_basePythiaCommandVector . push_back( "pypars mstp 52 2" ) ;
  m_basePythiaCommandVector . push_back( "pypars mstp 51 4032" ) ;
  m_basePythiaCommandVector . push_back( "pypars parp 82 3.47" ) ;
  m_basePythiaCommandVector . push_back( "pypars parp 89 14000" ) ;
  m_basePythiaCommandVector . push_back( "pypars parp 90 0.174" ) ;
  m_basePythiaCommandVector . push_back( "pydat1 parj 13 0.7625" ) ;
  m_basePythiaCommandVector . push_back( "pydat1 parj 14 0.320" ) ;
  m_basePythiaCommandVector . push_back( "pydat1 parj 15 0.033" ) ;
  m_basePythiaCommandVector . push_back( "pydat1 parj 16 0.099" ) ;
  m_basePythiaCommandVector . push_back( "pydat1 parj 17 0.165" ) ;
  declareProperty("BasePythiaCommand", m_basePythiaCommandVector );
  declareProperty("UserPythiaCommand", m_userPythiaCommandVector);
  declareProperty("BeamEnergy", m_pbeam = 7000.0);
  declareProperty("VerticalXAngle", m_vangle = 0.0);
  declareProperty("HorizontalXAngle", m_hangle = 285.0);
  declareProperty("Emittance", m_emit = 0.503);
  declareProperty("Beta", m_beta = 10.0);
  declareProperty("FixedTarget", m_fixed = false );
}

//===========================================================================
// Destructor
//===========================================================================
PythiaAlg::~PythiaAlg() { }

//===========================================================================
// Initialize PythiaAlg (before calling GenBase::initialize)
//===========================================================================
StatusCode PythiaAlg::preInitialize() {
  // Print PYTHIA Banner and values changed in PYGIVE and PYTHIA warnings
  // only in DEBUG output level 
  // suppress also PYTHIA pyinit printout
  if ( msgLevel( MSG::DEBUG ) ) {
    this -> pydat1().mstu( 12 ) = 1 ;
    this -> pydat1().mstu( 13 ) = 1 ;
    this -> pydat1().mstu( 25 ) = 1 ;
    this -> pypars().mstp( 122 ) = 1 ;
  } else {
    this -> pydat1().mstu( 12 ) = 0 ;
    this -> pydat1().mstu( 13 ) = 0 ;
    this -> pydat1().mstu( 25 ) = 0 ;
    this -> pypars().mstp( 122 ) = 0 ;
    m_initlistlevel = -1 ;
  }

  // Set User process to 0 for normal Pythia to be overriden for
  // specific generation
#ifdef WIN32
  int proc = 0 ;
  SETUSERPROCESS( &proc ) ;
#else
  int proc = 0 ;
  setuserprocess_ (&proc) ;
#endif  

  // Suppress pdflib output
  if ( ! msgLevel( MSG::DEBUG ) ) {
#ifdef WIN32
    PDFLIB_INIT() ;
#else
    pdflib_init_() ;
#endif
  }

  // Retrieve the EvtGen tool
  m_evtTool = tool< IEvtGenTool >( "EvtGenTool" ) ;
  
  // Override Event type number with the one given to EvtGenTool
  if ( m_evtTool->getEventType() != 0 ) m_evnType = m_evtTool->getEventType() ;

  // Override hadron ID list with the one given to EvtGenTool
  if ( ! m_evtTool->getBHadronsList().empty() ) 
    m_hadrons = m_evtTool->getBHadronsList() ;

  // Obtain Forced fragmentation information
  m_forcedFragmentation = m_evtTool -> forcedFragmentation ( ) ;

  if ( m_forcedFragmentation ) {
    debug() << endreq ;
    debug() << "*** Forced Fragmentation enabled ***" << endreq ;
    debug() << endreq ;
  }
  else {
    debug() << endreq ;
    debug() << "*** Forced Fragmentation disabled ***" << endreq ;
    debug() << endreq ;
  }

  // Obtain repeated hadronization information
  m_repHad = m_evtTool -> repeatedHadronization( ) ;
  
  if ( m_repHad ) {
    debug() << endreq ;
    debug() << "--- Repeated Hadronization enabled ---" << endreq ;
    debug() << endreq ;
  }
  else {
    debug() << endreq ;
    debug() << "--- Repeated Hadronization disabled ---" << endreq ;
    debug() << endreq ;
  }

  // Obtain inclusive production information
  m_binclus = m_evtTool -> inclusiveProduction ( ) ;

  // Retrieve Particle Property Svc
  m_ppSvc = svc< IParticlePropertySvc > ( "ParticlePropertySvc" , true ) ;

  // Determine the lightest hadron to generate
  // to be able to obtain it via EvtGen also (and not only Pythia)
  // For example for B** -> B pi or B_c -> B_s X
  ParticleProperty * partProp ;
  PIDs::iterator hi ;
  for ( hi = m_hadrons.begin ( ) ; m_hadrons.end ( ) != hi ; ++hi ) {
    partProp = m_ppSvc -> findByStdHepID( *hi ) ;
    if ( partProp ) 
      if ( ( partProp -> mass( ) - partProp -> maxWidth( ) ) < m_minBMass ) {
        m_minBMass = partProp -> mass( ) - partProp -> maxWidth ( ) ;
        m_targetIdMinBMass = abs( ( *hi ) ) ;
      }
  }
  
  if ( m_evtTool -> isSignalDecay( ) ) {
    // Check if the signal decay mode is also in the generic decay table
    if ( ! m_evtTool -> checkSignalPresence( ) ) {
      error() << "The decay mode defined in the user decay file" << endreq ;
      error() << "does not exist in the generic decay file."     << endreq ;
      error() << "Please correct DECAY.DEC file."                << endreq ;
    }
    // Obtain total branching fraction for the signal mode 
    m_signalBr = m_evtTool -> getSignalBr( ) ;
    info() << "-----------------------------------------------" << endreq ;
    info() << "The signal decay mode has a branching fraction " << endreq ;
    info() << "equal to " << m_signalBr << " in DECAY.DEC."     << endreq ;
    info() << "-----------------------------------------------" << endreq ;
  }
  return StatusCode::SUCCESS ;
}

//===========================================================================
// Initialization of Pythia parameters
//===========================================================================
StatusCode PythiaAlg::genInitialize() {
  // Initialise the listing output, parameter and decay data input streams
  //
  
  // set up the input parameters to pyinit: these can be changed by the user
  if ( m_fixed ) {
    m_win = m_pbeam;
  } else {
    m_win = 2 * m_pbeam;
  }

  double p1x,p1y,p1z,p2x,p2y,p2z;
  p1x = m_pbeam * sin(m_hangle*1.0e-6);
  p1y = m_pbeam * sin(m_vangle*1.0e-6);
  p1z = m_pbeam;
  if ( m_fixed ) {
    p2x = 0;
    p2y = 0;
    p2z = 0;
  } else {
    p2x = m_pbeam * sin(m_hangle*1.0e-6);
    p2y = m_pbeam * sin(m_vangle*1.0e-6);
    p2z = -m_pbeam;
  }

#ifdef WIN32
  SETBEAM (&p1x,&p1y,&p1z,&p2x,&p2y,&p2z);
#else
  setbeam_(&p1x,&p1y,&p1z,&p2x,&p2y,&p2z);
#endif  
  
  // if emittance specified
  if (m_emit>0.0) {
    this->pypars().mstp(171) = 1;  // new energy given for each event 
    this->pypars().mstp(172) = 1;  // event generated at requested energy
  }
  
  this->pydat3().mdme(4178,1) = -1; // like in egpyinit

  // set the value of angular smearing

  if ( 0.0 != m_beta ) m_anglesmear=1.0e6 * sqrt(m_emit*1.0e-9 / m_beta);
  else m_anglesmear=0.0;
  
  debug() << "Angular beam smearing: " << m_anglesmear << " microrad" 
         << endreq ;

  //
  // Make stable all particles known to EvtGen
  //

  int kc , pdgId ;
  ParticleProperty * partProp ;
  // Loop over all particles known to Pythia
  for ( kc = 1 ; kc <= this->pydat1().mstu(6) ; ++kc ) {
    // The PDG Id of the Pythia Particle (might be different
    // from the real PDG Id)
    pdgId = this->pydat2().kchg( kc , 4 ) ;
    // retrieve StdHep PDG Id
    partProp = m_ppSvc -> findByPythiaID( pdgId ) ;
    if ( partProp ) pdgId = partProp -> jetsetID( ) ;

    if ( ( m_evtTool->isKnownToEvtGen( pdgId ) ) ||
         ( m_evtTool->isKnownToEvtGen( -pdgId ) ) ) {
      // If the particle (or the antiparticle), Pythia does not
      // make any difference between the 2, declare it stable :
      // MDCY(KC,1) = 0
#ifdef WIN32
      this->pydat3().mdcy( kc , 1 ) = 0 ;
#else
      this->pydat3().mdcy( kc , 1 ) = 0 ;
#endif
    }
  }
  
  //
  // some defaults:
  // read base Pythia command vector from job options
  StatusCode sc = parsePythiaCommands( m_basePythiaCommandVector ) ;
  if ( ! sc.isSuccess( ) ) 
    return Error( "Unable to read base Pythia commands" , sc ) ;

  // flags for forced fragmentation
  if ( (m_signal) && ( m_forcedFragmentation ) ) {
    this->pydat1().mstj(30)=1;
    this->pydat1().mstj(31)=100;
    this->pydat1().mstj(33)=m_bhadron;
    
    switch(m_bhadron) {
    case 511:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=1;
      break;
      
    case -511:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-1;
      break;
      
    case 521:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=2;
      break;
      
    case -521:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-2;
      break;
      
    case 531:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=3;
      break;
      
    case -531:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-3;
      break;
      
    case 541:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=4;
      break;
      
    case -541:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-4;
      break;
      
    case 551:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=5;
      break;
      
    case 5122:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=2101;
      break;
      
    case -5122:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=-2101;
      break;
      
    default:
      error() << "Unknown b-hadron!" << endreq;
      break;
    }
  }
  else this->pydat1().mstj(30)=0;
  
  // Read user pythia command vector from option file
  sc = parsePythiaCommands( m_userPythiaCommandVector ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Unable to read User Pythia Commands" , sc ) ;
  
  // Now call pyinit and set listing
  const char* envval = m_envval1.c_str();
  
  // if file already exist, delete it
  std::remove( envval ) ;

#ifdef WIN32
  INITPYBLOCK(&m_fortout,envval,strlen(envval));
#else
  initpyblock_(&m_fortout,envval,strlen(envval));
#endif
  
  const char* frame = m_frame.c_str();
  const char* beam = m_beam.c_str();
  double winval = m_win;
  const char* target = m_target.c_str();
  int minlist   = m_initlistlevel;
  
#ifdef WIN32
  PYINIT(frame,strlen(frame),beam,strlen(beam),target,strlen(target),&winval );
  if ( minlist != -1 ) PYLIST(&minlist);
#else
  pyinit_(frame,beam,target,&winval, strlen(frame),strlen(beam),
          strlen(target) );
  if ( minlist != -1 ) pylist_(&minlist);
#endif
  
  m_events = 0;
  
  //decay table
  const char  *p_envar1="PYDAT";
  char  *p_envval1=0;
  p_envval1=getenv(p_envar1);
  
  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  
  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000); 

  // Reset forced fragmentation flag
  this->pydat1().mstu( 150 ) = 0 ;

  // print out Pythia settings
  printPythiaParameter( ) ;

  return StatusCode::SUCCESS;
}

//===========================================================================
// Generate one event with Pythia
//===========================================================================
int PythiaAlg::callGenerator() {  
  if ( ( m_signal ) && ( ! m_bfound ) && ( m_forcedFragmentation ) ) {
    // If b hadron has not yet been generated ( ! m_bfound ) 
    // and it is a signal job (m_signal) and if forced
    // fragmentation is requested (m_forcedFragmentation)
    // Force fragmentation in Pythia to produce hadron of
    // ID m_bhadron. This has to be done each event since 
    // m_bhadron might change.
    // These values are set to the default ones (no forced 
    // fragmentation) after the call to PYEVNT
    this->pydat1().mstu(150)=1;
    this->pydat1().mstu(151)=0;
    this->pydat1().mstu(152)=0;
    
    this->pydat1().mstj(33) = m_bhadron ;
    
    switch( m_bhadron ) {
    case 511:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=1;
      break;
      
    case -511:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-1;
      break;
      
    case 521:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=2;
      break;
      
    case -521:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-2;
      break;
      
    case 531:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=3;
      break;
      
    case -531:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-3;
      break;
      
    case 541:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=4;
      break;
      
    case -541:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=-4;
      break;
      
    case 551:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=5;
      break;
      
    case 5122:
      this->pydat1().mstj(32)=5;
      this->pydat1().mstj(34)=2101;
      break;
      
    case -5122:
      this->pydat1().mstj(32)=-5;
      this->pydat1().mstj(34)=-2101;
      break;
      
    default:
      error() << "Unknown b-hadron!" << endreq;
      break;
    }
  }  
  
  // Prepare the beam smearing
  if(m_anglesmear > 0.0) {
    double p1x,p1y,p1z,p2x,p2y,p2z;
    p1x = m_pbeam * sin((m_hangle+m_gaussDist()*m_anglesmear)*1.0e-6);    
    p1y = m_pbeam * sin((m_vangle+m_gaussDist()*m_anglesmear)*1.0e-6);
    p1z = m_pbeam;
    if (m_fixed) {
      p2x = 0;
      p2y = 0;
      p2z = 0;
    } else {
      p2x = m_pbeam * sin((m_hangle+m_gaussDist()*m_anglesmear)*1.0e-6);     
      p2y = m_pbeam * sin((m_vangle+m_gaussDist()*m_anglesmear)*1.0e-6); 
      p2z = -m_pbeam;
    }
#ifdef WIN32
    SETBEAM (&p1x,&p1y,&p1z,&p2x,&p2y,&p2z);
#else      
    setbeam_(&p1x,&p1y,&p1z,&p2x,&p2y,&p2z);
#endif
  }

  // Generate event 
#ifdef WIN32
  PYEVNT();
#else
  pyevnt_();
#endif
  
  // update event counter
  ++m_events;

  // For debuging : print each event if required
  if ( m_eventlistlevel >= 0 ) {
#ifdef WIN32
    PYLIST(&m_eventlistlevel); // List this event if required
#else
    pylist_(&m_eventlistlevel);
#endif
  }

  // if no repeated hadronization convert from PYJETS to HEPEVT
  if ( ( ! m_repHad ) || ( ! m_signal ) ) {
  
    // Tell lunhep to convert from PYJETS to HEPEVT
    // Do the conversion 
    // Use STDHEP translation routine 1999/01/27 CG

    int nB = convertToStdHep( ) ;

    if ( nB >= 1 ) {
      m_numberOfBInteractions++ ;
      m_bCounter++ ;
      if ( nB >= 3 ) m_fourbCounter++ ;
    }
  }
  
  return this->pypars().msti(1);
}

//===========================================================================
// Finalize PYTHIA (Print statistics)
//===========================================================================
StatusCode PythiaAlg::genFinalize() {
#ifdef WIN32
  PYSTAT( &m_pystatlevel ) ;
#else
  pystat_( &m_pystatlevel ) ;
#endif

  if ( ! msgLevel( MSG::DEBUG ) ) {
#ifdef WIN32
    PDFLIB_END() ;
#else
    pdflib_end_() ;
#endif
  }

  std::string delcmd ( "rm -f " ) ;
  delcmd += "pdflib_init.tmp" ;
#ifndef WIN32
  system( delcmd.c_str( ) ) ;
#endif

  return StatusCode::SUCCESS;
}

//===========================================================================
// Fill the HepMC event evt and change the Id of the particles to 
// the PDG ones
//===========================================================================
StatusCode PythiaAlg::fillEvt( HepMC::GenEvent* evt) {
  HepMC::IO_HEPEVT hepio;
  
  // Fill event into HepMC and transient store
  hepio.fill_next_event(evt);

  // if it is minimum bias production 
  // or a signal production with forced fragmentation exit
  // other wise process the event to decay with EvtGen stable
  // Pythia particles which might give one of the signal B hadron
  if ( ( ( m_signal ) && ( ! ( m_forcedFragmentation ) ) ) || ( m_binclus ) ) {

    // Copy the particles in a temp vector to sort them and
    // to be able to reproduce events
    
    GenParticles theHepMCVector ;
    HepMCUtils::SortHepMC( theHepMCVector , evt -> particles_size() ,
                           evt -> particles_begin( ) , 
                           evt -> particles_end( ) ) ;
    
    // ID are transformed from PythiaID to PDG Id in lunhep_ 
    ParticleProperty * partProp ;
    GenParticles::iterator iter ;
    for ( iter = theHepMCVector.begin( ) ; iter != theHepMCVector.end( ) ;
          ++iter ) { 

      // obtain particle property for this particle
      partProp = m_ppSvc -> findByStdHepID( (*iter) -> pdg_id( ) ) ;
      
      if ( partProp ) {
        // Decay generically with EvtGen all stable B hadrons
        // with a mass larger than the minimum signal B mass
        // Except for prompt J/psi production where we should also decay 
        // all particles even if they have no b hadrons
        ParticleID pid ( (*iter)->pdg_id() ) ;
        ParticleID pidSignal ( *(m_hadrons.begin()) ) ;
        
        if ( ( pid.hasBottom( ) ) || ( pidSignal.hasCharm( ) ) ) {
          // Mass check
          if ( ( (*iter) -> momentum() . m() > ( m_minBMass / GeV ) ) &&
               ( abs( (*iter)->pdg_id() ) != m_targetIdMinBMass ) ) {
            
            // Check it is known to EvtGen
            if ( m_evtTool -> isKnownToEvtGen( (*iter)->pdg_id() ) ) {
              // Check it has no daughter
              // and it is a stable particle
              // status = 1 : particle undecayed in Pythia
              if ( ( 1 ==  (*iter) -> status() ) && 
                   ( 0 == (*iter) -> end_vertex( ) ) ) {
                // Decay the particle up to the minBMass
                
                // The particle which has to be decayed by EvtGen
                HepMC::GenParticle * hepMCPart = *iter ;
                
                // disable possibility to flip b flavour in EvtGen
                m_evtTool -> disableFlip ( ) ;
                
                // Generate decay with EvtGen
                StatusCode sc =
                  m_evtTool -> generateDecayWithLimit( evt , hepMCPart ,
                                                       m_targetIdMinBMass ) ;
                
                if ( ! sc.isSuccess( ) ) return StatusCode::FAILURE ;
                
                // replace status 1 with status 888 for G4Particle
                // conversion
                (*iter) -> set_status( 888 ) ;
              }
            }
          }
        }
      }
    }
  }
  
 return StatusCode::SUCCESS;
}

//============================================================================
// Finalize method
//============================================================================
StatusCode PythiaAlg::preFinalize( ){
  debug() << "Finalize PythiaAlg" << endreq ;
  return StatusCode::SUCCESS ; 
}

//============================================================================
// true if event evt is selected ( == B is in acceptance and of correct
// type ) 
//============================================================================
bool PythiaAlg::selectEvt( HepMC::GenEvent * evt ) {
  // Reset skip event flag
  m_skipEvent = false ;
  
  // Reset signal particle counter
  m_obtainedSignalType = 0 ;

  // Order the HepMC Particles with respect to bar codes
  // Copy the particles in a temp vector to sort them and
  // to be able to reproduce events and remove particles with
  // status equal to 3
  
  GenParticles theHepMCVector ;
  HepMCUtils::SortHepMC( theHepMCVector , evt -> particles_size() ,
                         evt -> particles_begin() ,
                         evt -> particles_end() ) ;
 
  // Vector to contain pointers to particle which are in acceptance
  GenParticles theAcceptanceVector ;

  GenParticles::iterator iter ;
  PIDs::iterator hiter ;
  for ( iter = theHepMCVector.begin() ; iter!=theHepMCVector.end(); ++iter ) {
    for ( hiter = m_hadrons.begin() ; m_hadrons.end() != hiter ; ++hiter )
      if( 0 != *hiter ) { 
        // This particle is requested to be generated ?
        if( (*iter)->pdg_id() == *hiter ) {
          double px = (*iter)->momentum().px() ;
          double py = (*iter)->momentum().py() ;
          double pz = (*iter)->momentum().pz() ;
          
          double pp    = sqrt( px*px + py*py + pz*pz ) ;          
          double theta = acos( fabs( pz ) / pp ) ;                  
          
          m_numberBPresentedAngularCut++ ;
          if ( (*iter)->pdg_id() > 0 ) m_numberBPresentedAngularCutB++ ;
          else m_numberBPresentedAngularCutBbar++ ;
          // A B hadron of requested type is generated
          // do not repeat hadronization further
          m_skipEvent = true ;
          
          // Increase counter of B hadron of requested type
          m_obtainedSignalType++ ;
          
          //check if the requested b hadron is within the 
          // acceptance angle
          if ( ( theta <= m_thetamax )  && ( pp >= m_pmin ) ) {
            m_numberBAcceptedAngularCut++ ;
            if ( (*iter) -> pdg_id() > 0 ) m_numberBAcceptedAngularCutB++ ;
            else m_numberBAcceptedAngularCutBbar++ ;
            // Put Particle in acceptance vector             
            theAcceptanceVector.push_back( *iter ) ;
            // If it is a signal decay and if it is requested
            // to force fragmentation 
            // analyse event because there is only
            // one b hadron in the list (because we force
            // the fragmentation...)
            if ( ( m_signal ) && ( m_forcedFragmentation ) ) {
              //check if it is on the right side
              if ( pz<0 ) {
                double xPart , yPart , zPart , tPart ;
                double pxPart, pyPart, pzPart, ePart ;
                //do the 'parity trick' (z -> -z, pz -> -pz)
                HepMC::GenEvent::vertex_iterator vitr ;
                for ( vitr = evt->vertices_begin() ; 
                      vitr != evt->vertices_end() ; ++vitr ) {
                  xPart = (*vitr)->position().x() ;
                  yPart = (*vitr)->position().y() ;
                  zPart = (*vitr)->position().z() ;
                  tPart = (*vitr)->position().e() ;
                  
                  (*vitr)-> set_position(HepLorentzVector(xPart,yPart,
                                                          -zPart,tPart));
                }
                
                HepMC::GenEvent::particle_iterator pitr ;
                for( pitr = evt->particles_begin() ; 
                     pitr != evt->particles_end() ; ++pitr ) {
                  pxPart = (*pitr)->momentum().px() ;
                  pyPart = (*pitr)->momentum().py() ;
                  pzPart = (*pitr)->momentum().pz() ;
                  ePart  = (*pitr)->momentum().e()  ;
                  
                  (*pitr)-> set_momentum(HepLorentzVector(pxPart, pyPart,
                                                          -pzPart, ePart ) ) ;
                }
                //increase the counter of the 'z-transformed' 
                // events
                m_numbOfZminusZEvt++;
                if ( (*iter) -> pdg_id() > 0 ) m_numbOfZminusZEvtB++ ;
                else m_numbOfZminusZEvtBbar++ ;
              }
              
              // If this is the first B which is in acceptance
              // and which has the ID of the signal B to
              // generate (m_bhadron), give it status 889
              // to recognize him more easily
              // For a signal production and forced fragmentation
              if ( ( m_signal ) && ( m_forcedFragmentation ) ) 
                (*iter) -> set_status( 889 ) ;
              
              return true;
            }
          }
        }    
      }
  }
  
  // In case we generate signal with both B and Bbar, without
  // forced fragmentation (ie repeated hadronization or plain Pythia)
  // configurations with only one B and no Bbar or one Bbar and no B
  // should be rejected with a probability (1-BR)/(2-BR)
  if ( ( m_signal ) && ( ! ( m_forcedFragmentation ) ) &&
       ( m_evtTool -> doCPMixture( ) ) && ( 1 == m_obtainedSignalType ) ) {
    // generate random number between 0 and 1
    // and decide if accept of reject
    if ( m_evtTool -> randomFromEngine() < 
         ( ( 1. - m_signalBr ) / ( 2. - m_signalBr ) ) ) {
      // reject the event but allow to repeat hadronization
      m_skipEvent = false ;
      return false ;
    }
    // otherwise keep event
  }
  
  // If no particle in acceptance has been found
  if ( theAcceptanceVector.empty( ) ) return false ;
  
  // From here, we are in signal and no force fragmentation mode
  // or in inclusive production mode
  if ( ( ( ! ( m_signal ) ) || ( m_forcedFragmentation ) ) &&
       ( ! ( m_binclus ) ) ) return false ;
  
  int nPart = theAcceptanceVector.size( ) ;
  
  int iPart = 0 ;
  
  if ( nPart > 1 )
    // Choose randomly the particle to use as signal
    iPart = (int) floor( nPart * m_evtTool -> randomFromEngine( ) ) ;
  
  // The signal particle
  HepMC::GenParticle * thePart = theAcceptanceVector[ iPart ] ;

  // If signal without forced fragmentation, process the event to 
  // generate correctly CP violation
  if ( ( m_signal ) && ( ! ( m_forcedFragmentation ) ) ) {
    // Set status 889 to recognize it afterwards
    thePart -> set_status( 889 ) ;
    m_numberOfBBeforeFlip++ ;
    if ( thePart -> pdg_id() > 0 ) m_numberOfBBeforeFlipB++ ;
    else m_numberOfBBeforeFlipBbar++ ;
    // Decay the particle with EvtGen to see if it flips
    // (if it is a CP mixture and no forced fragmentation required)
    if ( ( m_evtTool -> doCPMixture ( ) ) && 
         ! ( m_evtTool -> forcedFragmentation ( )  ) )
      m_evtTool -> enableFlip ( ) ;
    
    // Find Id of the particle to decay
    EvtId decayId = m_evtTool -> getSignalAlias ( thePart -> pdg_id ( ) ) ;
  
    // Create a temporary GenEvent to store decay tree at rest  
    HepMCEvent * mcevt = new HepMCEvent( name( ) , 1 , 1 ) ;
    HepMC::GenEvent * hepMCevt = mcevt -> pGenEvt( ) ;
    
    // Generate the decay at rest and put it in hepMCevt 
    int resultId = 0 ;
    m_evtTool -> generateDecayAtRest( hepMCevt , decayId , resultId , 
                                      thePart ) ;
    
    // If the particle has been flipped, throw away the event
    if ( resultId != thePart -> pdg_id ( ) ) {
      thePart -> set_status( 888 ) ;
      delete mcevt ;
      return false ;
    }  

    m_numberOfFlipAccepted++ ;
    if ( thePart -> pdg_id ( ) > 0 ) m_numberOfFlipAcceptedB++ ;
    else m_numberOfFlipAcceptedBbar++ ;

    EventVector tempHepMC ;
    tempHepMC.push_back( mcevt ) ;
    
    // Store event in data store
    // at location "/Event/Gen/SignalDecay"
    EventVector::iterator ithep ;
    
    for ( ithep  = tempHepMC.begin( ) ; ithep != tempHepMC.end( ) ; ++ithep ) {
      // Check if signal HepMCEvents already exist
      if ( exist< HepMCEvents >( "/Event/Gen/SignalDecay" ) ) {
        thePart -> set_status( 888 ) ;
        delete mcevt ;
        tempHepMC.clear( ) ;
        warning() << "/Event/Gen/SignalDecay already exist "
                  << "Check logic ! " << endreq ;
        return false ;
      } else {
        // Create HepMCEvents and add to data store
        HepMCEvents * hepVect = new HepMCEvents ;
        hepVect -> insert ( *ithep ) ;
        StatusCode sc = put( hepVect , "/Event/Gen/SignalDecay" ) ;
        if ( ! sc.isSuccess() ) {
          thePart -> set_status( 888 ) ;
          delete hepVect ;
          delete mcevt ;
          tempHepMC.clear( ) ;
          return false ;
        }
      }  
    }
  }
  
  // If signal particle flies backward, "parity flip" the event
  // Here we are either in inclusive production mode or
  // in signal mode without forced fragmentation
  if ( thePart -> momentum() . pz() < 0 )  {
    double x_tr , y_tr , z_tr , t_tr ;
    double px_tr, py_tr, pz_tr, e_tr ;
    //do the 'parity trick' (z -> -z, pz -> -pz)
    HepMC::GenEvent::vertex_iterator vitr ;
    for ( vitr = evt->vertices_begin() ; vitr != evt->vertices_end() ; 
          ++vitr ) {                      
      x_tr = (*vitr)->position().x() ;
      y_tr = (*vitr)->position().y() ;
      z_tr = (*vitr)->position().z() ;
      t_tr = (*vitr)->position().e() ;
      
      (*vitr)-> set_position(HepLorentzVector(x_tr,y_tr,-z_tr,t_tr));
    }
    
    HepMC::GenEvent::particle_iterator pitr ;
    for ( pitr = evt->particles_begin() ; pitr != evt->particles_end() ; 
          ++pitr ) {
      px_tr = (*pitr)->momentum().px() ;
      py_tr = (*pitr)->momentum().py() ;
      pz_tr = (*pitr)->momentum().pz() ;
      e_tr  = (*pitr)->momentum().e()  ;
      
      (*pitr)-> set_momentum(HepLorentzVector(px_tr,py_tr,-pz_tr,e_tr));
    }
    //increase the counter of the 'z-transformed' 
    // events
    m_numbOfZminusZEvt++;
    if ( thePart -> pdg_id() > 0 ) m_numbOfZminusZEvtB++ ;
    else m_numbOfZminusZEvtBbar++ ;
  }
  return true ;
}
//=============================================================================
// Turn Off Pythia Fragmentation ( MSTJ(1) = 0 )
//=============================================================================
void PythiaAlg::turnOffPythiaFragmentation( ) {
  this -> pydat1( ) . mstj( 1 ) = 0 ;
}
//=============================================================================
// Turn On Pythia Fragmentation ( MSTJ(1) = 1 )
//=============================================================================
void PythiaAlg::turnOnPythiaFragmentation( ) {
  this -> pydat1( ) . mstj( 1 ) = 1 ;
}
//============================================================================
// Return 1 if the Pythia event contains a b (or b bar quark) or a B hadron
//============================================================================
int PythiaAlg::isBBbarEvent( ) {
#ifdef WIN32
  return CONTAINSB( ) ;
#else
  return containsb_() ;
#endif
}
//=============================================================================
// Save parton event (CALL PYEDIT(21) )
//=============================================================================
void PythiaAlg::savePartonEvent( ) {
  int arg = 21 ;
#ifdef WIN32
  PYEDIT( &arg ) ;
#else
  pyedit_ ( &arg ) ;
#endif
}
//=============================================================================
// Load parton event (CALL PYEDIT(22) )
//=============================================================================
void PythiaAlg::loadPartonEvent( ) {
  int arg = 22 ;
#ifdef WIN32
  PYEDIT( &arg ) ;
#else
  pyedit_ ( &arg ) ;
#endif
}
//=============================================================================
// Hadronize Pythia event ( CALL PYEXEC ) 
//=============================================================================
void PythiaAlg::hadronize( ) {
#ifdef WIN32
  PYEXEC( ) ;
#else
  pyexec_ ( ) ;
#endif
  // For debuging : print each event if required
  if ( m_eventlistlevel >= 0 ) {
#ifdef WIN32
    PYLIST(&m_eventlistlevel); // List this event if required
#else
    pylist_(&m_eventlistlevel);
#endif
  }
}
//=============================================================================
// Convert to StdHep format (CALL LUNHEP) abd returns the number of b quarks
//=============================================================================
int PythiaAlg::convertToStdHep( ) {
  int mconv=1;
  int result ;
#ifdef WIN32
  result = LUNHEP(&mconv);
#else
  result = lunhep_(&mconv);
#endif
  return result ;
}
//=============================================================================
// Print Pythia parameters
//=============================================================================
void PythiaAlg::printPythiaParameter( ) {  
  debug() << "****************************************************" << endreq ;
  debug() << "**        Pythia parameters for this job            " << endreq ;
  debug() << "**                                                  " << endreq ;
  debug() << "** MSTP(2) ( alpha_s order )                      : "
         << this -> pypars() . mstp( 2 ) << endreq ;
  debug() << "** MSTP(33) ( k factors )                         : "
         << this -> pypars() . mstp( 33 ) << endreq ;
  debug() << "** MSTP(128) ( doc. lines )                       : "
         << this -> pypars() . mstp( 128 ) << endreq ;
  debug() << "** MSTP(82) ( multiple interactions )             : "
         << this -> pypars() . mstp( 82 ) << endreq ;
  debug() << "** MSTP(52) ( PDF 1 = Pythia, 2 = PDFLIB )        : "  
         << this -> pypars() . mstp( 52 ) << endreq ;
  debug() << "** MSTP(51) ( PDF Function Code )                 : "
         << this -> pypars() . mstp( 51 ) << endreq ;
  debug() << "** PARP(82) ( scale for mult inter )              : "
         << this -> pypars() . parp( 82 ) << endreq ;
  debug() << "** PARP(89) ( reference energy scale )            : "
         << this -> pypars() . parp( 89 ) << endreq ;
  debug() << "** PARP(90) ( power of energy resc. term )        : " 
         << this -> pypars() . parp( 90 ) << endreq ;
  debug() << "** PARJ(13) ( prob. that heavy meson has spin 1 ) : "
         << this -> pydat1() . parj( 13 ) << endreq ;
  debug() << "** PARJ(14) ( prob. for spin=0 meson to be produced" << endreq ; 
  debug() << "**            with ang. mom= 1 if total spin=1 )  : "
         << this -> pydat1() . parj( 14 ) << endreq ;
  debug() << "** PARJ(15) ( prob. for spin=1 meson to be produced " << endreq ;
  debug() << "**            with ang. mom= 1 if total spin=0 )  : "
         << this -> pydat1() . parj( 15 ) << endreq ;
  debug() << "** PARJ(16) ( prob. for spin=1 meson to be produced " << endreq ;
  debug() << "**            with ang. mom= 1 if total spin=1 )  : "
         << this -> pydat1() . parj( 16 ) << endreq ;
  debug() << "** PARJ(17) ( prob. for spin=1 meson to be produced " << endreq ;
  debug() << "**            with ang. mom= 1 if total spin=2 )  : "
         << this -> pydat1() . parj( 17 ) << endreq ;
  debug() << "** MSTU(150) ( Forced fragmentation flag )        : "
         << this -> pydat1() . mstu( 150 ) << endreq ;
  debug() << "** MSTJ(30) ( Forced fragmentation flag )         : "
         << this -> pydat1() . mstj( 30 ) << endreq ;
  debug() << "** MSTJ(31) ( Forced fragmentation flag )         : "
         << this -> pydat1() . mstj( 31 ) << endreq ;
  debug() << "** MSTJ(32) ( Forced fragmentation flag )         : "
         << this -> pydat1() . mstj( 32 ) << endreq ;
  debug() << "** MSTJ(33) ( Forced fragmentation flag )         : "
         << this -> pydat1() . mstj( 33 ) << endreq ;
  debug() << "** MSTJ(34) ( Forced fragmentation flag )         : "
         << this -> pydat1() . mstj( 34 ) << endreq ;
  debug() << "** MSTJ(26) ( B0 mixing in Pythia, must be 0 )    : "
         << this -> pydat1() . mstj( 26 ) << endreq ;
  debug() << "** PARJ(33) ( cut off for fragmentation )         : "
         << this -> pydat1() . parj( 33 ) << endreq ;
  debug() << "** MSEL ( processes )                             : "
         << this -> pysubs().msel()       << endreq ;
  debug() << "** Activated processes                            : " << endreq ;
  int i ;
  for ( i = 1 ; i <= 500 ; ++i ) 
    if ( 1 == this -> pysubs() . msub( i ) ) 
      debug() << "** --- Process " << i << endreq ;
  debug() << "**                                                  " << endreq ;
  debug() << "****************************************************" << endreq ;
}

//=============================================================================
// Parse Pythia commands stored in a vector
//=============================================================================
StatusCode PythiaAlg::parsePythiaCommands( const CommandVector & 
                                           theCommandVector ) {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  unsigned int i ;
  for ( i = 0 ; i < theCommandVector.size() ; ++i ) {
    debug() << " Command is: " << theCommandVector[i]  << endreq ;
    StringParse mystring( theCommandVector[ i ] ) ;
    std::string myblock = mystring.piece(1);
    std::string myentry = mystring.piece(2);
    std::string mystr   = mystring.piece(3);
    int    myint1  = mystring.intpiece(3);
    int    myint2  = mystring.intpiece(4);
    int    myint3  = mystring.intpiece(5);
    int    myint4  = mystring.intpiece(6);
    double myfl1   = mystring.numpiece(4);
    double myfl2   = mystring.numpiece(5);
    double myfl0   = mystring.numpiece(3);
    
    // Note that Pythia needs doubles hence the convert here
    debug() << myblock << " block  " << myentry << " item  " 
            << myint1 << "  value " << myfl1 <<endreq;
    if ( "pyinit" == myblock ) {
      if      ( "pbar"    == myentry ) m_beam = "pbar-" ;
      else if ( "win"     == myentry ) m_win = myfl0 ;      
      else if ( "pylisti" == myentry ) m_initlistlevel = myint1;
      else if ( "pylistf" == myentry ) m_eventlistlevel=myint1 ;
      else if ( "pystatf" == myentry ) m_pystatlevel = myint1 ;
      else if ( "output"  == myentry ) {
        m_envval1=mystr;
        m_fortout=49;
        this->pydat1().mstu(11)=m_fortout;
      }
      else 
        return 
          Error(std::string("PYTHIA ERROR, entry PYINIT has PBAR PYLISTI ")+
                std::string("PYLISTF OUTPUT DUMPR AND WIN: ") + 
                std::string("YOU HAVE SPECIFIED ") +
                std::string( myentry ) ) ;
    }
    else if ( "pysubs" == myblock ) {
      if      ( "msel" == myentry )  this->pysubs().msel()=myint1 ; 
      else if ( "msub" == myentry ) this->pysubs().msub(myint1)=myint2;
      else if ( "ckin" == myentry ) this->pysubs().ckin(myint1)=myfl1;
      else if ( "kfin" == myentry ) this->pysubs().kfin(myint1,myint2)=myint3;
      else 
        return Error( std::string( "PYTHIA ERROR, block PYSUBS has MSEL" ) +
                      std::string( ", MSUB, KFIN AND CKIN: YOU HAVE " )    + 
                      std::string( "SPECIFIED " ) + std::string( myentry ) );
    }
    else if ( "pypars" == myblock ) {
      if      ( "mstp" == myentry ) this->pypars().mstp(myint1)=myint2;
      else if ( "msti" == myentry ) this->pypars().msti(myint1)=myint2;
      else if ( "parp" == myentry ) this->pypars().parp(myint1)=myfl1;
      else if ( "pari" == myentry ) this->pypars().pari(myint1)=myfl1;
      else 
        return Error( std::string( "PYTHIA ERROR, block PYPARS has MSTP," ) +
                      std::string( "PARP, MSTI AND PARI: YOU HAVE " ) +
                      std::string( "SPECIFIED " ) + std::string( myentry ) );
    }
    else if ( "pydat1" == myblock ) {
      if      ( "mstu" == myentry ) this->pydat1().mstu(myint1)=myint2;
      else if ( "mstj" == myentry ) this->pydat1().mstj(myint1)=myint2;
      else if ( "paru" == myentry ) this->pydat1().paru(myint1)=myfl1;
      else if ( "parj" == myentry ) this->pydat1().parj(myint1)=myfl1;
      else 
        return Error( std::string( "PYTHIA ERROR, block PYDAT1  HAS MSTU, ")+
                      std::string( "MSTJ, PARU AND PARJ: YOU HAVE ") +
                      std::string( "SPECIFIED " ) + std::string( myentry ) );
    }
    else if ( "pydat2" == myblock ){
      if      ( "kchg" == myentry ) this->pydat2().kchg(myint1,myint2)=myint3;
      else if ( "pmas" == myentry ) this->pydat2().pmas(myint1,myint2)=myfl2;
      else if ( "parf" == myentry ) this->pydat2().parf(myint1)=myfl1;
      else if ( "vckm" == myentry ) this->pydat2().vckm(myint1,myint2)=myfl2;
      else 
        return Error( std::string("PYTHIA ERROR, block PYDAT2  HAS KCHG,")+
                      std::string("PMAS, PARF AND VCKM: YOU HAVE ")+
                      std::string("SPECIFIED " ) + std::string(myentry )) ;
    }
    else if ( "pydat3" == myblock ){
      if       ( "mdcy" == myentry ) {
#ifdef WIN32
        this->pydat3().mdcy(PYCOMP(&myint1),myint2)=myint3; 
#else
        this->pydat3().mdcy(pycomp_(&myint1),myint2)=myint3; 
      }
#endif      
      else if ( "mdme" == myentry ) this->pydat3().mdme(myint1,myint2)=myint3;
      else if ( "brat" == myentry ) this->pydat3().brat(myint1)=myfl1;
      else if ( "kfdp" == myentry ) this->pydat3().kfdp(myint1,myint2)=myint3;
      else 
        return Error( std::string("PYTHIA ERROR, block PYDAT3  HAS KFDP, ")+
                      std::string("MDCY, BRAT AND MDME : YOU HAVE " ) + 
                      std::string("SPECIFIED ") + std::string( myentry ) ) ;
    }
    else if ( "pydatr" == myblock ){
      if       ( "mrpy" == myentry ) this->pydatr().mrpy(myint1)=myint2 ;
      else if  ( "rrpy" == myentry ) this->pydatr().rrpy(myint1)=myfl1;
      else 
        return Error( std::string( "PYTHIA ERROR, block PYDATR  HAS MRPY " )+
                      std::string( "AND RRPY : YOU HAVE SPECIFIED " ) +
                      std::string( myentry ) ) ;
    }
    else if ( "pymssm" == myblock ){
      if      ( "imss" == myentry ) this->pymssm().imss(myint1)=myint2;
      else if ( "rmss" == myentry ) this->pymssm().rmss(myint1)=myfl1;
      else 
        return Error( std::string( "PYTHIA ERROR, block PYMSSM has IMSS " )+
                      std::string( "AND RMSS: YOU HAVE SPECIFIED " ) +
                      std::string( myentry ) ) ;
    }
    else if ( "pyint2" == myblock ){
      if      ( "iset" == myentry ) this->pyint2().iset(myint1)=myint2;
      else if ( "kfpr" == myentry ) this->pyint2().kfpr(myint1,myint2)=myint3;
      else if ( "coef" == myentry ) this->pyint2().coef(myint1,myint2)=myfl2;
      else if ( "icol" == myentry )
        this->pyint2().icol(myint1,myint2,myint3)=myint4;
      else 
        return Error( std::string( "PYTHIA ERROR, block PYINT2 has ISET " ) +
                      std::string( "KFPR COEF AND ICOL: YOU HAVE " ) +
                      std::string( "SPECIFIED ") + std::string( myentry ) ) ;
    }
    else 
      return Error( std::string( " ERROR in PYTHIA PARAMETERS   " ) +
                    std::string( myblock ) +
                    std::string( " is and invalid common block name !" ) ) ;
  }
  // end of parsing
  //------------------------------------------------------------------
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Retrieve the Hard scatter information
//=============================================================================
void PythiaAlg::hardProcessInfo(HardInfo* hardInfo) {
  hardInfo->setSHat( this->pypars().pari(14) );
  hardInfo->setTHat( this->pypars().pari(15) );
  hardInfo->setUHat( this->pypars().pari(16) );
  hardInfo->setPtHat( this->pypars().pari(17) );
  hardInfo->setX1Bjorken( this->pypars().pari(33) );
  hardInfo->setX2Bjorken( this->pypars().pari(34) );

  debug() << "Hard process = " 
          << this->pypars().msti(1)  << " "
          << this->pypars().pari(14) << " " 
          << this->pypars().pari(15) << " " 
          << this->pypars().pari(16) << " " 
          << this->pypars().pari(17) << " " 
          << this->pypars().pari(33) << " " 
          << this->pypars().pari(34)
          << endreq ;
}
