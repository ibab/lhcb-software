// $Id: CaloHash.h,v 1.2 2002-04-27 16:26:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/27 15:31:02  ibelyaev
//  add CaloHash.h for efficient hashing on Win32 platform
// 
// ============================================================================
#ifndef KERNEL_CALOHASH_H 
#define KERNEL_CALOHASH_H 1
// Include files
#include <string>

/** @class CaloHash CaloHash.h Kernel/CaloHash.h
 *  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */

template <class TYPE> 
struct CaloHash : public std::unary_function<TYPE,size_t>
{
  size_t operator() ( const TYPE& key ) const 
  {
    size_t res     = 0 ;
    size_t len     = sizeof(TYPE) ;
    const  char* p = reinterpret_cast<const char*>( &key );
    while( len-- ) { res = ( res << 1 ) ^ p++ ; }  
    return res;
  };
};

template <>
struct CaloHash<std::string>
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

// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_CALOHASH_H
// ============================================================================
