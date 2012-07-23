// $Id: BcVegPyProduction.cpp,v 1.5 2009-09-28 19:40:14 jhe Exp $
// Include files 

// local
#include "BcVegPyProduction.h"
#include "LbBcVegPy/BcVegPy.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from Event
#include "Event/GenCollision.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "LbPythia/Pythia.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BcVegPyProduction
//
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BcVegPyProduction );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BcVegPyProduction::BcVegPyProduction( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
   : PythiaProduction ( type, name , parent ) {

  declareInterface< IProductionTool >( this ) ;

  declareProperty( "BcVegPyCommands", m_commandBcVegPyVector) ;
  //set the default setting of BcVegPy here
  m_defaultBcVegPySettings.clear();
  m_defaultBcVegPySettings.push_back( "mixevnt imix 0");
  m_defaultBcVegPySettings.push_back( "mixevnt imixtype 1");
  m_defaultBcVegPySettings.push_back( "counter ibcstate 1");  //Bc state
  m_defaultBcVegPySettings.push_back( "upcom pmb 4.95");       //mass of b quark
  m_defaultBcVegPySettings.push_back( "upcom pmc 1.326");   //mass of c quark 
  m_defaultBcVegPySettings.push_back( "upcom pmbc 6.276");  
  //mass of B_c, note that pmBc=pmB+pmC
  m_defaultBcVegPySettings.push_back( "upcom ecm 14000.0");   //E.C.M. of LHC
  m_defaultBcVegPySettings.push_back( "confine ptcut 0.0");
  m_defaultBcVegPySettings.push_back( "confine etacut 1000000000.0");
  m_defaultBcVegPySettings.push_back( "funtrans nq2 3"); 
  m_defaultBcVegPySettings.push_back( "funtrans npdfu 2");
  m_defaultBcVegPySettings.push_back( "loggrade ievntdis 0");
  m_defaultBcVegPySettings.push_back( "loggrade igenerate 0");
  m_defaultBcVegPySettings.push_back( "loggrade ivegasopen 0");
  m_defaultBcVegPySettings.push_back( "loggrade igrade 1");
  m_defaultBcVegPySettings.push_back( "loggrade iusecurdir 0");
  //
  m_defaultBcVegPySettings.push_back( "subopen subenergy 100.0");
  m_defaultBcVegPySettings.push_back( "subopen isubonly 0");
  m_defaultBcVegPySettings.push_back( "usertran ishower 1");
  m_defaultBcVegPySettings.push_back( "usertran idpp 3");
  m_defaultBcVegPySettings.push_back( "vegasinf number 1000000");
  m_defaultBcVegPySettings.push_back( "vegasinf nitmx 20");
  m_defaultBcVegPySettings.push_back( "vegcross iveggrade 0"); 
  //should be fixed to 0
  m_defaultBcVegPySettings.push_back( "qqbar iqqbar 0");
  m_defaultBcVegPySettings.push_back( "qqbar iqcode 2");
  m_defaultBcVegPySettings.push_back( "outpdf ioutpdf 0");     
  m_defaultBcVegPySettings.push_back( "outpdf ipdfnum 300");   
  //should be fixed to 300 for this version
  m_defaultBcVegPySettings.push_back( "coloct ioctet 0");
  m_defaultBcVegPySettings.push_back( "octmatrix coeoct 0.1");
}

//=============================================================================
// Destructor 
//=============================================================================
BcVegPyProduction::~BcVegPyProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode BcVegPyProduction::initialize( ) {
 //Change the parameter to 2, for BcVegPy,....

  m_userProcess = 2 ;

  // User process
  m_frame = "USER" ;
  m_beam = "p+";
  m_target = "p+" ;

  // Set default BcVegPy settings
  StatusCode  sc = parseBcVegPyCommands( m_defaultBcVegPySettings ) ;

  // read BcVegPy command vector from job options
  sc = parseBcVegPyCommands( m_commandBcVegPyVector ) ;

  if ( ! sc.isSuccess( ) ) 
  return Error( "Unable to read BcVegPy commands" , sc ) ;

  BcVegPy::SetParameter( );

  //Initialize of Pythia done here
  sc = PythiaProduction::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> BcVegPy
//=============================================================================
StatusCode BcVegPyProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;

  //Pythia::PyList(2);

  return StatusCode::SUCCESS ;
}


//=============================================================================
// Parse BcVegPy commands stored in a vector
//=============================================================================
StatusCode BcVegPyProduction::parseBcVegPyCommands( const CommandVector & 
                                                  theCommandVector ) {
  //
  // Parse Commands and Set Values from Properties Service...
  //
  CommandVector::const_iterator iter ;
  for ( iter = theCommandVector.begin() ; theCommandVector.end() != iter ; 
        ++iter ) {
    debug() << " Command is: " << (*iter) << endmsg ;
    StringParse mystring( *iter ) ;
    std::string block = mystring.piece(1);
    std::string entry = mystring.piece(2);
    std::string str   = mystring.piece(3);
    int    int1  = mystring.intpiece(3);
    double fl1   = mystring.numpiece(3);

    
    // Note that BcVegPy needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << endmsg ;

    if ( "mixevnt" == block)
      if      ( "imix"    == entry )BcVegPy::mixevnt().imix()      = int1 ;
      else if ( "imixtype"== entry ) BcVegPy::mixevnt().imixtype() = int1 ;
      else return Error(std::string("BcVegPy error, mixevnt"));

    else if ( "counter" == block)
      if      ( "ibcstate"    == entry ) BcVegPy::counter().ibcstate() = int1 ;
      else return Error(std::string("BcVegPy error, counter"));
			
    else if ( "upcom" == block)
      if      ( "pmb" == entry ) BcVegPy::upcom().pmb()     =fl1;
      else if ( "pmc" == entry ) BcVegPy::upcom().pmc()     =fl1;
      else if ( "pmbc"== entry ) BcVegPy::upcom().pmbc()    =fl1;
      else if ( "ecm" == entry ) BcVegPy::upcom().ecm()     =fl1;
      else return Error (std::string("BcVegPy error, upcom"));

    else if ( "confine" == block )
      if      ( "ptcut" == entry  ) BcVegPy::confine().ptcut() = fl1;
      else if ( "etacut" == entry ) BcVegPy::confine().etacut()= fl1;
      else return Error (std::string("BcVegPy error, confine"));
    
    else if ( "funtrans"== block )
      if      ("nq2"  ==entry)BcVegPy::funtrans().nq2()           =int1;
      else if ("npdfu"==entry)BcVegPy::funtrans().npdfu()         =int1;
      else return Error (std::string("BcVegPy error, funtrans"));
    
    else if ( "loggrade"==block )
      if      ("ievntdis"  ==entry ) BcVegPy::loggrade().ievntdis()   =int1;
      else if ("igenerate" ==entry ) BcVegPy::loggrade().igenerate()  =int1;
      else if ("ivegasopen"==entry ) BcVegPy::loggrade().ivegasopen() =int1;
      else if ("igrade"    ==entry ) BcVegPy::loggrade().igrade()     =int1;
      else if ("iusecurdir"==entry ) BcVegPy::loggrade().iusecurdir() =int1;
      else return Error (std::string("BcVegPy error, loggrade"));
    
    else if ( "subopen"==block )
      if       ( "subenergy"==entry)BcVegPy::subopen().subenergy()   =fl1;
      else if  ( "isubonly" ==entry)BcVegPy::subopen().isubonly()    =int1; 
      else return Error (std::string("BcVegPy error, subopen"));
    
    else if ( "usertran"==block )
      if       ( "ishower"==entry)BcVegPy::usertran().ishower()=int1;
      else if  ( "idpp"   ==entry)BcVegPy::usertran().idpp()=int1; 
      else return Error (std::string("BcVegPy error, usertran"));
    
    else if ( "vegasinf"==block )
      if      ( "number"==entry )BcVegPy::vegasinf().number()         =int1;
      else if ( "nitmx" ==entry )BcVegPy::vegasinf().nitmx()          =int1;
      else return Error (std::string("BcVegPy error, vagasinf"));
    
    else if ( "vegcross"==block )
      if ("iveggrade"==entry)BcVegPy::vegcross().iveggrade()          =int1;
      else return Error (std::string("BcVegPy error, vegcross"));
    
    else if ( "qqbar"==block )
      if       ( "iqqbar"==entry)BcVegPy::qqbar().iqqbar()     =int1;
      else if  ( "iqcode"==entry)BcVegPy::qqbar().iqcode()     =int1; 
      else return Error (std::string("BcVegPy error, qqbar"));
    
    else if ( "outpdf"==block )
      if       ( "ioutpdf"==entry)BcVegPy::outpdf().ioutpdf()  =int1;
      else if  ( "ipdfnum"==entry)BcVegPy::outpdf().ipdfnum()  =int1; 
      else return Error (std::string("BcVegPy error, outpdf"));
    
    else if ( "coloct" == block)
      if      ( "ioctet"    == entry ) BcVegPy::coloct().ioctet()    = int1 ;
      else return Error(std::string("BcVegPy error, coloct"));
    
    else if ( "octmatrix" == block)
      if      ( "coeoct"    == entry ) BcVegPy::octmatrix().coeoct() = fl1 ;
      else return Error(std::string("BcVegPy error, octmatrix"));
    
    else return Error (std::string("BcVegPy error in parse parameters"));
  }
  
  return StatusCode::SUCCESS ;
}
