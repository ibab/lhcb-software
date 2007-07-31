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
// ============================================================================
// Local 
// ============================================================================
#include "ReadFile.h"
// ============================================================================
namespace LbPythia
{
  /** @class ReadAlpGen ReadalpGen.cpp
   *  Simple production tool which reads parton level 
   *  files, produced by ALPGEN code  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-10-03
   */
  class ReadAlpGen: public LbPythia::ReadFile
  {
    /// friend factory for instantiation
    friend class ToolFactory<LbPythia::ReadAlpGen> ;
  public:
    /// tool initialization 
    virtual StatusCode initialize () ;
    /// tool finalization 
    virtual StatusCode finalize   () ;
  protected:
    /** standard constructor 
     *  @param type tool type (?)
     *  @param name tool name 
     *  @param parent tool parent 
     */
    ReadAlpGen
    ( const std::string& type,
      const std::string& name,
      const IInterface* parent ) 
      : LbPythia::ReadFile ( type , name , parent ) 
      , m_LUN161    ( 0 )
      , m_LUN162    ( 0 )
      , m_LUN163    ( 0 )
        // no default value! 
      , m_paramfile ()   ///< no default value! 
      , m_logfile   ( "AlpGen.log") ///< default value! 
    {
      // see ALPGENREADPROCESS
      PythiaProduction::m_userProcess    = 5 ; ///< see ALPGENREADPROCESS
      // Do not use TEMPORARLY the third argument to allow to use it in
      // Gauss v25rX
      // declareProperty ( "ParamFile" , m_paramfile , "The name of file with ALPGEN parameters") ;
      // declareProperty ( "LogFile"   , m_logfile   , "The name of ALPGEN's log-file") ;      
      declareProperty ( "ParamFile" , m_paramfile ) ;
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
    std::string m_paramfile ; ///< name of parameters  file 
    // name of log-file
    std::string m_logfile   ; ///< name of log-file
  } ;
} // end of namespace LbPythia
// ============================================================================
/// tool initialization 
// ============================================================================
StatusCode LbPythia::ReadAlpGen::initialize () 
{
  // check input file name 
  if ( file().empty()      ) { return Error ( "Input file name is not specified!") ; }
  if ( m_paramfile.empty() ) { return Error ( "Input param-file name is not specified!") ; }
  if ( m_logfile.empty()   ) { return Error ( "Input log-file name is not specified!") ; }
  { // open input parameter file:
    // get free fortran Unit 
    m_LUN161 = F77Utils::getUnit ( msgLevel ( MSG::DEBUG ) ) ;
    if ( 0 >= m_LUN161 ) { return Error("No free FORTRAN unit available ") ; }
    StatusCode sc = F77Utils::openOld ( m_LUN161 , m_paramfile , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Could not open the file '" + m_paramfile + "'" ) ; }
    Pythia::pypars().mstp ( 161 ) = m_LUN161 ;                            // ATTENTION!
  }
  { // open input events file:
    // get free fortran Unit 
    m_LUN162 = F77Utils::getUnit ( msgLevel ( MSG::DEBUG ) ) ;
    if ( 0 >= m_LUN162 ) { return Error("No free FORTRAN unit available ") ; }
    StatusCode sc = F77Utils::openOld ( m_LUN162 , file() , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Could not open the file '" + file() + "'" ) ; }
    Pythia::pypars().mstp ( 162 ) = m_LUN162 ;                            // ATTENTION!
  }
  { // open log file:
    // get free fortran Unit 
    m_LUN163 = F77Utils::getUnit ( msgLevel ( MSG::DEBUG ) ) ;
    if ( 0 >= m_LUN163 ) { return Error("No free FORTRAN unit available ") ; }
    StatusCode sc = F77Utils::openUnknown 
      ( m_LUN163 , m_logfile , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Could not open the file '" + m_logfile + "'" ) ; }
    Pythia::pypars().mstp ( 163 ) = m_LUN163 ;                            // ATTENTION!
  }
  //
  return LbPythia::ReadFile::initialize () ;
}
// ============================================================================
// tool finalization 
// ============================================================================
StatusCode LbPythia::ReadAlpGen::finalize   () 
{
  { // close input param file 
    StatusCode sc = F77Utils::close ( m_LUN161 , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { Error ( "Error in closing '" + m_paramfile + "'" , sc ) ; } // NO RETURN !
    m_LUN161 = 0 ;
    Pythia::pypars().mstp ( 161 ) = 0 ;                      // ATTENTION!
  }
  { // close input events file 
    StatusCode sc = F77Utils::close ( m_LUN162 , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { Error ( "Error in closing '" + file() + "'" , sc ) ; } // NO RETURN !
    m_LUN162 = 0 ;
    Pythia::pypars().mstp ( 162 ) = 0 ;                      // ATTENTION!
  }
  { // close log-file 
    StatusCode sc = F77Utils::close ( m_LUN163 , msgLevel ( MSG::INFO ) ) ;
    if ( sc.isFailure() ) 
    { Error ( "Error in closing '" + m_logfile + "'" , sc ) ; } // NO RETURN !
    m_LUN163 = 0 ;
    Pythia::pypars().mstp ( 163 ) = 0 ;                      // ATTENTION!
  }
  //
  return LbPythia::ReadFile::finalize() ;
} 
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LbPythia,ReadAlpGen)

// ============================================================================
// The END 
// ============================================================================
