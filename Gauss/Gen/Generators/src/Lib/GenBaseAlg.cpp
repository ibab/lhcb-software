// $Id: GenBaseAlg.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
// -------------------------------------------------------------
//
// initial version by M. Shapiro
//
// LHCb version W. Pokorski

// local
#include "Generators/GenBaseAlg.h"

// 
#include "Event/HepMCEvent.h"
#include "Event/GenHeader.h"
#include "Event/HardInfo.h"

#include "HepMC/GenEvent.h"

// Gaudi RandomNumberSvc
Rndm::Numbers randgaudi;

//===========================================================================
// Constructor
//===========================================================================
GenBaseAlg::GenBaseAlg(const std::string& name, 
                       ISvcLocator* pSvcLocator) : 
  GaudiAlgorithm(name, pSvcLocator) ,
  m_bhadron( 0 ) ,
  m_bfound( false ) ,
  m_signal ( false ) ,
  m_forcedFragmentation( false ) ,
  m_binclus( false ) ,
  m_repHad ( false ) ,
  m_numbOfZminusZEvt( 0 ) ,
  m_numbOfZminusZEvtB( 0 ) , m_numbOfZminusZEvtBbar( 0 ) ,
  m_numberOfBInteractions( 0 ) ,
  m_numberOfAcceptedBInteractions( 0 ) ,
  m_numberOfAcceptedDoubleBInteractions( 0 ) ,
  m_numberOfFlipAccepted( 0 ) ,
  m_numberOfFlipAcceptedB( 0 ) , m_numberOfFlipAcceptedBbar( 0 ) ,
  m_numberBPresentedAngularCut( 0 ) ,
  m_numberBPresentedAngularCutB( 0 ) , m_numberBPresentedAngularCutBbar( 0 ) ,
  m_numberBAcceptedAngularCut( 0 ) ,
  m_numberBAcceptedAngularCutB( 0 ) , m_numberBAcceptedAngularCutBbar( 0 ) ,
  m_numberOfBBeforeFlip( 0 ) ,
  m_numberOfBBeforeFlipB( 0 ) , m_numberOfBBeforeFlipBbar( 0 ) ,
  m_bCounter( 0 ) ,
  m_fourbCounter( 0 ) ,
  m_skipEvent( false ) ,
  m_rlpeak( 0. ) ,
  m_numbOfBunchX( 0 ) ,
  m_numbOfEvents( 0 ) ,
  m_numbOfAcceptedEvt( 0 ) ,
  m_numbOfInter( 0 ) ,
  m_numbOfAcceptedInt( 0 ) ,
  m_numberOfRepetitions( 0 ) ,
  m_numberOfExcessHad( 0 ) ,
  m_maxNumberOfRep( 1000 ) {
  declareProperty("Mode"          , m_mode = 1 ) ;
  declareProperty("EventType"     , m_evnType = 0 ) ;
  declareProperty("BhadronID"     , m_hadrons ) ;
  declareProperty("ThetaMax"      , m_thetamax = 0.4 ) ;
  declareProperty("Pmin"          , m_pmin = 0.0 ) ;
  declareProperty("MeanInt"       , m_meanInteractions = 1.0 ) ;
  declareProperty("Luminosity"    , m_luminos = 2.0 ) ;
  declareProperty("FillDuration"  , m_filldur = 7.0 ) ; 
  declareProperty("BeamDecayTime" , m_dectime = 10.0 ) ;
  declareProperty("CrossRate"     , m_crossrate = 30.0 ) ;
  declareProperty("TotalXSect"    , m_totxsect = 102.4 ) ;
}

//===========================================================================
// Destructor
//===========================================================================
GenBaseAlg::~GenBaseAlg() { } 

