// $Id: GiGaHash.h,v 1.1 2002-05-04 20:20:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGAHASH_H 
#define GIGA_GIGAHASH_H 1
// Include files
#include <string>

/** @class GiGaHash CaloHash.h Kernel/CaloHash.h
 *  
 *  Hashing function to be used in a conjunction with 
 *  GiGaMap and GiGaHashMap classes.
 *  The function is from B.Straustrup book.
 * 
 *  The code actually is imported from LHCb Calo software.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */

template <class TYPE> 
struct GiGaHash : public std::unary_function<TYPE,size_t>
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
struct GiGaHash<std::string>
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

#ifndef WIN32
template<>
struct std::hash<std::string>
  : public GiGaHash<std::string>{};
#endif 

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAHASH_H
// ============================================================================
