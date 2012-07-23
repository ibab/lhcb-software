// $Id:
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <string>
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/PythiaWrapper.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenEvent.h"
// ============================================================================
// Gen/Generators
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/program_options.hpp"
// ============================================================================
/** @file 
 *  Simple program to read ALPGEN "unweighted" files 
 *  and optimal output into HepMC ascii format.
 *  Essentially it is a bit modified version of pyuser.cc from 
 *  alpgen distribution
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-06-28
 */
// ============================================================================ 
//  avoid long names in parsing of options 
// ============================================================================
namespace po = boost::program_options ;
// ============================================================================
int main( int argc , char* argv[] ) 
{ 
  // number of events to process 
  int         _nEvents = 1000          ; ///< number of events to process
  // output HepMC-Ascii file
  std::string _oFile   = ""            ; ///< output HepMC-Ascii file
  // log-file
  std::string _lFile   = "AlpGen.log"  ; ///< log-file 
  
  po::options_description opts(" Allowed Options for AlpGen executable: ");
  opts.add_options() 
    ( "help,h",
      "Produce help message and exit") 
    ( "nevents,n"     , po::value<int>()->default_value(_nEvents),
      "Number of events to process"           )  
    ( "input,i"       , po::value<std::string>()  ,
      "Name of input file with (unweighted) Alpgen's partonic events") 
    ( "parameters,p"  , po::value<std::string>()  ,
      "Name of input file with parameters Alpgen's parameters")    
    ( "log,l"         , po::value<std::string>()->default_value(_lFile) ,
      "Name of output log-file"               )
    ( "output,o"      , po::value<std::string>()->default_value(_oFile) ,
      "Name of output file (in HepMC format)" ) 
    ( "seed,s"        , po::value<int>() ,
      "Random seed (MRPY(1))") 
    ;
  
  po::variables_map values ;
  po::store( po::command_line_parser 
             (argc,argv).options( opts ).run() ,values) ;
  po::notify( values );
  
  // help ??
  if ( values.count("help") ) 
  {
    std::cout << opts << std::endl ;
    exit(0) ;
  }
  
  if ( 1 > values["nevents"].as<int>() ) 
  {
    std::cout << "invalid number of events" << std::endl ;
    std::cout << opts << std::endl ;
    exit(1) ;
  }
  
  int iSeed = 0  ;
  if ( values.count("seed" ) ) { iSeed = values["seed"].as<int>() ; }
  else
  {
    iSeed = std::rand () ;
    while ( 0>=  iSeed ) { iSeed = std::rand() ; }
    std::cout << " Random seed is set automatically to " << iSeed << std::endl ;
  }

  // nbumber of events 
  
  const int         nEvents = values["nevents"].as<int>() ;
  
  // output file 

  const std::string oFile   = values["output"].as<std::string>() ;

  // input file 

  if ( !values.count("input") ) 
  {
    std::cerr << "Input file with partonic data is not specified!" 
              << std::endl << opts << std::endl ;
    exit(1); 
  }
  const std::string input   = values["input"].as<std::string> () ;
  
  // input parameters:

  if ( !values.count("parameters") ) 
  {
    std::cerr << "Input file with parameters is not specified!" 
              << std::endl << opts << std::endl ;
    exit(1); 
  }
  const std::string parameters = values["parameters"].as<std::string> () ;

  // name of log-file 
  const std::string logfile    = values["log"].as<std::string>  () ;
  
  //........................................HEPEVT
  // Pythia >6.1 uses HEPEVT with 4000 entries and 8-byte floating point
  //  numbers. We need to explicitly pass this information to the 
  //  HEPEVT_Wrapper.
  
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
    
  //........................................PYTHIA INITIALIZATIONS
  // (Some platforms may require the initialization of pythia PYDATA block 
  //  data as external - if you get pythia initialization errors try 
  //  commenting in/out the below call to initpydata() )
  // initpydata();
  //..............................................................
  
  // set random number seed (mandatory!)
  pydatr.mrpy[0] = iSeed ;
  
  { // open parameters file  
    int LUN = F77Utils::getUnit() ;
    StatusCode sc = F77Utils::openOld ( LUN , parameters , true ) ;
    if ( sc.isFailure() ) { exit(1) ; }
    pypars.mstp[161-1] = LUN ;
  }
  { // open input file
    int LUN = F77Utils::getUnit() ;
    StatusCode sc = F77Utils::openOld ( LUN , input , true ) ;
    if ( sc.isFailure() ) { exit(1) ; }
    pypars.mstp[162-1] = LUN ;
  }
  { // open parameters file  
    int LUN = F77Utils::getUnit() ;
    StatusCode sc = F77Utils::open ( LUN , logfile , true ) ;
    if ( sc.isFailure() ) { exit(1) ; }
    pypars.mstp[163-1] = LUN ;
  }
  
  // Call pythia initialization
  call_pyinit("USER", " ", " ", 0.);
  
  //........................................HepMC INITIALIZATIONS

  // Instantiate an IO strategy for reading from HEPEVT.
  HepMC::IO_HEPEVT hepevtio;
  hepevtio.set_print_inconsistency_errors( false ) ;
  
  // Instantiate an IO strategy to write the data to file
  HepMC::IO_GenEvent* ascii_io = 0 ;
  if ( !oFile.empty() ) 
  { ascii_io = new HepMC::IO_GenEvent ( oFile.c_str() ,std::ios::out) ; }
  

  
  
  //.......EVENT LOOP:
  int nold = 0 ;
  
  for ( int i = 0; i < nEvents ; ++i) 
  {
    // process one event with pythia
    call_pyevnt(); ///< process one event with pythia
    if ( pyint5.ngen[2][0] == nold ) { break; }
    nold = pyint5.ngen[2][0];
    
    // convert PYJETS into HEPEVT
    call_pyhepc(1); ///< convert PYJETS into HEPEVT
    
    
    HepMC::HEPEVT_Wrapper::check_hepevt_consistency() ;
    
    // convert HEPEVT intp HepMC
    HepMC::GenEvent* evt = hepevtio.read_next_event(); 

    // add some information to the event
    evt->set_event_number(i);
    evt->set_signal_process_id(20);
    
    // write the event out to the ascii file
    if ( 0 != ascii_io ) { *ascii_io << evt ; }
    
    // Analysing the event:
    // we also need to delete the created event from memory
    delete evt;
  } // end of event loop
  
  // write out some information from Pythia to the screen
  call_pystat(1);    
  
  if ( 0 != ascii_io ) { delete ascii_io ; ascii_io = 0 ; }
  if ( 0 < pypars.mstp[161-1] ) 
  {
    F77Utils::close( pypars.mstp[161-1] , true ) ;
    pypars.mstp[161-1] = 0 ;
  }
  if ( 0 < pypars.mstp[162-1] ) 
  {
    F77Utils::close( pypars.mstp[162-1] , true ) ;
    pypars.mstp[162-1] = 0 ;
  } 
  if ( 0 < pypars.mstp[163-1] ) 
  {
    F77Utils::close( pypars.mstp[163-1] , true ) ;
    pypars.mstp[163-1] = 0 ;
  }
  

  return 0;
}
// ============================================================================
// The END 
// ============================================================================
