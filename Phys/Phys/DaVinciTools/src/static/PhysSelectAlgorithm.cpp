// $Id: PhysSelectAlgorithm.cpp,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $

// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/Event.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "PhysEvent/PhysSel.h"

#include "PhysSelectAlgorithm.h"

//--------------------------------------------------------------------
// Implementation file for class : PhysSelectAlgorithm
// 
// Wrapper of the axselect package
//
// 05/05/2001 : Gloria Corti
//--------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const AlgFactory<PhysSelectAlgorithm>    Factory;
const IAlgFactory& PhysSelectAlgorithmFactory = Factory;

// List of called Fortran routines
extern "C" {
#ifdef WIN32
  void __stdcall AXSELCONFIG( int* iSELModeValue, int* iSELLevValue,
                              int* iSELTagValue, int* iFLGPrntValue, 
                              int* iFLGHistValue, const char* decayNameValue, 
                              int decayNameLenght );
  void __stdcall AXSELINIT( int* istatus );
  void __stdcall FHFILEOPEN( int* istatus, const char* decayNameValue, 
                             int decayNameLenght, const char* histFname,
                             int histFnameLenght );
  void __stdcall AXSELINERR( int* istatus, const char* decayNameValue,
                             int DecayNameLenght );
  void __stdcall AXSELECT( int* iMCFlag, int* iSELFlag, int* iAGRFlag, 
                           int* iTAGFlag );
  void __stdcall AXSELRESULTS( int* iMCFlag, int* iTKRFlag, int* iPQUALFlag,
                           int* iSELFlag, int* iAGRFlag, int* iTAGFlag ); 
  void __stdcall AXSELLAST( void );
  void __stdcall FHFILEEND( const char* decayNameValue, int decayNameLenght );
  void __stdcall SETCDFFNAME( int* istat, const char* decayNameValue, 
                              int decayNameLenght, 
                              const char* cdfFname, int cdfFnameLenght );
  void __stdcall AXSELIDCODE( const char* decayNameValue, int decayNameLenght,
                              int* decayCode );
#else
  void axselconfig_( int* iSELModeValue, int* iSELLevValue,
                     int* iSELTagValue, int* iFLGPrntValue,
                     int* iFLGHistValue, const char* decayNameValue, 
                     int decayNameLenght );
  void axselinit_( int* istatus );
  void fhfileopen_( int* istatus, const char* decayNameValue,
                    const char* histFname,int DecayNameLenght, 
                    int histFnameLenght );
  void axselinerr_( int* istatus, const char* decayNameValue, 
                    int DecayNameLenght );
  void axselect_( int* iMCFlag, int* iSELFlag, int* iAGRFlag, int* iTAGFlag );
  void axselresults_( int* iMCFlag, int* iTKRFlag, int* iPQUALFlag,
                      int* iSELFlag, int* iAGRFlag, int* iTAGFlag );
  void axsellast_( void );
  void fhfileend_( const char* decayNameValue, int decayNameLenght );
  void setcdffname_( int* istat, const char* decayNameValue, 
                     const char* cdfFname, 
                     int decayNameLenght, int cdfFnameLenght );
  void axselidcode_( const char* decayNameValue, int* decayCode,
                     int decayNameLenght );

#define AXSELCONFIG axselconfig_
#define AXSELINIT axselinit_
#define FHFILEOPEN fhfileopen_
#define FHFILEEND fhfileend_
#define AXSELINERR axselinerr_
#define AXSELECT axselect_
#define AXSELRESULTS axselresults_
#define AXSELLAST axsellast_
#define SETCDFFNAME setcdffname_
#define AXSELIDCODE axselidcode_
#endif
}

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
PhysSelectAlgorithm::PhysSelectAlgorithm(const std::string& name, 
                                     ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_decayName(" "),
  m_selMode(0),
  m_selLevel(0),  
  m_selTag(0),
  m_selHisto(0), 
  m_selCutsFname("default"),
  m_selHistFname("axsel.hbook"),
  m_decayCode(0),
  m_nEvents(0) {

  // Declare properties replacing the AXSE setting when running with
  // SICB main program
  declareProperty( "DecayName", m_decayName );
  declareProperty( "SelMode", m_selMode );
  declareProperty( "SelLevel", m_selLevel );
  declareProperty( "FlavourTag", m_selTag );
  declareProperty( "Histograms", m_selHisto );
  declareProperty( "CutsFileName", m_selCutsFname );
  declareProperty( "HistFileName", m_selHistFname );
}

//=============================================================================
// Standard destructor
//=============================================================================
PhysSelectAlgorithm::~PhysSelectAlgorithm() { };


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PhysSelectAlgorithm::initialize() {

  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initializing the AXSEL package" << endreq;

  // Configure the AXSEL package, first retrieve the parameter from the
  // job option
  int iselPrint = 0;
  IntegerProperty outLevel( "OutputLevel", 0 );
  StatusCode sc = this->getProperty( &outLevel );
  if( sc.isFailure() ) {
    log << MSG::ERROR << "    Failed to retrieve OutputLevel "
        << "- keep default in FORTRAN" << endreq;
  }
  else {
    if ( MSG::DEBUG == outLevel.value() ) {
      iselPrint = 2;
    }
    if ( MSG::INFO == outLevel.value() ) {
      iselPrint = 1;
    }
  }
  
  AXSELCONFIG( &m_selMode, &m_selLevel, &m_selTag, &iselPrint, &m_selHisto,
               m_decayName.data(), m_decayName.length() );

  // Open the histogram file where to put axsel monitoring histograms
  int istatus = 0;
  if( 0 != m_selHisto ) {
#ifdef WIN32
    FHFILEOPEN( &istatus, m_decayName.data(), m_decayName.length(),
                          m_selHistFname.data(), m_selHistFname.length() );
#else
    FHFILEOPEN( &istatus, m_decayName.data(), m_selHistFname.data(),
                          m_decayName.length(), m_selHistFname.length() );
#endif    
    if( 0 != istatus ) {
      log << MSG::FATAL << "   Failed to initialize the AXSEL package" 
          << endreq;
      istatus = 2;
      AXSELINERR( &istatus, m_decayName.data(), m_decayName.length() );
      return StatusCode::FAILURE;
    }
  }

  // Use the cdf file from the job options if different from default
  istatus = 0;
  if( "default" != m_selCutsFname ) { 
    log << MSG::DEBUG << "   Reading cuts from " << m_selCutsFname << endreq;
#ifdef WIN32
    SETCDFFNAME( &istatus, m_decayName.data(), m_decayName.length(), 
                 m_selCutsFname.data(), m_selCutsFname.length() );
#else
    SETCDFFNAME( &istatus, m_decayName.data(), m_selCutsFname.data(), 
                 m_decayName.length(), m_selCutsFname.length() );
#endif
    if( 0 != istatus ) {
      log << MSG::FATAL << "   Unable to find the cuts file" << endreq;
      return StatusCode::FAILURE;
    }
  }

  // Now initialize the package
  istatus = 1;
  AXSELINIT( &istatus );
  if( istatus > 0 ) {
    log << MSG::FATAL << "    Failed to initialize the AXSEL package" << endreq;
    int istat = istatus;
    AXSELINERR( &istat , m_decayName.data() , m_decayName.length() );
    return StatusCode::FAILURE;
  }
  
  // Retrieve decay Code as used in bookkeeping database
#ifdef WIN32
  AXSELIDCODE( m_decayName.data(), m_decayName.length(), &m_decayCode );
#else
  AXSELIDCODE( m_decayName.data(), &m_decayCode, m_decayName.length() );
#endif

  // Initialization terminated
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode PhysSelectAlgorithm::execute() {

  MsgStream          log( msgSvc(), name() );

  // Counter of events processed
  log << MSG::DEBUG << ">>> Processing event " << ++m_nEvents << endreq;

  StatusCode         sc = StatusCode::SUCCESS;

  // Call the AXSEL package for each event
  // the following will containg the results of the AXSEL package 
  int iMCFlag = 0;
  int iTKRFlag = 0;
  int iPQUALFlag = 0;
  int iSELFlag = 0;
  int iAGRFlag = 0;
  int iTAGFlag = 0;
  AXSELECT( &iMCFlag, &iSELFlag, &iAGRFlag, &iTAGFlag );
  // Now retrieve ALL results
  AXSELRESULTS( &iMCFlag, &iTKRFlag, &iPQUALFlag,
                &iSELFlag, &iAGRFlag, &iTAGFlag ); 
  // Fortran logicals correspond to C integer and they now need to be 
  // converted to C++ bool
  bool lMCFlag = (bool)iMCFlag;
  bool lTKRFlag = (bool)iTKRFlag;
  bool lPQUALFlag = (bool)iPQUALFlag;
  bool lSELFlag = (bool)iSELFlag;
  bool lAGRFlag = (bool)iAGRFlag;
  bool lTAGFlag = (bool)iTAGFlag;
  if( lMCFlag ) {
    log << MSG::INFO << "  The event contain the MC decay requested" << endreq;
  }
  if( lTKRFlag ) {
    log << MSG::INFO << " The MC decay has all end tracks reconstructed" 
        << endreq;
  }
  if( lPQUALFlag ) {
    log << MSG::INFO << " The MC decay end tracks are of physics quality"
        << endreq;
  }
  if( lSELFlag ) {
    log << MSG::INFO << "  The event passed the decay selection" << endreq;
  }
  if( lAGRFlag ) {
    log << MSG::INFO << "  At least one selected combination is the "
        << "real MC decay" << endreq;
  }
  if( lTAGFlag ) {
    log << MSG::INFO << "  The event is also flavour tagged" << endreq;
  }

  // At this point store the information in a data class
  // and put it in the transient store
  PhysSel* result = new PhysSel();
  PhysSelDecay* decay = new PhysSelDecay();
  decay->setDecayID(m_decayCode);
  decay->setResults(lMCFlag,lTKRFlag,lPQUALFlag,lSELFlag,lAGRFlag,lTAGFlag);
  result->addPhysSelDecay( m_decayName, decay );
  sc = eventSvc()->registerObject("/Event/Phys","Selection",result);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to store PhysSelection results for " 
        << m_decayName << " in /Event/Phys/Selection" << endreq;
  }
  
  // End of execution for each event
  return sc;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode PhysSelectAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Finalizing the Physics Selection package" << endreq;
  log << MSG::INFO << "    Number of events processed      = "
      << m_nEvents << endreq;
  log << MSG::INFO << "    Finalizing AXSEL " << endreq;

  // Call the print out summary
  AXSELLAST();

  // Close histogram file if requested
  if( 0 != m_selHisto ) {
    FHFILEEND( m_decayName.data(), m_decayName.length() );
  }
 
  // End of finalization step
  return StatusCode::SUCCESS;
}
