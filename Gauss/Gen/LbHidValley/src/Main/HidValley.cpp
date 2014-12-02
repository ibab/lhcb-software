// $Id: HidValley.cpp,v 1.2 2006-11-30 12:37:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
// New package: "Hidden Valley" production 
// 
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
#include <vector>
#include <iostream>
// ============================================================================
// Hidden Valley production
// ============================================================================
#include "LbHidValley/HidValley.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/program_options.hpp"
// ============================================================================
/** @file 
 *  Implementation of helper "main" program to drive "Hidden Valley"
 *  simulation, based on FORTRAN code by Matt Starssler
 *
 *  Matt Strassler <strasslr@noether.phys.washington.edu>:
 *
 * "This program simulates q qbar -> Z' -> v-quarks, followed by
 * v-showering and v-hadronization to v-pions.  The current program
 * makes various approximations and is not suitable for precision 
 * studies, but should be good at the factor-of-two level.
 *  In particular, the v-hadronization code uses Pythias QCD
 * hadronization code, scaled up in energy, and this is not a perfect 
 * simulation of the real model. Also, the Z' charges are not precisely 
 * those discussed in ref [1]; but in any case the model of [1] was 
 * just an example, and many other models exist.  Updates to this 
 * program will gradually correct these issues."
 *
 *  @author Vanya BELYAEV 
 *  @date 2006-09-25
 */
// ============================================================================
/// C++ interface to Fortran common blocl /MYDATA/
// ============================================================================
#ifdef WIN32
struct MYDATA_DEF { double zpmass, pimass, pizlif,piplif; } ;
extern "C" MYDATA_DEF MYDATA;
#define mydata MYDATA
#else 
extern struct mydata_def { double zpmass,pimass, pizlif,piplif; } mydata_ ;
#define mydata mydata_ 
#endif
// ============================================================================
//  avoid long names in parsing of options 
// ============================================================================
namespace po = boost::program_options ;
// ============================================================================

int main(int argc, char* argv[] )   
{
  
  const int         _nEvents  = 100 ;
  const std::string _oFile    = "HidVal.lhe" ;
  
  po::options_description opts(" Allowed Options for 'Hidded Valley' production");
  opts.add_options() 
    ("help,h",
     "Produce help message and exit") 
    ("nevents,n" , po::value<int>()->default_value(_nEvents),
     "Number of events to produce") 
    ("output,o" , po::value<std::string>()->default_value(_oFile) ,
     "Name of output file (in LHE format)") 
    ("seed,s"   , po::value<int>() ,
     "Random seed (MRPY(1))") 
    ("zpmass"   , po::value<double>() -> default_value(3000) , 
     "Z'* mass in GeV") 
    ("pimass"   , po::value<double>() -> default_value(35) , 
     "piv mass in GeV") 
    ("pizlif"   , po::value<double>() -> default_value(80) , 
     "piv0 lifetime ") 
    ("piplif"   , po::value<double>() -> default_value(-1) , 
     "piv+ lifetime " )
    ("ebeam"    , po::value<double>() -> default_value( 3.5e+3 ) , 
     "beam energy in GeV" )
    ;
  
  po::positional_options_description pos ;
  pos.add("output" ,1) ;
  pos.add("nevents" ,2) ;  
  
  po::variables_map values ;
  po::store( po::command_line_parser 
             (argc,argv).options( opts ).positional( pos ).run() ,values) ;
  po::notify( values );
  
  if ( values.count("help") ) 
  {
    std::cout << opts << std::endl ;
    exit(1) ;
  }
  
  if ( 1 > values["nevents"].as<int>() ) 
  {
    std::cout << "invalid number of events" << std::endl ;
    std::cout << opts << std::endl ;
    exit(1) ;
  }
  
  int iSeed = 0  ;
  if ( values.count("seed" ) ) 
  { iSeed = values["seed"].as<int>() ; }
  else
  {
    iSeed = std::rand () ;
    while ( 0 == iSeed || iSeed > 900000000 ) { iSeed = std::rand() ; }
    std::cout << " Random seed is set automatically to " << iSeed << std::endl ;
  }
  
  const int         nEvents = values["nevents"].as<int>() ;
  const std::string oFile   = values["output"].as<std::string>() ;
  
  const double zpmass = values["zpmass"].as<double>() ;
  const double pimass = values["pimass"].as<double>() ;
  const double pizlif = values["pizlif"].as<double>() ;
  const double piplif = values["piplif"].as<double>() ;
  const double ebeam  = values["ebeam" ].as<double>() ;
  
  if      ( !HidValley::setPar ( "ZPMASS" , zpmass ) ) 
  {
    std::cerr << "Could not set parameter ZPMASS" << std::endl ;
    exit(1) ;  
  }
  else if ( !HidValley::setPar ( "PIMASS" , pimass ) ) 
  {
    std::cerr << "Could not set parameter PIMASS" << std::endl ;
    exit(1) ;  
  }
  else if ( !HidValley::setPar ( "PIZLIF" , pizlif ) ) 
  {
    std::cerr << "Could not set parameter PIZLIF" << std::endl ;
    exit(1) ;  
  }
  else if ( !HidValley::setPar ( "PIPLIF" , piplif ) ) 
  {
    std::cerr << "Could not set parameter PIPLIF" << std::endl ;
    exit(1) ;  
  }
  else if ( !HidValley::setPar ( "EBEAM"  , ebeam ) ) 
  {
    std::cerr << "Could not set parameter EBEAM" << std::endl ;
    exit(1) ;  
  }
  
  // perform the actual work 
  return HidValley::HidVal ( iSeed , nEvents , oFile ) ;
}


// ============================================================================
/// The END 
// ============================================================================
