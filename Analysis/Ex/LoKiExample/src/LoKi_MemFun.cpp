// $Id: LoKi_MemFun.cpp,v 1.1 2009-05-21 13:58:58 ibelyaev Exp $
// ===========================================================================
// Include files 
// ===========================================================================
// LoKi 
// ===========================================================================
#include "LoKi/LoKi.h"
// ===========================================================================
/** @file 
 *  simple example on the extension of LoKi-based algorithm with some 
 *  data-members and member-functions 
 *  
 *  This file is a part of LoKi project - 
 *  \"C++ ToolKit  for Smart and Friendly Physics Analysis\"
 * 
 * The package has been designed with the kind help from
 * Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 * contributions and advices from G.Raven, J.van Tilburg, 
 * A.Golutvin, P.Koppenburg have been used in the design.
 *
 * By usage of this code one clearly states the disagreement 
 * with the campain of Dr.O.Callot et al.: 
 *   "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 * @date 2009-05-21
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class MemFun 
   *  simple example on the extension of LoKi-based algorithm with some 
   *  data-members and member-functions 
   *  
   * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   * @date 2009-05-21
   */
  // ============================================================================
  class MemFun : public LoKi::Algo
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<LoKi::MemFun> ;                      // the factory
    // ========================================================================
  public:
    // ========================================================================
    /// OPTIONAL: specific initialization (if needed) 
    virtual StatusCode initialize () ;
    /// OPTIONAL: specific finalization (if needed) 
    virtual StatusCode finalize   () ;
    // ========================================================================
  public:
    // ========================================================================
    /// MANDATORY: analyse  
    virtual StatusCode analyse    () ;
    // ========================================================================
  public:
    // ========================================================================
    /// the simplest member function 
    double memFun1 () const ;
    /// the simplest member function which takes an argument 
    const LHCb::Particle* memFun3 ( const LoKi::Types::Range& r ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor form the name and service locator 
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    MemFun ( const std::string& name ,        //    the algorithm instance name 
             ISvcLocator*       pSvc ) ;      //     pointer to Service Locator
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~MemFun(){}                       // virtual & protected destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    MemFun () ;                          // the default constructor is disabled    
    /// copy constructor is disabled 
    MemFun ( const MemFun& ) ;           //        copy constructor is disabled 
    /// assignement operator is disabled 
    MemFun& operator=( const MemFun& ) ; //             assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// local variable: int
    int         m_int    ;                            // local variable:    int
    /// local variable: double
    double      m_double ;                            // local variable: double
    /// local variable: string
    std::string m_string ;                            // local variable: string
    // ========================================================================
  } ; // end of class MemFun 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/*  standard constructor form the name and service locator 
 *  @param name the algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::MemFun::MemFun
( const std::string& name ,        //    the algorithm instance name 
  ISvcLocator*       pSvc )        //     pointer to Service Locator
  : LoKi::Algo ( name , pSvc ) 
  , m_int    ( -1  ) 
  , m_double ( -1  ) 
  , m_string ()
{
  declareProperty ( "MyInt"    , m_int    , "Local (int)    property" ) ;
  declareProperty ( "MyDouble" , m_double , "Local (double) property" ) ;
  declareProperty ( "MyString" , m_string , "Local (string) property" ) ; 
}
// ============================================================================
// OPTIONAL: specific initialization (if needed) 
// ============================================================================
StatusCode LoKi::MemFun::initialize () 
{
  /// initialize the base 
  StatusCode sc = LoKi::Algo::initialize () ;            // initialize the base 
  if ( sc.isFailure () ) { return sc ; }                              // RETURN 
  
  /// some local action (if needed) 
  info ()  << "My Local (int)    variable is '" << m_int    << "'" << endmsg ;
  info ()  << "My Local (double) variable is '" << m_double << "'" << endmsg ;
  info ()  << "My Local (string) variable is '" << m_string << "'" << endmsg ;    
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// OPTIONAL: specific finalization (if needed) 
// ============================================================================
StatusCode LoKi::MemFun::finalize () 
{
  /// some local action (if needed) 
  info ()  << "My Local (int)    variable is '" << m_int    << "'" << endmsg ;
  info ()  << "My Local (double) variable is '" << m_double << "'" << endmsg ;
  info ()  << "My Local (string) variable is '" << m_string << "'" << endmsg ;    
  /// finalize the base 
  return LoKi::Algo::finalize () ;                         // finalize the base  
} 
// ============================================================================
// MANDATORY: analyse  
// ============================================================================
StatusCode LoKi::MemFun::analyse    () 
{
  
  using namespace LoKi        ;
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;
  
  // get all input particles:
  Range all = select ( "all" , ALL ) ;
  
  counter ("#all") += all.size() ;

  // use the first member function: 
  const double result1 = memFun1() ;
  info () << " evaluate memFun1 : " << result1 << endmsg ;
  
  
   // use the third member function 
  const LHCb::Particle* p = memFun3 ( all ) ;
  info () << " select the particle? " 
          << (  0 != p  ? "True" : "False"  ) << endmsg ;
  
  return StatusCode::SUCCESS ;
}    
// ============================================================================
// the simplest member function 
// ============================================================================
double LoKi::MemFun::memFun1 () const 
{
  info() << " I am LoKi::MemFun::memFun1" << endmsg ;
  return 10 ;                                                         // RETURN 
}  
// ============================================================================
// the simplest member function which takes an argument 
// ============================================================================
const LHCb::Particle* 
LoKi::MemFun::memFun3 ( const LoKi::Types::Range& r ) const 
{
  info() << " I am LoKi::MemFun::memFun3" << endmsg ;  
  if ( r.empty() ) { return 0 ; }                                     // RETURN 
  return r.front() ;                                                  // RETURN 
}
// ============================================================================
/// the factory:
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LoKi , MemFun )             // the factory
// ============================================================================
// The END 
// ============================================================================
