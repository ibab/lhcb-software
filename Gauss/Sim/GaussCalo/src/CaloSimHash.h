// $Id: CaloSimHash.h,v 1.2 2002-12-13 16:52:57 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
template<>
struct std::hash<CaloSim::Path>       : public GiGaHash<CaloSim::Path>  {} ;
template<>
struct std::hash<const CaloSim::Path> : public std::hash<CaloSim::Path> {} ;
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
template<>
struct std::hash<CaloCellID>       : public GiGaHash<CaloCellID>  {} ;
template<>
struct std::hash<const CaloCellID> : public std::hash<CaloCellID> {} ;
#endif


// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM CALOSIMHASH_H
// ============================================================================
