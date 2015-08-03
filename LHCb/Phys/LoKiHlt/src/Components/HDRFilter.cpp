// $Id: HDRFilter.cpp,v 1.2 2009-05-20 08:57:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// HltEvent
// ============================================================================
#include "Event/HltDecReports.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/HLTTypes.h"
#include "LoKi/IHltFactory.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/algorithm/string.hpp"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class HDRFilter 
   *  Simple filtering algorithm bases on LoKi/Bender "hybrid" approach
   *  for filtering according to LHCb::HltDecReports 
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   *  @date 2008-09-23
   */
  class HDRFilter : public LoKi::FilterAlg 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<LoKi::HDRFilter> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialization 
    virtual StatusCode initialize () ;
    /// the main method: execute 
    virtual StatusCode execute    () ;
    // ========================================================================
  public:
    // ========================================================================
    /** Decode the functor (use the factory)
     *  @see LoKi::FilterAlg
     *  @see LoKi::FilterAlg::decode
     *  @see LoKi::FilterAlg::i_decode
     */
    virtual StatusCode decode () 
    {
      StatusCode sc = i_decode<LoKi::Hybrid::IHltFactory> ( m_cut ) ;
      Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @see LoKi::FilterAlg 
     *  @see GaudiAlgorithm 
     *  @see      Algorithm 
     *  @see      AlgFactory
     *  @see     IAlgFactory
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    HDRFilter
    ( const std::string& name ,   // the algorithm instance name 
      ISvcLocator*       pSvc ) ; // pointer to the service locator
    /// virtual and protected destructor 
    virtual ~HDRFilter () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    HDRFilter () ;                       // the default constructor is disabled 
    /// the copy constructor is disabled
    HDRFilter ( const HDRFilter& ) ;        // the copy constructor is disabled 
    /// the assignement operator is disabled 
    HDRFilter& operator=( const HDRFilter& ) ;   // the assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::Types::HLT_Cut  m_cut ;                         // the functor itself 
    /// TES location of LHCb::HltDecReports object 
    std::string m_location ;      // TES location of LHCb::HltDecReports object 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace 
{
  //
  inline bool _hlt1_ ( std::string name ) 
  {
    boost::to_upper ( name ) ;
    return std::string::npos != name.find ("HLT1" ) ;
  }
  //
  inline bool _hlt2_ ( std::string name ) 
  {
    boost::to_upper ( name ) ;
    return std::string::npos != name.find ("HLT2" ) ;
  }
  inline 
  //
  bool _ok_ ( const std::string& n1 )
  {
    const bool h1_1 = _hlt1_ ( n1 ) ;
    const bool h2_1 = _hlt2_ ( n1 ) ;
    //
    return ( h1_1 && !h2_1 ) || ( h2_1 && !h1_1 ) ;
  }
  //
  bool _ok_ ( const std::string& n1 , 
              const std::string& n2 ) 
  {
    const bool h1_1 = _hlt1_ ( n1 ) ;
    const bool h1_2 = _hlt1_ ( n2 ) ;
    const bool h2_1 = _hlt2_ ( n1 ) ;
    const bool h2_2 = _hlt2_ ( n2 ) ;
    //
    return  
      ( h1_1 && h1_2 && !h2_1 && !h2_2 ) || 
      ( h2_1 && h2_2 && !h1_1 && !h1_2 ) ;
  }
  bool _ok_ ( const std::string& n1 , 
              const std::string& n2 , 
              const std::string& n3 ) 
  {
    const bool h1_1 = _hlt1_ ( n1 ) ;
    const bool h1_2 = _hlt1_ ( n2 ) ;
    const bool h1_3 = _hlt1_ ( n3 ) ;
    const bool h2_1 = _hlt2_ ( n1 ) ;
    const bool h2_2 = _hlt2_ ( n2 ) ;
    const bool h2_3 = _hlt2_ ( n3 ) ;
    //
    return  
      ( h1_1 && h1_2 && h1_3 && !h2_1 && !h2_2 && !h2_3 ) || 
      ( h2_1 && h2_2 && h2_3 && !h1_1 && !h1_2 && !h1_3 ) ;
  }
  // special case: Hlt2*Hlt1*
  inline bool special_case ( const std::string& name )
  {
    
    return 
      4 < name.size()          && 
      0 == name.find ( "Hlt2") && _hlt1_ ( name ) ;  
  }
  // ==========================================================================
} // ==========================================================================
// ============================================================================
/*  standard constructor 
 *  @see LoKi::FilterAlg 
 *  @see GaudiAlgorithm 
 *  @see      Algorithm 
 *  @see      AlgFactory
 *  @see     IAlgFactory
 *  @param name the algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
LoKi::HDRFilter::HDRFilter
( const std::string& name , // the algorithm instance name 
  ISvcLocator*       pSvc ) // pointer to the service locator
  : LoKi::FilterAlg ( name , pSvc ) 
    // the functor itself
  , m_cut ( LoKi::BasicFunctors<const LHCb::HltDecReports*>::BooleanConstant( false ) ) 
    // TES location of LHCb::HltDecReports
  , m_location ( LHCb::HltDecReportsLocation::Default ) 
{
  //
  if ( special_case ( name ) )
  { m_location =  LHCb::HltDecReportsLocation::Hlt2Default ; }    
  else if ( std::string::npos != name.find ( "Hlt1"  ) || 
            std::string::npos != name.find ( "HLT1"  ) ) 
  { m_location =  LHCb::HltDecReportsLocation::Hlt1Default ; }
  else if ( std::string::npos != name.find ( "Hlt2"  ) ||  
            std::string::npos != name.find ( "HLT2"  ) ) 
  { m_location =  LHCb::HltDecReportsLocation::Hlt2Default ; }
  else if ( std::string::npos != name.find ( "Strip" ) ||  
            std::string::npos != name.find ( "STRIP" ) ) 
  { m_location =  "Strip/Phys/DecReports"                  ; }
  //
  declareProperty 
    ( "Location" , 
      m_location , 
      "TES location of LHCb::HltDecReports object" ) ;    
  //
  StatusCode sc = setProperty ( "Code" , "HLT_NONE" ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Code'"    , sc ) ;
  sc = setProperty 
    ( "Factory" , 
      0 == name.find ( "Hlt1" ) ? 
      "LoKi::Hybrid::HltFactory/Hlt1HltFactory:PUBLIC" : 
      0 == name.find ( "Hlt2" ) ?
      "LoKi::Hybrid::HltFactory/Hlt2HltFactory:PUBLIC" :
      "LoKi::Hybrid::HltFactory/HltFactory:PUBLIC"     ) ;
  Assert ( sc.isSuccess () , "Unable (re)set property 'Factory'" , sc ) ;
  //
} 
// ============================================================================
// initialization 
// ============================================================================
StatusCode LoKi::HDRFilter::initialize () 
{
  //
  StatusCode sc = LoKi::FilterAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  /// the special name 
  if ( special_case ( name() ) ) 
  {
    std::string s ( name() , 5 ) ;
    if ( !_ok_ ( s       , m_location ) ) 
    { Error    ( "Inconsistent setting of name&location/2"      ) ;  } 
    if ( !_ok_ ( code()  , m_location ) ) 
    { Error    ( "Inconsistent setting of code&location/2"      ) ;  } 
    if ( !_ok_ ( s       , code() , m_location ) ) 
    { Warning  ( "Inconsistent setting of name&code&location/2" ) ;  }
  }
  else if ( !_hlt1_ ( name ()     )   && 
            !_hlt1_ ( code ()     )   && 
            !_hlt1_ ( m_location  )   && 
            !_hlt2_ ( name ()     )   && 
            !_hlt2_ ( code ()     )   && 
            !_hlt2_ ( m_location  ) ) { /* stripping case? */ }
  else 
  {
    if ( !_ok_ ( code () , m_location ) ) 
    { Error    ( "Inconsistent setting of code&location      " ) ;  }
    if ( !_ok_ ( name () , m_location ) ) 
    { Warning  ( "Inconsistent setting of name&location      " ) ;  }
    if ( !_ok_ ( name() , code() , m_location ) ) 
    { Warning  ( "Inconsistent setting of name&code&location " ) ;  }
  }
  //
  return sc ;
}
// ============================================================================
// the main method: execute 
StatusCode LoKi::HDRFilter::execute () // the main method: execute 
{
  if ( updateRequired() ) 
  {
    StatusCode sc = decode() ; 
    Assert ( sc.isSuccess() , "Unable to decode the functor!" ) ;
  }
  // get LHCb::HltDecReports from TES 
  const LHCb::HltDecReports* hdr = get<LHCb::HltDecReports> ( m_location ) ;
  // 
  // use the functor 
  // 
  const bool result = m_cut ( hdr ) ;
  //
  // some statistics
  counter ("#passed" ) += result ;
  // 
  // set the filter:
  setFilterPassed ( result ) ;
  //
  return StatusCode::SUCCESS ;
}  
// ============================================================================
/// the factory (needed for instantiation)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,HDRFilter)
// ============================================================================
// The END
// ============================================================================

