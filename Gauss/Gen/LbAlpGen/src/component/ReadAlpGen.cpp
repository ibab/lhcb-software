// $Id: ReadAlpGen.cpp,v 1.3 2007-07-31 12:49:53 ibelyaev Exp $
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

#include "boost/filesystem.hpp"

// ============================================================================
// Local 
// ============================================================================
#include "LbPythia/ReadFile.h"
#include "LbAlpGen/Ahio.h"
// ============================================================================
/** @class ReadAlpGen ReadalpGen.cpp
 *  Simple production tool which reads parton level 
 *  files, produced by ALPGEN code  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-10-03
 */
class ReadAlpGen: public PythiaProduction
{
  /// friend factory for instantiation
  //  friend class ToolFactory<LbPythia::ReadAlpGen> ;
public:
  /// tool initialization 
  virtual StatusCode initialize () ;
  /// tool finalization 
  virtual StatusCode finalize   () ;
public:
  /** standard constructor 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  ReadAlpGen
  ( const std::string& type,
    const std::string& name,
    const IInterface* parent ) 
    : PythiaProduction ( type , name , parent ) 
    , m_LUN161    ( 0 )
    , m_LUN162    ( 0 )
    , m_LUN163    ( 0 )
  // no default value! 
    , m_fileLabel ()   ///< no default value! 
    , m_logfile   ( "AlpGen.log") ///< default value! 
  {
    // see ALPGENREADPROCESS
    PythiaProduction::m_userProcess    = 5 ; ///< see ALPGENREADPROCESS
    // Do not use TEMPORARLY the third argument to allow to use it in
    // Gauss v25rX
    // declareProperty ( "ParamFile" , m_paramfile , "The name of file with ALPGEN parameters") ;
    // declareProperty ( "LogFile"   , m_logfile   , "The name of ALPGEN's log-file") ;      
    declareProperty ( "FileLabel" , m_fileLabel ) ;
    declareProperty ( "LogFile"   , m_logfile ) ;      
  } ;                      
  virtual ~ReadAlpGen() {}
private:
  // the default constructor is disabled 
  ReadAlpGen() ;
  // the copy constructor is disabled 
  ReadAlpGen ( const ReadAlpGen & ) ;
  // assignement operator is disabled 
  ReadAlpGen& operator=( const ReadAlpGen& ) ;
private:
  // logical file unit  (F77)
  int         m_LUN161    ; ///< logical file unit  (F77)
  // logical file unit  (F77)
  int         m_LUN162    ; ///< logical file unit  (F77)
  // logical file unit  (F77)
  int         m_LUN163    ; ///< logical file unit  (F77)
  // name of parameters  file 
  std::string m_fileLabel ; ///< name of parameters  file 
  // name of log-file
  std::string m_logfile   ; ///< name of log-file
} ;
// ============================================================================
/// tool initialization 
// ============================================================================
StatusCode ReadAlpGen::initialize () 
{
  Ahio ahio ;
 
  boost::filesystem::path alpfile( std::tmpnam( NULL ) ) ;
  if ( boost::filesystem::exists( alpfile ) ) 
    boost::filesystem::remove( alpfile ) ;
  
  std::ofstream g( alpfile.string().c_str() ) ;
  g << m_fileLabel << std::endl ;    // label of the output files
  g.close() ;

  F77Utils::open( 89 , alpfile.string() , false ) ;  
 
  // open log file:
  // get free fortran Unit 
  m_LUN163 = F77Utils::getUnit ( msgLevel ( MSG::DEBUG ) ) ;
  if ( 0 >= m_LUN163 ) { return Error("No free FORTRAN unit available ") ; }
  StatusCode sc = F77Utils::openUnknown 
    ( m_LUN163 , m_logfile , msgLevel ( MSG::INFO ) ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Could not open the file '" + m_logfile + "'" ) ; }
  Pythia::pypars().mstp ( 163 ) = m_LUN163 ;                            // ATTENTION!
  
  //
  sc = PythiaProduction::initialize () ;

  F77Utils::close( 89 ) ;
  boost::filesystem::remove( alpfile ) ;

  return sc ;
}
// ============================================================================
// tool finalization 
// ============================================================================
StatusCode ReadAlpGen::finalize   () 
{
  { // close log-file 
    StatusCode sc = F77Utils::close ( m_LUN163 , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { Error ( "Error in closing '" + m_logfile + "'" , sc ) ; } // NO RETURN !
    m_LUN163 = 0 ;
    Pythia::pypars().mstp ( 163 ) = 0 ;                      // ATTENTION!
  }
  //
  return PythiaProduction::finalize() ;
} 
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY(ReadAlpGen) ;

// ============================================================================
// The END 
// ============================================================================
