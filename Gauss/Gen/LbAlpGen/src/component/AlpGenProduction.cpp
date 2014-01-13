// Include files 
//
// local
#include "AlpGenProduction.h"

//DELETE ME
// STL 
#include <fstream>

// Boost
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/SystemOfUnits.h"
//for getenv():
#include "GaudiKernel/System.h"

// from Event
#include "Event/GenCollision.h"

// local
#include "Generators/StringParse.h"
#include "Generators/IBeamTool.h"
#include "Generators/F77Utils.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/Hepeup.h"
#include "LbAlpGen/AlpGenFunctions.h"
#include "LbAlpGen/Ahio.h"
#include "LbAlpGen/Aiounits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlpGenProduction
//
// 
//-----------------------------------------------------------------------------

//new job option cuts communicated to fortran:
extern "C" {
  struct {
    float letamin,  betamin, letamin1, betamin1, betamin2;
    
  } alpgenetacut_;    
}
extern "C" {
  struct {
    float lptmin1, bptmin1, bptmin2;
    
  } alpgenptcut_;    
}

//Don't declare it a tool. The derived classes should be used instead 
//DECLARE_TOOL_FACTORY( AlpGenProduction )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlpGenProduction::AlpGenProduction( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : PythiaProduction ( type, name , parent ) ,
    m_nevents  ( 0  ) , 
    m_randSvc  ( 0  ) ,
    m_engine   ( 0  ) 
  
