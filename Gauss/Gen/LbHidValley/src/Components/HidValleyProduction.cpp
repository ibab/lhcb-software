// $Id: HidValleyProduction.cpp,v 1.3 2008-06-24 10:49:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL
// ============================================================================
#include <set>
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h" 
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/HEPEVT_Wrapper.h"
// ============================================================================
// Generators 
// ============================================================================
#include "Generators/IBeamTool.h"
// ============================================================================
// LbPythia 
// ============================================================================
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
// ============================================================================
// local 
// ============================================================================
#include "LbHidValley/HidValley.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @class HidValleyProduction
 *  the first attempt to perform 
 *  the Hidden Valley production embedded into Gauss.
 *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
 *  @date 2006-10-20
 */
class HidValleyProduction : public PythiaProduction 
{
  // ==========================================================================
  // friend factory for instantiation 
  friend class ToolFactory<HidValleyProduction> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// initialize the production tool 
  virtual StatusCode initialize          ( ) ;
  /// initiailze the generator 
  /// virtual StatusCode initializeGenerator ( ) ;
  /// generate the event
  virtual StatusCode generateEvent
  ( HepMC::GenEvent*    theEvent     , 
    LHCb::GenCollision* theCollision ) ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent parent component 
   */
  HidValleyProduction 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) 
    : PythiaProduction ( type , name , parent ) 
    , m_beamene        ( 3.5 * Gaudi::Units::TeV ) 
  {
    PythiaProduction::m_defaultSettings.clear() ;
    //
    // postpone the PyInit initialization 
    Assert ( setProperty ( "ValidateHEPEVT" , true ).isSuccess () , 
             "Unable to set property 'ValidateHEPEVT'" ) ;
    declareProperty 
      ( "BeamEnergy" , 
        m_beamene    , 
        "Beam energy for Hidden-Valley production" ) ;
    //
    PythiaProduction::m_frame          = "CMS" ;
    PythiaProduction::m_beam           = "p+"  ;
    PythiaProduction::m_target         = "p+"  ;
    PythiaProduction::m_win            = 7.0 * Gaudi::Units::TeV ;
    PythiaProduction::m_userProcess    = 4 ;
      
  } 
  /// virtual and protected desctructor 
  virtual ~HidValleyProduction() {}
  // ==========================================================================  
private:
  // ==========================================================================
  /// default constructor is disabled 
  HidValleyProduction () ; // default constructor is disabled 
  /// copy constructor is disabled 
  HidValleyProduction ( const HidValleyProduction& ) ; // no copy
  /// assignement operator is disabled 
  HidValleyProduction& operator=( const HidValleyProduction& );
  // ==========================================================================
private:
  // ==========================================================================
  double m_beamene ;
  // ==========================================================================
} ;
// ============================================================================
// initilize the production tool 
// ============================================================================
StatusCode HidValleyProduction::initialize() 
{
  // return GaudiTool::initialize() ;
  StatusCode sc = PythiaProduction::initialize () ;
  // StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  // Set size of common blocks in HEPEVT: note these correspond to stdhep
  BOOST_STATIC_ASSERT( 4 == sizeof (    int ) ) ;
  BOOST_STATIC_ASSERT( 8 == sizeof ( double ) ) ;
  //
  HepMC::HEPEVT_Wrapper::set_sizeof_int         (     4 ) ;
  HepMC::HEPEVT_Wrapper::set_sizeof_real        (     8 ) ;
  HepMC::HEPEVT_Wrapper::set_max_number_entries ( 10000 ) ; 
  
  // use PYGIVE commands (if any) (as THE LAST action)
  for ( CommandVector::const_iterator item = 
          m_pygive.begin() ; m_pygive.end() != item ; ++item ) 
  {
    // use FORTRAN PYGIVE routine
    debug  () << " CALL PYGIVE(' " << (*item) << "')" << endreq ;
    always () << " CALL PYGIVE(' " << (*item) << "')" << endreq ;
    const int mstu_13 = Pythia::pydat1().mstu(13) ;
    Pythia::pydat1().mstu(13) =1   ;
    Pythia::PyGive( *item ) ;
    Pythia::pydat1().mstu(13) = mstu_13 ;
  }  
  // Reset forced fragmentation flag
  Pythia::pydat1().mstu( 150 ) = 0 ;
  
  Pythia::SetUserProcess ( 4 ) ;
  
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// generate the event
// ============================================================================
StatusCode HidValleyProduction::generateEvent
( HepMC::GenEvent*    theEvent     , 
  LHCb::GenCollision* theCollision ) 
{
  // (re)set parameters 
  const double zpmass = Pythia::pydat2().pmas (32,1) /* 3000 */ ;
  if ( !HidValley::setPar ( "ZPMASS" , zpmass  ) ) 
  { return Error ( " Could not set parameter 'ZPMASS' " ) ; }
  //
  const double pimass = Pythia::pydat2().pmas (35,1) /* 35 */   ;
  if ( !HidValley::setPar ( "PIMASS" , pimass  ) ) 
  { return Error ( " Could not set parameter 'PIMASS' " ) ; }
  //
  const double pizlif = Pythia::pydat2().pmas (35,4) /* 80 */   ;
  if ( !HidValley::setPar ( "PIZLIF" , pizlif ) ) 
  { return Error ( " Could not set parameter 'PIZLIF' " ) ; }
  //
  const double piplif = Pythia::pydat2().pmas (36,4) /* 80 */   ;
  if ( !HidValley::setPar ( "PIPLIF" , piplif    ) ) 
  { return Error ( " Could not set parameter 'PIPLIF' " ) ; }
  //
  if ( !HidValley::setPar ( "EBEAM"  , m_beamene / Gaudi::Units::GeV ) ) 
  { return Error ( " Could not set parameter 'EBEAM'  " ) ; }
  //
  Pythia::SetUserProcess ( 4 ) ;
  // Generate Event
  const bool OK = HidValley::hvEvent () ;
  if ( !OK ) { return Error ( "Error from HidValley::hvEvent" ) ; }
  
  StatusCode sc = toHepMC ( theEvent , theCollision ) ;
  
  if ( msgLevel ( MSG::VERBOSE ) ) 
  {
    MsgStream& log = verbose() ;
    if ( log.isActive() ) { theEvent->print ( log.stream() ) ; }
  }
  return sc ;
} 
// ============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HidValleyProduction )
// ============================================================================

// ============================================================================
/// The END 
// ============================================================================

