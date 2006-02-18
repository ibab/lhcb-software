// $Id: Services.cpp,v 1.2 2006-02-18 18:06:04 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/24 09:39:41  ibelyaev
// New Import: the core part of restructurized LoKi project
//
// ===========================================================================
// Include files 
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/Services.h"
#include "LoKi/Welcome.h"
#include "LoKi/Report.h"
// ===========================================================================


// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Services
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ===========================================================================
/// accessor to unique instance ("Meyer's singleton" pattern)
// ===========================================================================
LoKi::Services& LoKi::Services::instance()
{
  static LoKi::Services s_instance = Services() ;
  return s_instance ;
};
// ===========================================================================

// ===========================================================================
/// standard (default) constructor 
// ===========================================================================
LoKi::Services::Services()
  : m_lokiSvc  ( 0 ) 
  , m_ppSvc    ( 0 ) 
{
  LoKi::Welcome::instance() ;
};
// ===========================================================================

// ===========================================================================
/// destructor
// ===========================================================================
LoKi::Services::~Services(){};
// ===========================================================================

// ===========================================================================
// release all services 
// ===========================================================================
StatusCode LoKi::Services::releaseAll() 
{
  // release services 
  if ( 0 != m_ppSvc   ) { m_ppSvc   -> release () ; m_ppSvc   = 0 ; }
  // 'release' the service 
  if ( 0 != m_lokiSvc ) { m_lokiSvc -> release () ; m_lokiSvc = 0 ; }
  //
  return StatusCode::SUCCESS ;
};
// ===========================================================================

// ===========================================================================
/** Print the error  message, return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ===========================================================================
StatusCode LoKi::Services::Error     
( const std::string& msg , 
  const StatusCode   st  , 
  const size_t       mx  ) const  
{
  return LoKi::Report::Error(" LoKi::Services " + msg , st , mx );
};
// ===========================================================================

// ===========================================================================
/** Print the warning  message, return status code 
 *  @param msg    warning message 
 *  @param st     status code  
 *  @return       status code 
 */
// ===========================================================================
StatusCode LoKi::Services::Warning   
( const std::string& msg  , 
  const StatusCode   st   , 
  const size_t       mx   ) const   
{
  return LoKi::Report::Warning(" LoKi::Services " + msg , st , mx ) ;
} ;
// ===========================================================================


// ===========================================================================
/// accessor to main LoKi algorithm 
// ===========================================================================
LoKi::ILoKiSvc* LoKi::Services::lokiSvc () const { return m_lokiSvc ; };
// ===========================================================================

// ===========================================================================
/// set new main LoKi algorithms 
// ===========================================================================
LoKi::ILoKiSvc* LoKi::Services::setLoKi( LoKi::ILoKiSvc* svc )  
{
  // add reference to the new algo 
  if ( 0 != svc ) { svc -> addRef() ; }
  // release all  previously allocated  services  
  releaseAll() ;
  // set new algorithm
  m_lokiSvc = svc ;
  // get particle properties service from the algorithm 
  if ( 0 != m_lokiSvc ) { m_ppSvc = m_lokiSvc -> ppSvc () ; }
  if ( 0 != m_ppSvc   ) { m_ppSvc -> addRef            () ; }
  //
  return lokiSvc();
};
// ===========================================================================

// ===========================================================================
/// accessor to particle properties service
// ===========================================================================
IParticlePropertySvc* LoKi::Services::ppSvc     () const 
{
  if ( 0 != m_ppSvc ) { return m_ppSvc ; }
  Error ( " IParticlePropertySvc* points to NULL, return NULL" ) ;
  return 0 ;
};
// ===========================================================================


// ===========================================================================
// The END 
// ===========================================================================
