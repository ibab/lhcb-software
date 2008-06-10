#ifndef FPEGUARD_H
#define FPEGUARD_H 1
#if defined(linux) && defined(__GNUC__)
#include <fenv.h>
#elif defined(_WIN32)
#include <float.h>
#endif

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "GaudiKernel/GaudiException.h"
#include "boost/assign/list_of.hpp"

/** @namespace FPE
 *
 *  Namespace For Floating Point Exception Handling code
 *
 *  @author Gerhard Raven
 *  @date   09/06/2008
 */
namespace FPE {

  /** @namespace detail
   *
   *  This namespace contains machine & libc specific code
   *  required to implement the functionality used by FPEGuard
   *  Do not use directly.... if you do, you are on your own....
   *
   *  @author Gerhard Raven
   *  @date   09/06/2008
   */
  namespace detail {

#if defined(linux) && defined(__GNUC__)
    static const bool has_working_implementation = true;
    typedef int mask_type;
    // make sure the FPU has caught up by explicitly issueing an fwait...
    mask_type get()  { asm volatile("fwait"); return fegetexcept() & FE_ALL_EXCEPT; }
    mask_type disable(mask_type mask) { return fedisableexcept( mask & FE_ALL_EXCEPT ); }
    mask_type enable(mask_type mask)  {
      mask &= FE_ALL_EXCEPT ;
      feclearexcept(mask); // remove any 'stale' exceptions before switching on trapping
                           // otherwise we immediately trigger an exception...
      return feenableexcept(mask) & FE_ALL_EXCEPT;
    }
    const std::map<std::string,mask_type>& map() {
      static std::map<std::string,mask_type> m = boost::assign::map_list_of
        ( "Inexact"   , mask_type(FE_INEXACT)  )
        ( "DivByZero" , mask_type(FE_DIVBYZERO))
        ( "Underflow" , mask_type(FE_UNDERFLOW))
        ( "Overflow"  , mask_type(FE_OVERFLOW) )
        ( "Invalid"   , mask_type(FE_INVALID)  )
        ( "AllExcept" , mask_type(FE_ALL_EXCEPT));
      return m;
    }
    /// Default mask (for default FPE::Guard constructor)
    static const mask_type s_default_guard_mask ( FE_ALL_EXCEPT );
#elif defined(_WIN32)
    static const bool has_working_implementation = true;
    typedef unsigned int mask_type;
    // VS8
    // mask_type disable(mask_type mask) { mask_type p; _controlfp_s(&p,~mask,_MCW_EM); return p;}
    // mask_type enable(mask_type mask)  { mask_type p; _controlfp_s(&p, mask,_MCW_EM); return p;}
    // VS7
    mask_type get() { __asm { fwait };  return _controlfp(0,0); }
    mask_type disable(mask_type mask) { return _controlfp(~mask,_MCW_EM);}
    mask_type enable(mask_type mask)  { return _controlfp( mask,_MCW_EM);}
    const std::map<std::string,mask_type>& map() {
      static std::map<std::string,mask_type> m = boost::assign::map_list_of
        ( "Inexact"   , mask_type(EM_INEXACT)   )
        ( "DivByZero" , mask_type(EM_ZERODIVIDE))
        ( "Underflow" , mask_type(EM_UNDERFLOW) )
        ( "Overflow"  , mask_type(EM_OVERFLOW)  )
        ( "Invalid"   , mask_type(EM_INVALID)   )
        ( "AllExcept" , mask_type(EM_INVALID|EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));
      return m;
    }
    /// Default mask (for default FPE::Guard constructor)
    static const mask_type s_default_guard_mask = EM_INVALID|EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL;
#else
    static const bool has_working_implementation = false;
    typedef int mask_type;
    mask_type get() { return 0; }
    mask_type disable(mask_type) { return 0; }
    mask_type enable(mask_type) { return 0; }
    const std::map<std::string,mask_type>& map() {
      static std::map<std::string,mask_type> m;
      return m;
    }
    /// Default mask (for default FPE::Guard constructor)
    static const mask_type s_default_guard_mask = 0;
#endif

  } // namespace detail

