
#ifndef RICHRECKERNEL_RICHHASH_H
#define RICHRECKERNEL_RICHHASH_H 1

// Include files
#include <string>

/** @class RichHash RichHash.h RichRecKernel/RichHash.h
 *  
 * 
 *  @author Chris Jones Christopher.Rob.Jones@cern.ch (original Vanya Belyaev)
 *  @date   19/11/2002
 */

template <class TYPE> 
struct RichHash : public std::unary_function<TYPE,size_t>
{
  size_t operator() ( const TYPE& key ) const 
  {
    size_t res     = 0 ;
    size_t len     = sizeof(TYPE) ;
    const  char* p = reinterpret_cast<const char*>( &key );
    while( len-- ) { res = ( res << 1 ) ^ *p++ ; }  
    return res;
  };
};

template <>
struct RichHash<std::string>
  : public std::unary_function<std::string,size_t>
{
  size_t operator() ( const std::string& key ) const
  {
    typedef std::string::const_iterator CI;
    size_t res  = 0 ;
    CI end = key.end   () ;
    for( CI p = key.begin(); end != p ; ++p ) { res = ( res << 1 )^*p ;}
    return res ;
  };
};

#endif // RICHRECKERNEL_RICHHASH_H
