// $Id: GenXiccProduction.cpp,v 1.0 2011-04-11 F. Zhang
// Include files 

// local
#include "GenXiccProduction.h"
#include "LbGenXicc/GenXicc.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/GenCollision.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "LbPythia/Pythia.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenXiccProduction
//
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( GenXiccProduction );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenXiccProduction::GenXiccProduction( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
   : PythiaProduction ( type, name , parent ) {

  declareInterface< IProductionTool >( this ) ;

  declareProperty( "GenXiccCommands", m_commandGenXiccVector) ;
  //set the default setting of GenXicc here
  //THE INTERPERATIONS OF THESE VARIABLES CAN BE FOUND IN "COMP. PHYS. COMMUNI. 177(2007)467-478"
  //AND IN "README   " UNDER "DOC" DIRECTORY
  m_defaultGenXiccSettings.clear();
  //COMMON BLOCK /MIXEVNT/IMIX,IMIXTYPE 
  m_defaultGenXiccSettings.push_back( "mixevnt imix 0");
  m_defaultGenXiccSettings.push_back( "mixevnt imixtype 1");
  //COMMON BLOCK /COUNTER/IXICCSTATE,NEV
  m_defaultGenXiccSettings.push_back( "counter ixiccstate 1");  //Xi state
  //COMMON BLOCK /UPCOM/ECM,PMXICC,PMB,PMC,FXICC,PMOMUP(5,8)
  //                    COLMAT(6,64),BUNDAMP(4),PMOMZERO(5,8)
  //                    DOUBLE COMPLEX COLMAT,BUNDAMP
  m_defaultGenXiccSettings.push_back( "upcom pmb 5.10");       //mass of b quark
  m_defaultGenXiccSettings.push_back( "upcom pmc 1.75");   //mass of c quark 
  m_defaultGenXiccSettings.push_back( "upcom pmxicc 3.50");  
  //                    mass of Xi, note that pmxicc=pmb+pmc exactly
  m_defaultGenXiccSettings.push_back( "upcom ecm 7000.0");   //E.C.M. of LHC
  //COMMON BLOCK /CONFINE/PTCUT,ETACUT,PSETACUT
  m_defaultGenXiccSettings.push_back( "confine ptcut 0.0");
  m_defaultGenXiccSettings.push_back( "confine etacut 1000000000.0");
  //COMMON BLOCK /FUNTRANS/NPDFU
  m_defaultGenXiccSettings.push_back( "funtrans npdfu 2");
  //COMMON BLOCK /LOGGRADE/IVENTDIS,IGENERATE,IVEGASOPEN,IGRADE
  m_defaultGenXiccSettings.push_back( "loggrade ievntdis 0"); //SWITCH ON/OFF TO GET THE EVNT NUM.
  m_defaultGenXiccSettings.push_back( "loggrade igenerate 0"); 
  //                    whether generating full events used when idwtup=1
  m_defaultGenXiccSettings.push_back( "loggrade ivegasopen 0");
  m_defaultGenXiccSettings.push_back( "loggrade igrade 1");
  //                    whether generating on the GRID
  m_defaultGenXiccSettings.push_back( "loggrade iusecurdir 0");
  //COMMON BLOCK /SUBOPEN/SUBFACTOR,SUBENERGY,ISUBONLY
  m_defaultGenXiccSettings.push_back( "subopen subenergy 100.0");//GENERALLY USELESS
  m_defaultGenXiccSettings.push_back( "subopen isubonly 0");
  //COMMON BLOCK /USERTRAN/IDPP
  m_defaultGenXiccSettings.push_back( "usertran idpp 3"); //=IDWTUP
  //COMMON BLOCK /VEGASINF/NUMBER,NITMX
  m_defaultGenXiccSettings.push_back( "vegasinf number 1000000");
  m_defaultGenXiccSettings.push_back( "vegasinf nitmx 20");
  //COMMON BLOCK /VEGCROSS/VEGSEC,VEGERR,IVEGGRADE
  m_defaultGenXiccSettings.push_back( "vegcross iveggrade 0"); 
  //COMMON BLOCK /OUTPDF/IOUTPDF,IPDFNUM
  m_defaultGenXiccSettings.push_back( "outpdf ioutpdf 1");  
  //                     ioutpdf = 0  <==>  inner PDFs (proton parton-dis func.) used in PYTHIA
  //                                       (CTEQ3M, CTEQ5L, CTEQ5M)
  //                             mstp(51)=  2       7       8
  //                     ioutpdf = 1  <==>  outer PDFs used
  //                                       (CTEQ6HQ,GRV98L, MRST2001L)                  
  //                             ipdfnum =  100     200     300          
  m_defaultGenXiccSettings.push_back( "outpdf ipdfnum 300");   
  //COMMON BLOCK /VEGASBIN/NVBIN
  m_defaultGenXiccSettings.push_back( "vegasbin nvbin 300");   
  //COMMON BLOCK /VALMATRIX/CMFACTOR
  m_defaultGenXiccSettings.push_back( "valmatrix cmfactor 1.0");
  //COMMON BLOCK /MTYPROFXI/MEGENXI
  m_defaultGenXiccSettings.push_back( "mtypeofxi mgenxi 1");
  //                        mgenxi = 1 for Xi_cc; = 2 for Xi_bc; =3 for Xi_cc;
  //COMMON BLOCK /WBSTATE/RATIOU,RATIOD,RATIOS,NBOUND
  m_defaultGenXiccSettings.push_back( "wbstate nbound 1");
  //                      nbound = 1 for Xi_{cc}^{++} or Xi_{bc}^{+} or Xi_{bb}^{0}
  //                             = 2 for Xi_{cc}^{+} or Xi_{bc}^{0} or Xi_{bb}^{-}
  //                             = 3 for Omega_{cc}^{+} or Omega_{bc}^{0} or Omega_{bb}^{-}
}

//=============================================================================
// Destructor 
//=============================================================================
GenXiccProduction::~GenXiccProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode GenXiccProduction::initialize( ) {
 //Change the parameter to 7, for GenXicc,....
 //The 7th external process of GAUSS by F. ZHANG 29-Oct-2011
 //in the older version of GAUSS, this parameter should equal to 6  

  m_userProcess = 7 ;

  // User process
  m_frame = "USER" ;
  m_beam = "p+";
  m_target = "p+" ;

  // Set default GenXicc settings
  StatusCode  sc = parseGenXiccCommands( m_defaultGenXiccSettings ) ;

  // read GenXicc command vector from job options
  sc = parseGenXiccCommands( m_commandGenXiccVector ) ;

  if ( ! sc.isSuccess( ) ) 
  return Error( "Unable to read GenXicc commands" , sc ) ;

  GenXicc::SetParameter( );
  GenXicc::EvntInit( );

  //Initialize of Pythia done here
  sc = PythiaProduction::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  return sc ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> GenXicc
//=============================================================================
StatusCode GenXiccProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision )
{
  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;

  //Pythia::PyList(7);
  //Pythia::PyList(1);

  return StatusCode::SUCCESS ;
}


//=============================================================================
// Parse GenXicc commands stored in a vector
//=============================================================================
StatusCode GenXiccProduction::parseGenXiccCommands( const CommandVector & 
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

    
    // Note that GenXicc needs doubles hence the convert here
    debug() << block << " block  " << entry << " item  " << int1 
            << "  value " << fl1 << endmsg ;

    if ( "mixevnt" == block)
      if      ( "imix"    == entry )GenXicc::mixevnt().imix()      = int1 ;
      else if ( "imixtype"== entry ) GenXicc::mixevnt().imixtype() = int1 ;
      else return Error(std::string("GenXicc error, mixevnt"));

    else if ( "counter" == block)
      if      ( "ixiccstate"    == entry ) GenXicc::counter().ixiccstate() = int1 ;
      else return Error(std::string("GenXicc error, counter"));
			
    else if ( "upcom" == block)
      if      ( "pmb" == entry ) GenXicc::upcom().pmb()     =fl1;
      else if ( "pmc" == entry ) GenXicc::upcom().pmc()     =fl1;
      else if ( "pmxicc"== entry ) GenXicc::upcom().pmxicc()  =fl1;
      else if ( "ecm" == entry ) GenXicc::upcom().ecm()     =fl1;
      else return Error (std::string("GenXicc error, upcom"));

    else if ( "confine" == block )
      if      ( "ptcut" == entry  ) GenXicc::confine().ptcut() = fl1;
      else if ( "etacut" == entry ) GenXicc::confine().etacut()= fl1;
      else return Error (std::string("GenXicc error, confine"));
    
    else if ( "funtrans"== block )
      if ("npdfu"==entry)GenXicc::funtrans().npdfu()         =int1;
      else return Error (std::string("GenXicc error, funtrans"));
    
    else if ( "loggrade"==block )
      if      ("ievntdis"  ==entry ) GenXicc::loggrade().ievntdis()   =int1;
      else if ("igenerate" ==entry ) GenXicc::loggrade().igenerate()  =int1;
      else if ("ivegasopen"==entry ) GenXicc::loggrade().ivegasopen() =int1;
      else if ("igrade"    ==entry ) GenXicc::loggrade().igrade()     =int1;
      else if ("iusecurdir"==entry ) GenXicc::loggrade().iusecurdir() =int1;
      else return Error (std::string("GenXicc error, loggrade"));
    
    else if ( "subopen"==block )
      if       ( "subenergy"==entry)GenXicc::subopen().subenergy()   =fl1;
      else if  ( "isubonly" ==entry)GenXicc::subopen().isubonly()    =int1; 
      else return Error (std::string("GenXicc error, subopen"));
    
    else if ( "usertran"==block )
      if  ( "idpp"   ==entry)GenXicc::usertran().idpp()=int1; 
      else return Error (std::string("GenXicc error, usertran"));
    
    else if ( "vegasinf"==block )
      if      ( "number"==entry )GenXicc::vegasinf().number()         =int1;
      else if ( "nitmx" ==entry )GenXicc::vegasinf().nitmx()          =int1;
      else return Error (std::string("GenXicc error, vagasinf"));
    
    else if ( "vegcross"==block )
      if ("iveggrade"==entry)GenXicc::vegcross().iveggrade()          =int1;
      else return Error (std::string("GenXicc error, vegcross"));
    
    else if ( "outpdf"==block )
      if       ( "ioutpdf"==entry)GenXicc::outpdf().ioutpdf()  =int1;
      else if  ( "ipdfnum"==entry)GenXicc::outpdf().ipdfnum()  =int1; 
      else return Error (std::string("GenXicc error, outpdf"));
      
    else if ( "vegasbin"==block )
      if  ( "nvbin"   ==entry)GenXicc::vegasbin().nvbin()=int1; 
      else return Error (std::string("GenXicc error, vegasbin"));
    
    else if ( "valmatrix"==block )
      if  ( "cmfactor"   ==entry)GenXicc::valmatrix().cmfactor()=fl1; 
      else return Error (std::string("GenXicc error, valmatrix"));
    
    else if ( "mtypeofxi"==block )
      if  ( "mgenxi"   ==entry)GenXicc::mtypeofxi().mgenxi()=int1; 
      else return Error (std::string("GenXicc error, mtypeofxi"));
      
    else if ( "wbstate"==block )
      if  ( "nbound"   ==entry)GenXicc::wbstate().nbound()=int1; 
      else return Error (std::string("GenXicc error, wbstate"));
      
    else return Error (std::string("GenXicc error in parse parameters"));
  }
  
  return StatusCode::SUCCESS ;
}