  /** @class Guard GaudiKernel/FPEGuard.h
   *
   *  Create a guard which, depending on the value of 'disable',
   *  enables (disables) the trapping of Floating Point Exceptions
   *  (i.e. the generation of a SIGFPE signel) according to the
   *  specified mask, for the duration of lifetime of the guard.
   *  The destructor will restore the state at the time of the
   *  creation of the guard.
   *
   *  Example usage :-
   *  @code
   *  // You pass a range of strings to FPE::Guard::mask, and assign the result
   *  // to a FPE::Guard::mask_type, i.e:
   *  std::vector<std::string> vec = ..... ; // create a vector of valid names, such as 'Invalid', 'Overflow', etc
   *  FPE::Guard::mask_type mask = FPE::Guard::mask( vec.begin(), vec.end() );
   *  FPE::Guard guard( mask );
   *
   *  // To switch of all known exceptions
   *  std::vector<std::string> vec = boost::assign::list_of( "AllExcept" );
   *  FPE::Guard::mask_type mask = FPE::Guard::mask( vec.begin(), vec.end() );
   *  bool disable = true;
   *  FPE::Guard guard( mask , disable );
   *  @endcode
   *
   *  @author Gerhard Raven
   *  @date   09/06/2008
   */
  class Guard {
  public:
    /// export the type of the FPE mask
    typedef FPE::detail::mask_type mask_type;
    /// Export whether a working implementation exists.
    /// In case it doesn't, the code (silently!) defaults
    /// to no-operation.
    static const bool has_working_implementation = FPE::detail::has_working_implementation;

    /** Constructor with explicit mask
     *
     *  @attention Note: to create a (valid) mask, use FPE::Guard::mask
     *
     *  @param The mask of exceptions to guard against
     *  @param disable Disable or enable the given exceptions
     */
    Guard( mask_type mask,
           bool disable   = false )
      : m_initial( disable ? 
                   FPE::detail::disable(mask) : 
                   FPE::detail::enable(mask)  )
    { }

    /** Default Constructor. Activates protection for all known exceptions
     *
     *  @param disable Disable or enable the given exceptions
     */
    explicit Guard( bool disable = false )
      : m_initial( disable ?
                   FPE::detail::disable ( FPE::detail::s_default_guard_mask ) :
                   FPE::detail::enable  ( FPE::detail::s_default_guard_mask ) )
    { }

    /// Destructor. Returns system to the same state as before the object was constructed
    ~Guard()
    {
      //TODO: in disable mode, report which FPE happened between c'tor and d'tor.
      FPE::detail::disable( ~m_initial );
      mask_type mask = FPE::detail::enable( m_initial );
      // retry once, in case the FPU is a bit behind... yes, that happens
      if (mask!=m_initial && FPE::detail::get()!=m_initial) { 
          throw GaudiException("oops -- FPEGuard failed to restore initial state","",StatusCode::FAILURE);
      }
    }

    /**  Convert a range of strings (i.e. Iter::value_type
     *   must be of type std::string) into the corresponding mask to be used
     *   to enable (or disable) FPE trapping by the FPE::Guard.
     *
     *   Valid strings are "Inexact","DivByZero","Underflow","OverFlow","Invalid"
     *   and the catch-all "AllExcept".
     *
     *   @param begin Begin iterator
     *   @param end   End iterator
     *
     *   @return The exeption mask for the given list of exception strings
     */
    template <typename Iter>
    static mask_type mask(Iter begin, Iter end) {
      mask_type m=0;
      for (;begin!=end;++begin) {
        std::map<std::string,mask_type>::const_iterator j = FPE::detail::map().find(*begin);
        if (j==FPE::detail::map().end()) {
          throw GaudiException("FPE::mask: unknown mask... ",*begin,StatusCode::FAILURE);
        }
        m |= j->second;
      }
      return m;
    }

    /**  Convert a string into the corresponding mask to be used
     *   to enable (or disable) FPE trapping by the FPE::Guard.
     *
     *   Valid strings are "Inexact","DivByZero","Underflow","OverFlow","Invalid"
     *   and the catch-all "AllExcept".
     *
     *   @param excpt The exception type
     *
     *   @return The exeption mask for the given exception string
     */
    static mask_type mask( const std::string & excpt )
    {
      const std::vector<std::string> excpts(1,excpt);
      return mask( excpts.begin(), excpts.end() );
    }

  private:
    mask_type m_initial; ///< Saved mask
  };

} // namespace FPE
#endif