//===========================================================================
// Initialization
//===========================================================================
StatusCode GenBaseAlg::initialize() {
  // Initialize the base class
  StatusCode sc = GaudiAlgorithm::initialize( ) ;
  
  if ( sc.isFailure( ) ) return Error( "Base class is not initialized" , sc ) ;
  
  // Optional Pre-initialization sequence of the generator 
  sc = this -> preInitialize( ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Error in pre-initialization of Algorithm" , sc ) ;
  
  // Initialization of Gaudi Rndm::Flat
  // if it has not been initialized by another algorithm
  if ( ! ( randgaudi ) ) {
    sc = randgaudi.initialize( randSvc() , Rndm::Flat(0,1) ) ;
    if ( ! sc.isSuccess( ) ) 
      return Error( "Could not initilize Rndm::Flat" , sc ) ;
  }
  
  // Initialization of Gaudi Rndm::Gauss
  sc = m_gaussDist.initialize( randSvc() , Rndm::Gauss(0.0,1.0) ) ;
  if ( ! sc.isSuccess( ) ) 
    return Error( "Could not initilize Rndm::Gauss" , sc ) ;
  
  info() << endreq ;
  
  // check the length of the list of b-hadrons  
  // if it is 0 - generate min-bias
  // if it is 1 - set the flag for the forced fragmentation and
  // set the m_bhadron variable which will be then used in PythiaAlg
  // if it is more than 1 - set the flag of 'inclusive' event

  info() << "Generate event of type " << m_evnType << endreq;
  if ( m_hadrons.empty( ) ) 
    info() << "Generating minimum-bias events." << endreq ;
  else if ( 1 == m_hadrons.size() ) {
    // Pythia will force fragmentation
    // If requested in EvtGenTool
    if ( ! m_binclus ) m_signal = true ;
    m_bhadron = *(m_hadrons.begin()) ;
    if ( m_binclus ) info() << "*** Inclusive Production ***" << endreq ;
    else info() << "*** Signal Production ***" << endreq ;
    info() << "Generating decays of: " << m_bhadron << endreq ;
  } 
  else if ( 2 == m_hadrons.size() ) {
    if ( * ( m_hadrons.begin() ) == - * ( m_hadrons.begin() + 1 ) ) {
      // CP mixture
      // Pythia will force fragmentation
      // if requested in EvtGenTool
      if ( ! m_binclus ) m_signal = true ;
      if ( m_binclus ) info() << "*** Inclusive Production ***" << endreq ;
      else info() << "*** Signal Production ***" << endreq ;
      m_bhadron = * ( m_hadrons.begin() ) ;
      info() << "Generating mixture of: " << m_bhadron << " and " 
             << - m_bhadron << endreq ;
    }
    else {
      m_binclus = true ;
      m_signal  = false ;
      info() << "Generating inclusive decays with the following list "
             << "of 'required' hadrons: " ;
      
      PIDs::iterator hiter ;
      for ( hiter = m_hadrons.begin() ; m_hadrons.end() != hiter ; ++hiter ) 
        info() << *hiter << " " ;      
    }
  }
  else if (m_hadrons.size()>2) {
    m_binclus = true ;
    m_signal  = false ;
    info() << "Generating inclusive decays with the following list "
           << "of 'required' hadrons: ";
    
    PIDs::iterator hiter ;
    for ( hiter = m_hadrons.begin() ; m_hadrons.end() != hiter ; ++hiter ) 
      info() << *hiter << " " ;      
  }
  
  info() << endreq ;
  
  switch (m_mode) {      
  case  0:
    if ( 1.0 == m_meanInteractions )
      info() << "Standard Initialization: Single Interaction Mode " << endreq;
    else 
      info() << "Fixed Number of Interactions per Event is: "
             << m_meanInteractions << endreq;
    break;
    
  case 1:
    info() << "Poisson distribution with 'LHCb mean' . " << endreq ;
    info() << "Luminosity (10^32 / cm^2 s): " << m_luminos << endreq ;
    debug() << "Fill duration (hours): " << m_filldur << endreq ;
    debug() << "Beam decay time (hours): " << m_dectime << endreq ;
    debug() << "Bunch crossing rate (MHz): " << m_crossrate << endreq ;
    debug() << "Total cross section (mbarn): " << m_totxsect << endreq ;
    
    m_rlpeak=m_luminos*m_filldur/m_dectime/(1.0-exp(-m_filldur/m_dectime));
    
    break;
    
  default:
    info() << "Unknown pile-up mode" << endreq;
    break;
  }
  
  // to be sure the options are coherent
  m_repHad = ( m_repHad ) && ( m_signal ) ;
  m_forcedFragmentation = ( m_forcedFragmentation ) && ( m_signal ) ;

  // Initialize the generator itself
  StatusCode status = this -> genInitialize( ) ;

  return status;
}

//===========================================================================
// Execute method (Generate one event)
//===========================================================================
StatusCode GenBaseAlg::execute() {
  if ( ( m_signal ) && ( m_forcedFragmentation ) ) {
    // Generation of signal B events
    // Check if predefined B decay exist
    HepMCEvents * hepMCptr( 0 ) ;
    try { 
      hepMCptr = get<HepMCEvents>( "/Event/Gen/SignalDecay" ) ;
    }
    catch ( const GaudiException & exc ) {
      Exception( "No predefined signal B decay for forced fragmentation" , 
                 exc ) ;
    }
    
    if ( 1 != hepMCptr->size() ) 
      return Error( "More than one predefined signal B decay !" ) ;
    else {
      HepMCEvents::iterator it = hepMCptr->begin() ;
      // Look for particle with status = 998 which is
      // the signal B decay already produced by SignalDecayAlg
      HepMCPIterator itp ;
      HepMC::GenEvent * pEvt = (*it) -> pGenEvt() ;
      for ( itp = pEvt -> particles_begin() ; itp != pEvt -> particles_end() ;
            ++itp ) {
        if ( 998 == ( *itp ) -> status() ) {
          // sets the m_bhadron property to the PDG Id of the
          // particle generated by SignalDecayAlg
          m_bhadron = (*itp) -> pdg_id() ;
          // Sets m_hadrons list to the same Id
          m_hadrons.clear() ;
          m_hadrons.push_back( m_bhadron ) ;
          // There is only one signal B hadron so we can exit
          break ;
        }
      }
    }
  }
  
  int numToGenerate;
  int numberOfRep ;
  StatusCode status;
  // temporary vector for HepMC events
  EventVector tempHepMC;
  // temporary vector for Hard scatter info
  HardVector tempHard;
  int procType;
  int nB ;
  double rlumnow=0;

  bool eventIsOK=false;

  m_numbOfAcceptedEvt++;

  // loop untill the satisfactory set of events has been generated
  do {
    m_numbOfEvents++;
    // Initialise counter of accepted B interactions in one event
    m_bCounter = 0 ;
    m_fourbCounter = 0 ;

    // delete old events and clear the temporary vector
    EventVector::iterator itr ;
    for ( itr = tempHepMC.begin() ; itr != tempHepMC.end() ; ++itr ) 
      delete (*itr) ;    
    tempHepMC.clear() ;
    
    // do the same for hard process information
    HardVector::iterator ith ;
    for ( ith = tempHard.begin() ; ith != tempHard.end() ; ++ith ) 
      delete (*ith) ;
    tempHard.clear();
      
    // Decide how many interactions to generate 
    if ( 0 == m_mode ) { 
      numToGenerate = (int) m_meanInteractions;
      m_numbOfBunchX++;
      m_numbOfInter=m_numbOfInter+numToGenerate;
    }
    else if ( 1 == m_mode ) {
      do {
        double time = randgaudi() * m_filldur ;
        rlumnow = m_rlpeak * exp( -time / m_dectime ) ;
        double rnav = rlumnow * m_totxsect / ( 10.0 * m_crossrate ) ;
        Rndm::Numbers 
          m_pPoissonGenerator( randSvc() , Rndm::Poisson( rnav ) ) ;
        numToGenerate = (int) m_pPoissonGenerator() ;
        
        m_numbOfBunchX++;
        m_numbOfInter=m_numbOfInter+numToGenerate;
        
      } while ( 0 == numToGenerate ) ;
    }
    else {
      numToGenerate = 1 ;
      m_numbOfBunchX++ ;
      m_numbOfInter++ ;
    }

    m_bfound = false ;
    
    // Generate as many interactions as requested
    for( int i=0 ; i<numToGenerate ; ++i ) {
      int isBBbar = 0 ;
      // Turn off Pythia fragmentation
      if ( ( m_repHad ) && ( m_signal) ) 
        this -> turnOffPythiaFragmentation( ) ;
        
      // Generate an event
      procType = this->callGenerator();
      
      if ( ( m_repHad ) && ( m_signal ) ) { 
        // In case method is repeated hadronization
        // Check if event contains b or bbar quark
        // (if no B already found)
        if ( ! m_bfound ) isBBbar = this -> isBBbarEvent() ;
        // Turn on fragmentation
        this -> turnOnPythiaFragmentation( ) ;
        // if event contains BBbar save it for further use
        if ( ( ! m_bfound ) && ( 1 == isBBbar ) ) this -> savePartonEvent( ) ;
        this -> hadronize ( ) ;
        nB = this -> convertToStdHep ( ) ;
        if ( nB >= 1 ) {
          m_numberOfBInteractions++ ;
          m_bCounter++ ;
          if ( nB >= 3 ) m_fourbCounter++ ;
        }
      }        
      
      // Create the HepMC event and store there the generated event
      HepMCEvent* mcevt = new HepMCEvent(name(),1,1);
      HepMC::GenEvent* evt = mcevt->pGenEvt();
      status = this->fillEvt(evt);
      if ( ! status.isSuccess( ) ) 
        return Error( "Failed to fill event" , status ) ;
        
      evt->set_signal_process_id( procType ) ;
      
      // Create the object with the hard scatter information
      HardInfo* hard = new HardInfo();
      this->hardProcessInfo(hard);
      hard->setEvent(mcevt);
      
      // if forced fragmentation or b-inclusive enabled, check 
      // whether the requested b hadron is there if it has not already
      // been produced
      if( ( m_signal && !m_bfound ) || ( m_binclus && !m_bfound ) ) {
        try { m_bfound = this -> selectEvt( evt ) ; }
        catch ( const GaudiException & exc ) {
          Exception( "Error is selectEvt" , exc ) ;
        }
        
        if (m_bfound) evt -> set_event_scale( 1.0 ) ;
        // If method is repeated hadronization do it here
        // if correct B has not been found and the event contains
        // a b-bbar quarks
        numberOfRep = 1 ;
        if ( ( ! m_bfound ) && ( m_repHad ) && ( 1 == isBBbar ) &&
             ( m_signal ) ) {
          // hadronization has already been performed once so increment
          // the corresponding counter
          m_numberOfRepetitions++ ;
          while ( ( ! m_bfound ) && ( numberOfRep < m_maxNumberOfRep ) ) {
            // delete previous event 
            delete mcevt ;
            // increase counter
            numberOfRep++ ;
            m_numberOfRepetitions++ ;
            // retrieve parton level event which contains b-bbar quark
            this -> loadPartonEvent( ) ;
            // turn on fragmentation
            this -> turnOnPythiaFragmentation( ) ;
            // store parton event
            this -> savePartonEvent( ) ;
            // hadronization process
            this -> hadronize( ) ;
            // convert to StdHep
            this -> convertToStdHep ( ) ;
            // Create HepMC event
            mcevt = new HepMCEvent(name(),1,1);
            evt = mcevt->pGenEvt();
            status = this->fillEvt(evt);
            if ( ! status.isSuccess( ) ) 
              return Error( "Failed to fill event" , status ) ;

            evt->set_signal_process_id( procType ) ;
            // Reset hard process info
            this->hardProcessInfo(hard);
            hard->setEvent(mcevt);
            
            // stop hadronization when a B of interest is found
            // in order not to bias the angular distribution :
            // an event is skipped when it has a B of correct type 
            // outside the angular acceptance. We then look for the
            // next bb event.
            // However before generating another minimum bias event
            // we need to rehadronize the bb event (because it contains
            // necessarily a B of correct type, then keeping it
            // would bias the double bb events samples)
            if ( m_skipEvent ) break ;
            
            // analyse event
            try { m_bfound = this -> selectEvt( evt ) ; }
            catch ( const GaudiException & exc ) {
              Exception( "Error in selectEvt" , exc ) ;
            }                
            
            if ( m_bfound ) evt -> set_event_scale( 1.0 ) ;
            
            if ( ( ! m_bfound ) && ( numberOfRep == m_maxNumberOfRep ) ) {
              info() << "Repeated hadronization " 
                     << numberOfRep << " times." << endreq ;
              info() << "Will reject this event and look"
                     << " for the next bb one" << endreq ;
              m_numberOfExcessHad++ ;
            }
          }
        }   
      }
      
      // put the HepMC event into a temporary vector
      tempHepMC.push_back( mcevt ) ;  
      // put the hard Scatter info in a temporary vector
      tempHard.push_back( hard );
    }
    
    
    eventIsOK = ( !m_signal && !m_binclus ) 
      || ( m_signal && m_bfound ) || ( m_binclus && m_bfound );
    
  } while ( ! eventIsOK ) ;

  m_numbOfAcceptedInt = m_numbOfAcceptedInt + numToGenerate ;
  m_numberOfAcceptedBInteractions += m_bCounter ;
  m_numberOfAcceptedDoubleBInteractions += m_fourbCounter ;
  // if requested (for signal) run a selection for particles
  // inside acceptance

  /////////////
  /////////////

  // once a satisfactory event (with pile-up) has been created,
  // create the GenHeader and then
  // copy the temporary vector into the transient store

  GenHeader* genHead = new GenHeader();
  
  genHead->setLuminosity(rlumnow*1000*1.e29/(cm2*s));
  genHead->setEvType(m_evnType);

  StatusCode sc = put( genHead , GenHeaderLocation::Default ) ;
  
  if ( ! sc.isSuccess() )
    return Error( "GenHeader could not be registered!" , sc ) ;
  else
    debug() << "Processing event type " << m_evnType << endreq ;

  EventVector::iterator ithep;
  for( ithep = tempHepMC.begin() ; tempHepMC.end() != ithep ; ++ithep ) {
    HepMCEvents * anhepMCVector = 
      getOrCreate< HepMCEvents , HepMCEvents >( HepMCEventLocation::Default );
    anhepMCVector -> insert( *ithep ) ;
  }
  
  // add to the event store the hard process information
  HardVector::iterator itHard ;
  for( itHard = tempHard.begin(); tempHard.end() != itHard; ++itHard ) {
    HardInfos * hardVector = 
      getOrCreate< HardInfos , HardInfos >( HardInfoLocation::Default ) ;
    hardVector -> insert( *itHard ) ;
  }    
  
  return status;
}

//===========================================================================
// Finalize method
//===========================================================================
StatusCode GenBaseAlg::finalize() {
  StatusCode status = this -> preFinalize( ) ;
  
  if ( status.isSuccess( ) ) status = this -> genFinalize( ) ;
  
  // Print counters
  info() << endreq ;
  info() << "************************************************** " << endreq ;
  info() << "***********   Generation counters   **************"  << endreq ;
  info() << "Number of bunch crossings                        : " 
         << m_numbOfBunchX << endreq;
  info() << "Number of non-empty events                       : " 
         << m_numbOfEvents << endreq;
  info() << "Number of interactions                           : " 
         << m_numbOfInter << endreq;
  info() << "Number of accepted  events                       : " 
         << m_numbOfAcceptedEvt << endreq;
  info() << "Number of interactions within the accepted events: " 
         << m_numbOfAcceptedInt << endreq;
  info() << "Number of z-transformed events                   : " 
         << m_numbOfZminusZEvt << endreq ;
  info() << "  - number of B    z-transformed events          : "
         << m_numbOfZminusZEvtB << endreq ;
  info() << "  - number of Bbar z-transformed events          : "
         << m_numbOfZminusZEvtBbar << endreq ;
  info() << "Number of generated interactions with a b-quark  : "
         << m_numberOfBInteractions << endreq ;
  info() << "Number of accepted interactions with a b-quark   : "
         << m_numberOfAcceptedBInteractions << endreq ;
  info() << "Number of accepted interactions with 2 bb pairs  : "
         << m_numberOfAcceptedDoubleBInteractions << endreq ;
  info() << "Number of repetions of hadronization             : "
         << m_numberOfRepetitions << endreq ;
  info() << "Number of times the hadronization is repeated "
         << endreq ;
  info() << "      more than the limit ( " 
         << m_maxNumberOfRep << " )               : "
         << m_numberOfExcessHad << endreq ;
  info() << "Number of B/Bbar before angular test             : "
         << m_numberBPresentedAngularCut << endreq ;
  info() << "  - number of B    before angular test           : "
         << m_numberBPresentedAngularCutB << endreq ;
  info() << "  - number of Bbar before angular test           : "
         << m_numberBPresentedAngularCutBbar << endreq ;
  info() << "Number of B/Bbar after angular test              : "
         << m_numberBAcceptedAngularCut << endreq ;
  info() << "  - number of B    after angular test            : "
         << m_numberBAcceptedAngularCutB << endreq ;
  info() << "  - number of Bbar after angular test            : "
         << m_numberBAcceptedAngularCutBbar << endreq ;
  info() << "Number of B/Bbar before flip test                : "
         << m_numberOfBBeforeFlip << endreq ;
  info() << "  - number of B    before flip test              : "
         << m_numberOfBBeforeFlipB << endreq ;
  info() << "  - number of Bbar before flip test              : "
         << m_numberOfBBeforeFlipBbar << endreq ;  
  info() << "Number of flip accepted B                        : "
         << m_numberOfFlipAccepted << endreq ;
  info() << "  - number of flip accepted B                    : "
         << m_numberOfFlipAcceptedB << endreq ;
  info() << "  - number of flip accepted Bbar                 : "
         << m_numberOfFlipAcceptedBbar << endreq ;
  info() << "************************************************** "
         << endreq ;
  info() << endreq ;

  randgaudi.finalize( ) ;

  return GaudiAlgorithm::finalize( ) ;
}

//===========================================================================
// Basic select event function. It should be overriden to perform more
// specific actions
//===========================================================================
bool GenBaseAlg::selectEvt( HepMC::GenEvent* evt) {
  HepMCPIterator iter ;
  for ( iter = evt->particles_begin() ; iter!=evt->particles_end(); ++iter ) {
    PIDs::iterator hiter ;
    
    for ( hiter = m_hadrons.begin() ; m_hadrons.end()!=hiter ; ++hiter ) 
      if ( 0 != *hiter ) 
        if ( (*iter)->pdg_id() == *hiter ) {
          double px = (*iter) -> momentum().px();
          double py = (*iter) -> momentum().py();
          double pz = (*iter) -> momentum().pz();
          
          double pp = sqrt(px*px+py*py+pz*pz);          
          double theta = acos(fabs(pz)/pp);
          
          m_numberBPresentedAngularCut++ ;
                  
          //check if the requested b hadron is within the 
          // acceptance angle
          if (theta <= m_thetamax  && pp>=m_pmin) { 
            m_numberBAcceptedAngularCut++ ;
            //check if it is on the right side
            if ( pz<0 ) {
              double xPart, yPart, zPart, tPart ;
              double pxPart, pyPart, pzPart, ePart ;
              //do the 'parity trick' (z -> -z, pz -> -pz)
              
              HepMCVIterator vitr ;
              for ( vitr=evt->vertices_begin() ; vitr!=evt->vertices_end() ; 
                    ++vitr ) {                      
                xPart = (*vitr)->position().x();
                yPart = (*vitr)->position().y();
                zPart = (*vitr)->position().z();
                tPart = (*vitr)->position().e();
                
                (*vitr) -> set_position(HepLorentzVector( xPart , yPart , 
                                                          -zPart , tPart ) ) ;
              }
              
              HepMCPIterator pitr ;
              for ( pitr = evt->particles_begin() ; 
                    pitr != evt->particles_end() ; ++pitr ) {
                pxPart = (*pitr)->momentum().px();
                pyPart = (*pitr)->momentum().py();
                pzPart = (*pitr)->momentum().pz();
                ePart  = (*pitr)->momentum().e();
                
                (*pitr) -> set_momentum(HepLorentzVector( pxPart , pyPart ,
                                                          -pzPart , ePart ) ) ;
              }
              //increase the counter of the 'z-transformed' events
              m_numbOfZminusZEvt++;
            }
            
            // This is the first B which is in acceptance
            // and which has the ID of the signal B to
            // generate, give it status 889
            // to recognize him more easily
            if ( m_forcedFragmentation ) (*iter) -> set_status( 889 ) ;
            
            return true;
          }
        }    
  }  
  return false;
}
