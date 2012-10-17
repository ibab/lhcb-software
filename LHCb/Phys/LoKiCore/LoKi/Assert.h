// ============================================================================
#ifndef LOKI_ASSERT_H 
#define LOKI_ASSERT_H 1
// ============================================================================
// Include files
// ============================================================================
/** @file LoKi/Assert.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */
namespace LoKi 
{
  // ==========================================================================
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
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ASSERT_H
// ============================================================================
