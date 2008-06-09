#ifndef FPEGUARD_H
#define FPEGUARD_H 1
#if defined(linux) && defined(__GNUC__)
#include <fenv.h>
#elif defined(_WIN32)
#include <float.h>
#endif

#include <map>
#include <string>
#include <iostream>
#include "GaudiKernel/GaudiException.h"
#include "boost/assign/list_of.hpp"


namespace FPE { 
    
    
namespace detail {
    /// This namespace contains machine & libc specific code
    /// required to implement the functionality used by FPEGuard
    /// Do not use directly.... if you do, you are on your own....
#if defined(linux) && defined(__GNUC__)
    typedef int mask_type;
    mask_type disable(mask_type mask) { return fedisableexcept(mask); }
    mask_type enable(mask_type mask)  { 
        feclearexcept(mask); // remove any 'stale' exceptions before switching on trapping
                             // otherwise we immediately trigger an exception...
        return feenableexcept(mask); 
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
#elif defined(_WIN32)
    typedef unsigned int mask_type;
    // VS8
    // mask_type disable(mask_type mask) { mask_type p; _controlfp_s(&p,~mask,_MCW_EM); return p;}
    // mask_type enable(mask_type mask)  { mask_type p; _controlfp_s(&p, mask,_MCW_EM); return p;}
    // VS7
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
#else
    typedef int mask_type;
    mask_type disable(mask_type) { 
      std::cerr << "Warning: FPE trapping requested, but not available on this architecture... " << std::endl;
      return 0; 
    }
    mask_type enable(mask_type) { return disable(); }
    const std::map<std::string,mask_type>& map() {
         static std::map<std::string,mask_type> m;
         return m;
    }
#endif
} // namespace detail


class Guard {
public: 
    /// export the type of the FPE mask
    typedef FPE::detail::mask_type mask_type;

    /// create a guard which, depending on the value of 'disable', 
    /// enables (disables) the trapping of Floating Point Exceptions
    /// (i.e. the generation of a SIGFPE signel) according to the 
    /// specified mask, for the duration of lifetime of the guard.
    /// The destructor will restore the state at the time of the 
    /// creation of the guard.
    /// 
    /// Note: to create a (valid) mask, use FPE::Guard::mask
    Guard(mask_type mask, bool disable=false)
    : m_initial( disable ? FPE::detail::disable(mask) : FPE::detail::enable(mask) )
    { }

    ~Guard() 
    { 
       //TODO: in disable mode, report which FPE happened between c'tor and d'tor.
       FPE::detail::disable( ~m_initial );
       mask_type mask = FPE::detail::enable( m_initial );
       if (mask!=m_initial) { throw GaudiException("oops -- FPEGuard failed to restore initial state","",StatusCode::FAILURE); }
    }

    /// 'mask' will convert a range of strings (i.e. Iter::value_type 
    /// must be of type std::string) into the corresponding mask to be used
    /// to enable (or disable) FPE trapping by the FPEGuard.
    /// Valid strings are typically "Inexact","DivByZero","Underflow","OverFlow","Invalid"
    /// and the catch-all "AllExcept".
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
private:
    mask_type m_initial;
};

} // namespace FPE
#endif
