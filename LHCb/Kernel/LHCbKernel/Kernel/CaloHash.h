// $Id: CaloHash.h,v 1.4 2004-07-27 14:01:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef KERNEL_CALOHASH_H 
#define KERNEL_CALOHASH_H 1
// Include files
#include <string>

/** @class CaloHash CaloHash.h Kernel/CaloHash.h
 *
 *  The simple hashing function 
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
    while( len-- ) { res = ( res << 1 ) ^ *p++ ; }  
    return res;
  };
};

template <class TYPE> 
struct CaloHash <const TYPE>  :  public CaloHash <TYPE> {} ;
template <class TYPE> 
struct CaloHash <const TYPE&> :  public CaloHash <TYPE> {} ;
template <class TYPE> 
struct CaloHash       <TYPE&> :  public CaloHash <TYPE> {} ;

template <class TYPE> 
struct CaloHash<const TYPE*> 
  : public std::unary_function<const TYPE*,size_t>
{
  size_t operator() ( const TYPE* key ) const 
  { return 0 == key ? 0 : m_type( *key ) ; };
private: 
  Hash<TYPE> m_type ;
};

template <class TYPE> 
struct CaloHash       <TYPE*>  :  public CaloHash <const TYPE*> {} ;
template <class TYPE> 
struct CaloHash       <TYPE*&> :  public CaloHash <const TYPE*> {} ;
template <class TYPE> 
struct CaloHash <const TYPE*&> :  public CaloHash <const TYPE*> {} ;

template <>
struct CaloHash<std::string>
  : public std::unary_function<const std::string,size_t>
{
  size_t operator() ( const std::string& key ) const
  {
    typedef std::string::const_iterator CI;
    size_t res  = 0 ;
    for( CI p = key.begin() ; key.end() != p ; ++p ) 
    { res = ( res << 1 )^(*p) ; }
    return res ;
  };
};

template <>
struct CaloHash<const std::string>  : public CaloHash<std::string> {} ;
template <>
struct CaloHash<std::string&>       : public CaloHash<std::string> {} ;
template <>
struct CaloHash<const std::string&> : public CaloHash<std::string> {} ;

struct CaloHash<char*>
  : public std::unary_function<char*,size_t>
{
  size_t operator() ( const char* key ) const
  {
    size_t res = 0 ;
    if    ( 0 == key ) { return 0 ; }
    while ( *key ) { res = ( res<<1)^*key++ ; }
    return res ;
  };
};

template <>
struct CaloHash<const char*> : public CaloHash<char*>        {} ;


// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_CALOHASH_H
// ============================================================================