{
  
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "FileLabel" , m_fileLabel = "Zbb" );
  declareProperty( "nevxiter", m_nevxiter = 100000, 
                   "number of events per iteration to generate alpgen's phase space grid in alpgen mode 1. Do tests!");
  declareProperty( "niter", m_niter = 2, 
                   "number of iterations to generate alpgen's phase space grid in alpgen mode 1.");
  declareProperty( "nwgtev", m_nwgtev = 1000000, 
                   "number of weighted events to generate by alpgen in mode 1.  Do tests!");
  declareProperty( "ndns" , m_ndns = 9 , "parton density set (cf alpgen for codes) 9: CTEQ6L1 5: CTEQ5L");
  declareProperty( "iqopt" , m_iqopt = 1 , "scale option (process dependent)");
  declareProperty( "qfac" , m_qfac =  1.00, " Q scale rescaling factor");
  declareProperty( "ickkw" , m_ickkw = 0 , 
                   "0 : no matching of Matrix Elements and Parton Shower - 1: matching (mlm scheme)");
  declareProperty( "ktfac" , m_ktfac = 1.00 , " scale factor for ckkw alphas scale");
  declareProperty( "njets" , m_njets = 0, "number of light jets");
  declareProperty( "ihvy" , m_ihvy = 5 , "heavy flavour type for procs like WQQ, ZQQ, 2Q, etc(4=c, 5=b, 6=t)");
  declareProperty( "ihvy2" , m_ihvy2 = 5 , "2nd heavy flavour type for procs like 4Q");
  declareProperty( "mc" , m_mc =  1.50, "charm mass");
  declareProperty( "mb" , m_mb = 4.70 , "bottom mass");
  //top mass from Frederic Deliot's LHC-Tevatron combination at ICHEP 2012 (173.1+-0.9 GeV/c2):");
  declareProperty( "mt" , m_mt = 173.1, "top mass" ); 
  declareProperty( "ptjmin" , m_ptjmin = 1.0 , "minimum pt for light jets");
  declareProperty( "ptbmin" , m_ptbmin = 1.0 , " ptmin for bottom quarks (in procs with explicit b)");
  declareProperty( "ptcmin" , m_ptcmin = 1.0 , " ptmin for charm quarks (in procs with explicit c)");
  declareProperty( "ptlmin" , m_ptlmin = 1.0 , "minimum pt for charged leptons");
  declareProperty( "etajmax" , m_etajmax = 100.0 , "max|eta| for light jets");
  declareProperty( "etabmax" , m_etabmax = 100.0 , " max|eta| for b quarks (in procs with explicit b)");
  declareProperty( "etacmax" , m_etacmax = 100.0, "max|eta| for c quarks (in procs with explicit c)");
  declareProperty( "etalmax" , m_etalmax = 100.0 , "max abs(eta) for charged leptons");
  declareProperty( "etalmin" , m_etalmin = -100.0 , "min eta for charged leptons");
  declareProperty( "etabmin" , m_etabmin = -100.0 , "min eta for b quarks  ");
  declareProperty( "eta1lmin" , m_eta1lmin = -100.0 , "min eta for at least 1 charged lepton");
  declareProperty( "eta1bmin" , m_eta1bmin = -100.0 , "min eta for at least 1 b quark  ");
  declareProperty( "eta2bmin" , m_eta2bmin = -100.0 , "min eta for at least 2 b quarks  ");
  declareProperty( "pt1lmin" , m_pt1lmin = -100.0 , "min pt for at least 1 charged lepton");
  declareProperty( "pt1bmin" , m_pt1bmin = -100.0 , "min pt for at least 1 b quark  ");
  declareProperty( "pt2bmin" , m_pt2bmin = -100.0 , "min pt for at least 2 b quarks  ");
  declareProperty( "drjmin" , m_drjmin = 0.10 , " min deltaR(j-j), deltaR(Q-j) [j=light jet, Q=c/b]");
  declareProperty( "drbmin" , m_drbmin = 0.10 , "min deltaR(b-b) (procs with explicit b)");
  declareProperty( "drcmin" , m_drcmin = 0.10 , "min deltaR(c-c) (procs with explicit c)");
  declareProperty( "drlmin" , m_drlmin = 0.0 , "min deltaR between charged lepton and light jets");
  //declareProperty( "ilep" , m_ilep =  , "Z*/gamma fin state: 0=lept (1 family) 1=nu (3 fam)");
  declareProperty( "mllmin" , m_mllmin = 5.0 , "min dilepton inv mass");
  declareProperty( "mllmax" , m_mllmax = 1000.0 , "max dilepton inv mass");
  declareProperty( "itopprc" , m_itopprc = 1 , "Selection of single-top process \
   - 1:t+q 2:t+b 3:t+W 4:t+bbar+W" );
  declareProperty( "iwdecmode" , m_iwdecmode = 6 , "W decay modes \
   - 1: e nu 2: mu nu 3: tau nu 4: e/mu/tau nu 5: q q''bar' 6: fully inclusive");
  declareProperty( "itdecmode" , m_itdecmode = 6 , 
                   "top (or t-tbar) decay modes - 1: e nu b 2: mu nu b 3: tau nu b 4:l nu b  : 5:q q b 6:fully inclusive");
  declareProperty( "izdecmode" , m_izdecmode = 4 , "Z decay modes, in imode=2 - 1:electron 2: muon 3:tau 4:e/mu/tau");
  declareProperty( "xlclu" , m_xlclu = -1.0 , "lambda value for ckkw alpha (match shower alpha) - default -1");
  declareProperty( "lpclu" , m_lpclu =  -1.0, "loop order for ckkw alpha (match shower alpha) - default -1");
  declareProperty( "cluopt" , m_cluopt =  1, "kt scale option. 1:kt propto pt, 2:kt propto mt");

}

