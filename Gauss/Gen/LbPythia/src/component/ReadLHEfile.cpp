// $Id: ReadLHEfile.cpp,v 1.1 2006-10-04 14:16:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Generators  
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
// LbPythia
// ============================================================================
#include "LbPythia/Pypars.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
// ============================================================================
/** @class ReadLHEfile ReadLHEfile.cpp
 *
 *  Simple production tool which reads parton level 
 *  LHE file (Les Houches Events). The LHE standard 
 *  in described in hep-ph/0609017 CERN-LCGAPP-2006-03
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-10-03
 */
class ReadLHEfile : public PythiaProduction 
{
  /// friend factory for instantiation
  friend class ToolFactory<ReadLHEfile> ;
public:
  /// tool initialization 
  virtual StatusCode initialize () ;
  /// tool finalization 
  virtual StatusCode finalize   () ;
public:
  /// Read the event from the file @see IProductionTool::generateEvent
  virtual StatusCode generateEvent 
  ( HepMC::GenEvent*    event     , 
    LHCb::GenCollision* collision ) ;
protected:
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  ReadLHEfile 
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : PythiaProduction ( type , name , parent ) 
    // the name of LHE input file  (no DEFAULT value!)
    , m_LHEfile ()
    // logical file unit  (F77)    ( DEFAULT is 25)
    , m_LUN ( 25 )
  {
    declareProperty ( "LHEfile" , m_LHEfile ) ;
    declareProperty ( "LUN"     , m_LUN     ) ;
    ///
    PythiaProduction::m_frame  = "USER" ;
    PythiaProduction::m_beam   = " " ;
    PythiaProduction::m_target = " " ;
    // see LHEREADPROCESS
    PythiaProduction::m_userProcess    = 3 ; ///< see LHEREADPROCESS
    PythiaProduction::m_variableEnergy = false ;
    //
    // switch off the processed, switched on in base class 
    addPygive ( "MSUB(11)=0;MSUB(12)=0;MSUB(13)=0") ;
    addPygive ( "MSUB(28)=0;MSUB(53)=0;MSUB(68)=0") ;
    addPygive ( "MSUB(86)=0;MSUB(87)=0;MSUB(88)=0") ;
    addPygive ( "MSUB(89)=0;MSUB(90)=");
    addPygive ( "MSUB(91)=0;MSUB(92)=0;MSUB(93)=0") ;
    addPygive ( "MSUB(94)=0;MSUB(95)=0;MSUB(96)=0") ;
    addPygive ( "MSUB(106)=0" ) ;
    //  
  } ;                      
  virtual ~ReadLHEfile() {}
private:
  // the default constructor is disabled 
  ReadLHEfile() ;
  // the copy constructor is disabled 
  ReadLHEfile ( const ReadLHEfile& ) ;
  // assignement operator is disabled 
  ReadLHEfile& operator=( const ReadLHEfile& ) ;
private:
  // the name of LHE input file 
  std::string m_LHEfile ; ///< the name of LHE input file 
  // logical file unit  (F77)
  int         m_LUN     ; ///< logical file unit  (F77)
} ;
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( ReadLHEfile );
// ============================================================================
/// tool initialization 
// ============================================================================
StatusCode ReadLHEfile::initialize () 
{
  if (      msgLevel( MSG::VERBOSE ) ) 
  {
    //m_commandVector.push_back( "pyinit pylisti 12" ) ;  
    // m_commandVector.push_back( "pyinit pyliste 3"  ) ;
    //m_commandVector.push_back( "pyinit pyliste 1"  ) ;
  }
  else if ( msgLevel ( MSG::DEBUG ) ) 
  {
    //m_commandVector.push_back( "pyinit pyliste 2"  ) ;
    //m_commandVector.push_back( "pyinit pystatf 3"  ) ;    
  }
  

  // open input LHE file 
  StatusCode sc = F77Utils::openOld ( m_LUN , m_LHEfile ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Could not open the file '" + m_LHEfile + "'" ) ; }
  //
  Pythia::pypars().mstp ( 161 ) = m_LUN ;                                // ATTENTION!
  Pythia::pypars().mstp ( 162 ) = m_LUN ;                                // ATTENTION!
  //
  sc = PythiaProduction::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// tool finalization 
// ============================================================================
StatusCode ReadLHEfile::finalize   () 
{
  // close input LHE file 
  StatusCode sc = F77Utils::close ( m_LUN ) ;
  if ( sc.isFailure() ) 
  { Error ( "Error in opening '" + m_LHEfile + "'" , sc ) ; } // NO RETURN !
  //
  return PythiaProduction::finalize() ;
} ;
// ============================================================================
/// Read the event from the file @see IProductionTool::generateEvent
// ============================================================================
StatusCode ReadLHEfile::generateEvent 
( HepMC::GenEvent*    event     , 
  LHCb::GenCollision* collision ) 
{
  StatusCode sc = PythiaProduction::generateEvent ( event , collision ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from PythiaProduction::generateEvent" , sc ) ; }
  if ( 0 != Pythia::pypars().msti(51) ) 
  {
    Pythia::PyGive("MSTI(51)=");
    return Warning ("MSTI(51)!=0 : No more event in the input file?");  
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================







// ============================================================================
// The END 
// ============================================================================
