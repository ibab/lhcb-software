// $Id: Assert.h,v 1.1 2007-08-11 20:17:00 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_ASSERT_H 
#define LOKI_ASSERT_H 1
// ============================================================================
// Include files
// ============================================================================
namespace LoKi 
{
  /** throw LoKi::Exception 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-11
   */
  void throwException ( const std::string& message ) ;
  // ==========================================================================
  /** assertion  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-11
   */
  inline void Assert 
  ( const bool         assertion , 
    const std::string& message   ) 
  {
    if ( !assertion ) { throwException ( message ) ; }
  }
  // ==========================================================================
  /** assertion  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-11
   */
  inline void Assert 
  ( const bool         assertion , 
    const char*        message   ) 
  {
    if ( !assertion ) { throwException ( message ) ; }
  }
  // ==========================================================================
  /** assertion  
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-11
   */
  template <unsigned int N>
  inline void Assert 
  ( const bool   assertion   ,
    const char (&message)[N] )
  {
    if ( !assertion ) 
    { throwException ( std::string ( message , message + N ) ) ; }
  }  
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ASSERT_H
// ============================================================================
