// $Id: CaloSimHash.h,v 1.3 2003-07-10 11:27:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2.2.1  2003/07/07 16:54:41  ranjard
// v3r1 is a branch on v3r0 - fix for gcc3.2
//
// Revision 1.2  2002/12/13 16:52:57  ibelyaev
//  put updated versions of the packages
//
// Revision 1.1  2002/12/07 14:41:44  ibelyaev
//  add new Calo stuff
//
// ============================================================================
#ifndef CALOSIM_CALOSIMHASH_H 
#define CALOSIM_CALOSIMHASH_H 1
// Include files
#include "GiGa/GiGaHashMap.h"
// Kernel
#include "Kernel/CaloCellID.h"
// local 
#include "CaloSim.h"

/** @class CaloSimHash CaloSimHash.h
 *  
 *  Simple hashing functions 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru  
 *  @author Alexei Berdiouguine
 *  @date   2002-07-09
 */

template <>
struct GiGaHash<CaloSim::Path> : 
  public std::unary_function<const CaloSim::Path,size_t>
{
  /** the only one essential method : actual "hash"
   *  @param  cell id to be hashed 
   *  @return hashed id 
   */
  size_t operator() ( const CaloSim::Path& cell ) const 
  {
    size_t res     = 0 ;
    for( CaloSim::Path::const_iterator i = cell.begin() ; 
         cell.end() != i ; ++i ) { res = ( res << 1 ) ^ (*i) ; }
    
    return res ;
  };
};

template <>
struct GiGaHash<const CaloSim::Path> : public GiGaHash<CaloSim::Path>   {} ;

#ifndef WIN32
#if defined (__GNUC__) && ( __GNUC__ > 2 )
namespace __gnu_cxx {
#endif
template<>
struct hash<CaloSim::Path>       : public GiGaHash<CaloSim::Path>  {} ;
template<>
struct hash<const CaloSim::Path> : public hash<CaloSim::Path> {} ;
#if defined (__GNUC__) && ( __GNUC__ > 2 )
}
#endif
#endif

template <>
struct GiGaHash<CaloCellID> : 
  public std::unary_function<const CaloCellID,size_t>
{
  /** the only one essential method : actual "hash"
   *  @param  cell id to be hashed 
   *  @return hashed id 
   */
  size_t operator() ( const CaloCellID& cell ) const 
  { return cell.index() ; }
};

template <>
struct GiGaHash<const CaloCellID>  : public GiGaHash<CaloCellID>  {} ;

#ifndef WIN32
#if defined (__GNUC__) && ( __GNUC__ > 2 )
namespace __gnu_cxx {
#endif
template<>
struct hash<CaloCellID>       : public GiGaHash<CaloCellID>  {} ;
template<>
struct hash<const CaloCellID> : public hash<CaloCellID> {} ;
#if defined (__GNUC__) && ( __GNUC__ > 2 )
}
#endif
#endif


// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM CALOSIMHASH_H
// ============================================================================
