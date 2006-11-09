// $Id: Reporter.cpp,v 1.6 2006-11-09 18:26:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/11/09 17:01:06  ibelyaev
//  v1r8: improve printout
//
// Revision 1.4  2006/06/24 17:18:41  ibelyaev
//  reduce the printout
//
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================ 
// LoKi
// ============================================================================ 
#include "LoKi/IReporter.h"
#include "LoKi/Welcome.h"
#include "LoKi/ErrorReport.h"
// ============================================================================
/** @file
 *
 *  The implementation file for the class LoKi::Reporter 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  /** @class Reporter Reporter.h LoKi/Reporter.h
   *  
   *  Simple class (tool)  for error/warning/exception
   *  reports from different LoKi components 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2003-01-16
   */
  class Reporter : public virtual LoKi::IReporter ,
                   public               GaudiTool 
  {
    // friend factory for instantiation 
    friend class ToolFactory<LoKi::Reporter>;
  public:    
    //
    virtual StatusCode Error     
    ( const std::string& msg , 
      const StatusCode   st  = StatusCode::FAILURE ,
      const size_t       mx  = 10                  ) const 
    {  return GaudiTool::Error   ( msg , st , mx ) ; }
    //
    virtual StatusCode Warning   
    ( const std::string& msg , 
      const StatusCode   st  = StatusCode::FAILURE ,
      const size_t       mx  = 10                  ) const 
    {  return GaudiTool::Warning ( msg , st , mx ) ; }
    //
    virtual StatusCode Print     
    ( const std::string& msg , 
      const StatusCode   st  = StatusCode::SUCCESS ,
      const MSG::Level   lev = MSG::INFO           ) const 
    {  return GaudiTool::Print ( msg , st , lev ) ; }
    //
    virtual StatusCode Assert 
    ( const bool         ok                            , 
      const std::string& message = ""                  , 
      const StatusCode   sc      = StatusCode::FAILURE ) const 
    {  return GaudiTool::Assert ( ok , message , sc ) ; }
    //
    virtual StatusCode Exception 
    ( const std::string    & msg                        ,  
      const GaudiException & exc                        , 
      const StatusCode       sc  = StatusCode::FAILURE  ) const 
    {  return GaudiTool::Exception ( msg , exc , sc ) ; }
    //
    virtual StatusCode Exception 
    ( const std::string    & msg                        ,  
      const std::exception & exc                        , 
      const StatusCode       sc  = StatusCode::FAILURE  ) const 
    {  return GaudiTool::Exception ( msg , exc , sc ) ; }
    //
    virtual StatusCode Exception 
    ( const std::string& msg = "no message"        ,  
      const StatusCode   sc  = StatusCode::FAILURE ) const 
    {  return GaudiTool::Exception ( msg      , sc ) ; }
    //
  public:
    /** standard initialization
     *  @see AlgTool
     *  @return status code 
     */
    virtual StatusCode initialize   () 
    {
      StatusCode sc = GaudiTool::initialize();
      if ( sc.isFailure() ) { return sc ; }
      //
      LoKi::ErrorReport& rep = LoKi::ErrorReport::instance() ;
      if ( 0 == rep.reporter() ) { rep.setReporter ( this ); }
      // locate LoKi service 
      return StatusCode::SUCCESS ;
    } ;
    /** standard finalization  
     *  @see AlgTool
     *  @return status code 
     */
    virtual StatusCode finalize     () 
    {
      // check for global reporter 
      LoKi::ErrorReport& rep = LoKi::ErrorReport::instance();
      // owner for reporter?
      if ( rep.reporter() == this ) { rep.report(); rep.setReporter( 0 ); }      
      // finalize the base class 
      return GaudiTool::finalize();
    } ;
  protected:
    /** Standard constructor for AlgTools
     *  @see AlgTool 
     *  @param type   tool type 
     *  @param name   tool name 
     *  @param parent tool parent 
     */
    Reporter
    ( const std::string& type   , 
      const std::string& name   ,
      const IInterface*  parent ) 
      : GaudiTool ( type, name , parent ) 
    {
      // declare the interface 
      declareInterface<LoKi::IReporter> ( this ) ;
      declareInterface<IErrorTool>      ( this ) ;
    } ;
    /// virtual destructor 
    virtual ~Reporter(){} ;
  private:
    Reporter            (                 ) ;
    Reporter            ( const Reporter& ) ;
    Reporter& operator= ( const Reporter& ) ;
  };
}
// ============================================================================
// anonymouse namepsace to hide static factory
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  concrete factory for instantriation of class LoKi::Reporter 
   *  @date 2006-01-17
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   */
  // ==========================================================================
  const  ToolFactory<LoKi::Reporter>       s_Factory ;
} ;
// ============================================================================
/** @var LoKiReporterFactory 
 *  abstract factory for instantriation of class LoKi::Reporter 
 *  @date 2006-01-17
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
const   IToolFactory&LoKiReporterFactory = s_Factory ; 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
