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
#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
#include "boost/assign/list_of.hpp"
#endif

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
    inline mask_type get()  { asm volatile("fwait"); return fegetexcept() & FE_ALL_EXCEPT; }
    inline mask_type disable(mask_type mask) { return fedisableexcept( mask & FE_ALL_EXCEPT ); }
    inline mask_type enable(mask_type mask)  {
      mask &= FE_ALL_EXCEPT ;
      feclearexcept(mask); // remove any 'stale' exceptions before switching on trapping
                           // otherwise we immediately trigger an exception...
      return feenableexcept(mask) & FE_ALL_EXCEPT;
    }
    inline const std::map<std::string,mask_type>& map() {
      static std::map<std::string,mask_type> m =
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
        {{ "Inexact"   , mask_type(FE_INEXACT)   },
         { "DivByZero" , mask_type(FE_DIVBYZERO) },
         { "Underflow" , mask_type(FE_UNDERFLOW) },
         { "Overflow"  , mask_type(FE_OVERFLOW)  },
         { "Invalid"   , mask_type(FE_INVALID)   },
         { "AllExcept" , mask_type(FE_ALL_EXCEPT)}};
#else
        boost::assign::map_list_of
        ( "Inexact"   , mask_type(FE_INEXACT)  )
        ( "DivByZero" , mask_type(FE_DIVBYZERO))
        ( "Underflow" , mask_type(FE_UNDERFLOW))
        ( "Overflow"  , mask_type(FE_OVERFLOW) )
        ( "Invalid"   , mask_type(FE_INVALID)  )
        ( "AllExcept" , mask_type(FE_ALL_EXCEPT));
#endif
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
    inline mask_type get() { __asm { fwait };  return _controlfp(0,0); }
    inline mask_type disable(mask_type mask) {
      int cw = get(); // Get current control word
      cw |= ~mask; // set control bits, turn exceptions off
      return _controlfp(cw,_MCW_EM);}
    inline mask_type enable(mask_type mask)  {
      _clearfp(); // remove any 'stale' exceptions before switching on trapping
                  // otherwise we immediately trigger an exception...
      int cw = mask;
      return _controlfp(cw,_MCW_EM);}
    inline const std::map<std::string,mask_type>& map() {
      static std::map<std::string,mask_type> m = boost::assign::map_list_of
        ( "Inexact"   , mask_type(~EM_INEXACT)   )
        ( "DivByZero" , mask_type(~EM_ZERODIVIDE))
        ( "Underflow" , mask_type(~EM_UNDERFLOW) )
        ( "Overflow"  , mask_type(~EM_OVERFLOW)  )
        ( "Invalid"   , mask_type(~EM_INVALID)   )
        ( "AllExcept" , mask_type(~(EM_INVALID|EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL)));
      return m;
    }
    /// Default mask (for default FPE::Guard constructor)
    static const mask_type s_default_guard_mask = ~(EM_INVALID|EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL);
#else
    static const bool has_working_implementation = false;
    typedef int mask_type;
    inline mask_type get() { return 0; }
    inline mask_type disable(mask_type) { return 0; }
    inline mask_type enable(mask_type) { return 0; }
    inline const std::map<std::string,mask_type>& map() {
      static std::map<std::string,mask_type> m;
      return m;
    }
    /// Default mask (for default FPE::Guard constructor)
    static const mask_type s_default_guard_mask = 0;
#endif

  } // namespace detail

  /** @class Guard FPEGuard.h LHCbKernel/FPEGuard.h
   *
   *  Create a guard which, depending on the value of the 'disable'
   *  constructor argument,
   *  enables (disables) the trapping of Floating Point Exceptions
   *  (i.e. the generation of a SIGFPE signal) according to the
   *  specified mask, for the duration of lifetime of the guard.
   *
   *  The destructor will restore the state at the time of the
   *  creation of the guard.
   *
   *  Example usage :-
   *  @code
   *
   *  // Create a mask and Guard for a single exception type
   *  // For the lifetime of the guard object an FPE will be thrown
   *  // for any 'Invalid' floating point errors.
   *  FPE::Guard guard( FPE::Guard::mask("Invalid") );
   *
   *  // Pass a range of strings to FPE::Guard::mask and assign the result
   *  // to a FPE::Guard::mask_type, i.e:
   *  // create a vector of valid names, such as 'Invalid', 'Overflow', etc
   *  std::vector<std::string> vec = ..... ;
   *  FPE::Guard::mask_type mask = FPE::Guard::mask( vec.begin(), vec.end() );
   *  FPE::Guard guard( mask );
   *
   *  // The Default Guard enables all known exceptions
   *  // This is the simpliest use case
   *  FPE::Guard guard;
   *
   *  // To disable instead of enable expections, pass disable=true
   *  FPE::Guard guard( true );
   *
   *  // Usage of this Guard is only intended in cases where it is impossible
   *  // to fix problems properly, such as exceptions from externals libraries
   *  // like GSL or CLHEP. It should not be used to mask problems in your code ;)
   *
   *  // You should also minimise the lifetime of the guard as much as possible,
   *  // for instance by using a localised scope around the problematic call, e.g.
   *  double result(0);
   *  {
   *    FPE::Guard guard;
   *    result = problematic_external_library_method();
   *  }
   *  // or by using new/delete
   *  FPE::Guard * guard = new FPE::Guard();
   *  double result = problematic_external_library_method();
   *  delete guard;
   *
   *  @endcode
   *
   *  @attention The methods FPE::Guard::mask(...) are non-trivial (map lookup).
   *             Thus you should avoid calling these too frequently in local scopes.
   *             If needed create the mask once and cache, or use the default
   *             FPE::Guard constructor.
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
     *  @param mask    The mask of exceptions to activate/deactive
     *  @param disable Disable or enable the given FPE exceptions
     */
    explicit Guard( mask_type mask,
		    bool disable   = false )
      : m_initial( disable ?
                   FPE::detail::disable(mask) :
                   FPE::detail::enable(mask)  )
    { }

    /** Default Constructor. Activates/Deactivate all known FPE exceptions.
     *
     *  @param disable Disable(true) or enable(false) all known FPE exceptions
     *                 (default is to enable exceptions).
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
          throw GaudiException("oops -- Guard failed to restore initial state","FPE::Guard",StatusCode::FAILURE);
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
     *   @return The exception mask for the given list of exception strings
     */
    template <typename Iter>
    static mask_type mask(Iter begin, Iter end)
    {
#if defined(_WIN32)
      mask_type m = 0x9ffff;
      for (;begin!=end;++begin) { m &= mask(*begin); }
#else
      mask_type m = 0;
      for (;begin!=end;++begin) { m |= mask(*begin); }
#endif
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
     *   @return The exception mask for the given exception string
     */
    static mask_type mask( const std::string & excpt )
    {
      mask_type mask(0);
      if ( FPE::detail::has_working_implementation )
      {
	auto j = FPE::detail::map().find(excpt);
	if ( FPE::detail::map().end() == j )
	{
	  throw GaudiException("FPE::Guard::mask : Unknown mask "+excpt,excpt,StatusCode::FAILURE);
	}
	mask = j->second;
      }
      return mask;
    }

  private:

    mask_type m_initial; ///< Saved previous mask value

  };

} // namespace FPE

#endif