//=============================================================================
// Destructor 
//=============================================================================
AlpGenProduction::~AlpGenProduction( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode AlpGenProduction::initialize( ) {

  // Do some cleaning of files
  boost::filesystem::remove( boost::filesystem::path( "cnfg.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "par.list" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid1" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid2" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".mon" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".pyt-log" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".stat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".unw" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".wgt" ) ) ;

  Makelink2pdftable(m_ndns);

  //
  m_nevents = 0 ;
  // generate events with default seed for Pythia initialization
  generateWeightedEvents( ) ;
  generateUnweightedEvents() ;

  // For Jet Parton Matching MSTP(143) = 1 , (1: Pythia subroutine UPVETO called - 0: not called)
  Pythia::pypars().mstp( 143 ) =  m_ickkw ;

 //Change the parameter to 5 for AlpGen
  m_userProcess = 5 ;
  // User process
  m_frame = "USER" ;
  m_beam = "p+";
  m_target = "p+" ;

  // SET Filename
  //boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  boost::filesystem::path alpfile( boost::filesystem::current_path().string() + "/alpgenlabelname");
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << m_fileLabel << std::endl ;    // label of the output files
  g.close() ;
  
  // moved to alpho.f :
  //  F77Utils::open( 79 , alpfile.string() , false ) ;  

  //Initialization of Pythia done here
  StatusCode sc = PythiaProduction::initialize( ) ;

  //removed in 11/2013 because it causes a job crash with gauss v45rx:
  // boost::filesystem::remove( alpfile ) ;

  if ( sc.isFailure() ) return sc ;

  return sc ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode AlpGenProduction::finalize( ) {
  AlpGenFunctions::AlSfin() ;
  Ahio ahio ;
  F77Utils::close( ahio.nunit() ) ;
  F77Utils::close( ahio.nunitout() ) ;

  Aiounits aiounits ;
  F77Utils::close( aiounits.niosta() ) ;
  F77Utils::close( aiounits.niopar() ) ;
  F77Utils::close( aiounits.niowgt() ) ;
  F77Utils::close( aiounits.niounw() ) ;
  
  // Do some cleaning of files
    boost::filesystem::remove( boost::filesystem::path( "cnfg.dat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "par.list" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid1" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".grid2" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".mon" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".pyt-log" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".stat" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".unw" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.par" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + "_unw.top" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( m_fileLabel + ".wgt" ) ) ;
  boost::filesystem::remove( boost::filesystem::path( "alpgenlabelname" ) ) ;
  
  return PythiaProduction::finalize() ;
}

//=============================================================================
//   Function called to generate one event with Pythia --> AlpGen
//=============================================================================
StatusCode AlpGenProduction::generateEvent( HepMC::GenEvent * theEvent , 
                                            LHCb::GenCollision * theCollision)
{
  m_nevents++ ;
  if ( 1 == m_nevents ) {
    // generate events with correct random seed for the first event
    Ahio ahio ;
    F77Utils::close( ahio.nunit() ) ;
    F77Utils::close( 99 ) ;
    Aiounits aiounits ;
    F77Utils::close( aiounits.niosta() ) ;
    F77Utils::close( aiounits.niopar() ) ;
    F77Utils::close( aiounits.niowgt() ) ;
    F77Utils::close( aiounits.niounw() ) ;

    generateWeightedEvents( ) ;
    generateUnweightedEvents( ) ;
    
    F77Utils::open( ahio.nunit() , m_fileLabel + ".unw" , false ) ;    
  }

  StatusCode sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
  if ( sc.isFailure() ) return sc ;

  Hepeup hepeup ;
  
  if ( 0 == hepeup.nup() ) {
    // end of ALPGEN file reached, generate a new file
    debug() << "End of ALPGEN file reached. Generate a new one" << endmsg ;
    Ahio ahio ;
    F77Utils::close( ahio.nunit() ) ;
    F77Utils::close( 99 ) ;
    Aiounits aiounits ;
    F77Utils::close( aiounits.niosta() ) ;
    F77Utils::close( aiounits.niopar() ) ;
    F77Utils::close( aiounits.niowgt() ) ;
    F77Utils::close( aiounits.niounw() ) ;

    generateWeightedEvents( ) ;
    generateUnweightedEvents( ) ;
    
    F77Utils::open( ahio.nunit() , m_fileLabel + ".unw" , false ) ;

    sc = PythiaProduction::generateEvent(theEvent, theCollision) ;
    if ( sc.isFailure() ) return sc ;
  }

  return StatusCode::SUCCESS ;
}

//============================================================================
// Generate weighted events with ALPGEN
//============================================================================
StatusCode AlpGenProduction::generateWeightedEvents( ) {

  // create temporary file containing parameters to input to ALPGEN
  // boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  boost::filesystem::path alpfile( boost::filesystem::current_path().string() + "/alpgeninput1file");
  
  if ( boost::filesystem::exists( alpfile ) )
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  
  g << 1 << std::endl // Generation mode : 1 = generate weighted events
    << m_fileLabel << std::endl          // label of the output files
    << 0 << std::endl              // new grid
    //    << (m_nevents!=0 ? m_nevxiter : 10000) << " " << m_niter << std::endl 
// nevents per iteration and n iterations
    //CHANGE BACK!
    //<< (m_nevents!=0 ? m_nwgtev : 100000) << std::endl;            // nevents to generate
    << (m_nevents!=0 ? m_nevxiter : 10000) << " " << m_niter << std::endl
 // nevents per iteration and n iterations                                                   
    << (m_nevents!=0 ? m_nwgtev : 1000) << std::endl;            // nevents to generate

  //set up the new job options for fortran (see file */*usr.F , '*'=process ) :
  alpgenetacut_.letamin = m_etalmin ;
  alpgenetacut_.betamin = m_etabmin ;
  alpgenetacut_.letamin1 = m_eta1lmin ;
  alpgenetacut_.betamin1 = m_eta1bmin ;
  alpgenetacut_.betamin2 = m_eta2bmin ;
  alpgenptcut_.lptmin1 = m_pt1lmin ;
  alpgenptcut_.bptmin1 = m_pt1bmin ;
  alpgenptcut_.bptmin2 = m_pt2bmin ;  

  // Obtain beam tool
  if ( 0 == m_beamTool ) 
    m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;
  
  Gaudi::XYZVector pBeam1 , pBeam2 ;
  m_beamTool -> getMeanBeams( pBeam1 , pBeam2 ) ;

  g << "ebeam " << pBeam1.R() / Gaudi::Units::GeV << std::endl  // beam energy
    << "ih2 " << 1  << std::endl //pp collision
    << "ndns " << m_ndns << std::endl
    << "iqopt " << m_iqopt << std::endl
    << "qfac " << m_qfac << std::endl
    << "ickkw " << m_ickkw << std::endl
    << "ktfac " << m_ktfac << std::endl
    << "ilep " << 0 << std::endl 
    << "njets " << m_njets << std::endl
    << "ihvy " << m_ihvy << std::endl
    << "ihvy2 " << m_ihvy2 << std::endl
    << "mc " << m_mc << std::endl
    << "mb " << m_mb << std::endl
    << "mt " << m_mt << std::endl
    << "ptjmin " << m_ptjmin << std::endl
    << "ptbmin " << m_ptbmin << std::endl
    << "ptcmin " << m_ptcmin << std::endl
    << "ptlmin " << m_ptlmin << std::endl
    << "etajmax " << m_etajmax << std::endl
    << "etabmax " << m_etabmax << std::endl
    << "etacmax " << m_etacmax << std::endl
    << "etalmax " << m_etalmax << std::endl
    << "etalmin " << m_etalmin << std::endl
    << "drjmin " << m_drjmin << std::endl
    << "drbmin " << m_drbmin << std::endl
    << "drcmin " << m_drcmin << std::endl
    << "drlmin " << m_drlmin << std::endl
    << "mllmin " << m_mllmin << std::endl
    << "mllmax " << m_mllmax << std::endl
    << "itopprc " << m_itopprc << std::endl
    << "iwdecmode " << m_iwdecmode << std::endl
    << "itdecmode " << m_itdecmode << std::endl
    << "izdecmode " << m_izdecmode << std::endl
    << "xlclu " << m_xlclu << std::endl
    << "lpclu " << m_lpclu << std::endl
    << "cluopt " << m_cluopt << std::endl ;
  
  if ( 0 == m_nevents ) {
    // use default seeds
    g << "iseed1 " << 12345 << std::endl 
      << "iseed2 " << 67890 << std::endl
      << "iseed3 " << 12345 << std::endl 
      << "iseed4 " << 67890 << std::endl ;
  } 
  else {
    // use Gauss seeds
    if( 0 == m_randSvc ) m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
    if( 0 == m_engine  )
    {
      m_engine  = m_randSvc->engine();
      if( 0 == m_engine ) {
        return Error( "Random number engine not found!" );
      }
    }
    std::vector< long > theSeeds ;
    m_engine -> seeds( theSeeds ) ;
    int iseed1 = (theSeeds[ 0 ]*10+theSeeds[ 2 ]) - 100000*((theSeeds[ 0 ]*10+theSeeds[ 2 ])/100000);
    int iseed2 = theSeeds[ 1 ] - 100000*(theSeeds[ 1 ]/100000);
    g << "iseed1 " << iseed1 << std::endl
      << "iseed2 " << iseed2 << std::endl
      << "iseed3 " << iseed1 << std::endl
      << "iseed4 " << iseed2 << std::endl ;
    
    release( m_randSvc ) ;
    m_randSvc = 0 ;
    m_engine = 0 ;
  }  
  
  g << "print " << 2 << std::endl ;
  g.close() ;

  AlpGenFunctions::AlSprc() ;

  F77Utils::open( 5 , alpfile.string() , false ) ;  
  AlpGenFunctions::AlSdef() ;
  F77Utils::close( 5 ) ;

  boost::filesystem::remove( alpfile ) ;

  AlpGenFunctions::AlHset() ;
  AlpGenFunctions::AlInit() ;
  AlpGenFunctions::AlSbkk() ;
  AlpGenFunctions::AlShis() ;
  AlpGenFunctions::AlSgrd() ;
  AlpGenFunctions::AlIgrd() ;
  AlpGenFunctions::AlEgen() ;
  AlpGenFunctions::AlFhis() ;
  AlpGenFunctions::AlFbkk() ;
  
  //added: 18 nov 2013:
  F77Utils::close( 11) ;
  F77Utils::close( 10 ) ;
  F77Utils::close( 12 ) ;

  return StatusCode::SUCCESS ;
}

//============================================================================
// Generate unweighted events with ALPGEN
//============================================================================
void AlpGenProduction::generateUnweightedEvents( ) {

  // create temporary file containing parameters to input to ALPGEN                                                                               
  boost::filesystem::path alpfile( boost::filesystem::current_path().string() + "/alpgin2file");
  
  if ( boost::filesystem::exists( alpfile ) )
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << 2 << std::endl // Generation mode : 2 = generate unweighted events
    << m_fileLabel << std::endl  ;        // label of the output files
  g.close() ;

  AlpGenFunctions::AlSprc() ;

  F77Utils::open( 5 , alpfile.string() , false ) ;  
  AlpGenFunctions::AlSdef() ;
  F77Utils::close( 5 ) ;

  boost::filesystem::remove( alpfile ) ;

  AlpGenFunctions::AlHset() ;
  AlpGenFunctions::AlInit() ;
  AlpGenFunctions::AlSbkk() ;
  AlpGenFunctions::AlShis() ;
  AlpGenFunctions::AlSgrd() ;
  AlpGenFunctions::AlIgrd() ;
  AlpGenFunctions::AlEgen() ;
  AlpGenFunctions::AlFhis() ;
  AlpGenFunctions::AlFbkk() ;

  F77Utils::close( 11 ) ;
  F77Utils::close( 10 ) ;
  F77Utils::close( 12 ) ;
}


//============================================================================
// Create a symbolic link to the pdf table used in this job
//============================================================================
void AlpGenProduction::Makelink2pdftable(int ndns) 
{
  std::string pdftbldir;
  std::string pdftbl;
  
  switch (ndns) {
  case 1 :
    pdftbldir = "/ctq45/";
    pdftbl = "cteq4m.tbl";
    break;
  case 2 :
    pdftbldir = "/ctq45/";
    pdftbl = "cteq4l.tbl";
    break;
  case 3 :
    pdftbldir = "/ctq45/";
    pdftbl = "cteq4hj.tbl";
    break;
  case 4 :
    return;
    break;
  case 5 :
    return;
    break;
  case 6 :
    pdftbldir = "/ctq45/";
    pdftbl = "cteq5hj.tbl";
    break;
  case 7 :
    pdftbldir = "/ctq61/";
    pdftbl = "cteq6m.tbl";
    break;
  case 8 :
    pdftbldir = "/ctq61/";
    pdftbl = "cteq6l.tbl";
    break;
  case 9 :
    pdftbldir = "/ctq61/";
    pdftbl = "cteq6l1.tbl";
    break;
  }
  if (ndns>9 && ndns<20) {
    pdftbldir = "/ctq61sys/";
    pdftbl = "ctq61.0";
    pdftbl += boost::lexical_cast<std::string>(ndns-10);
    pdftbl += ".tbl";
  }
  else if (ndns>19 && ndns<51) {
    pdftbldir = "/ctq61sys/";
    pdftbl = "ctq61.";
    pdftbl +=boost::lexical_cast<std::string>(ndns-10);
    pdftbl += ".tbl";
  }
  else if (ndns<1 || ndns>50) 
    error() <<  "The ndns parameter should be comprised between 1 and 50. The job will crash." 
            << endmsg ;
  
  
  //Make a symbolic link to the pdf table: 
  boost::filesystem::path symlink (pdftbl);
  boost::filesystem::remove(symlink) ;
  boost::filesystem::create_symlink(boost::filesystem::path(System::getEnv("ALPGENPDFPATH")
                                                            +pdftbldir+pdftbl), symlink);
}
