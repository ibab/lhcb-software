// $Id: CaloSimHash.h,v 1.6 2006-01-17 15:52:57 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2004/10/08 15:06:54  ibelyaev
//  fix a 'feature'
//
// ============================================================================
#ifndef CALOSIM_CALOSIMHASH_H 
#define CALOSIM_CALOSIMHASH_H 1
// Include files
#include "GiGa/GiGaHashMap.h"
#include "GiGa/GiGaHash.h"
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
struct GiGaHash<CaloSim::Path::value_type> : 
  public std::unary_function<const CaloSim::Path::value_type,size_t>
{
  /** the only one essential method : actual "hash"
   *  @param  cell id to be hashed 
   *  @return hashed id 
   */
  size_t operator() ( const CaloSim::Path::value_type& id ) const 
  { return reinterpret_cast<long>( id ) ; } ;
};

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
    for ( CaloSim::Path::const_iterator i = cell.begin() ; cell.end() != i ; ++i ) 
    { res = ( res << 4 ) ^ m_hash(*i) ; }
    return res ;
  };
private:
  GiGaHash<CaloSim::Path::value_type> m_hash ;
};

template <>
struct GiGaHash<LHCb::CaloCellID> : 
  public std::unary_function<const LHCb::CaloCellID,size_t>
{
  /** the only one essential method : actual "hash"
   *  @param  cell id to be hashed 
   *  @return hashed id 
   */
  size_t operator() ( const LHCb::CaloCellID& cell ) const 
  { return cell.index() ; }
};

namespace __gnu_cxx 
{
  template<>
  struct hash<CaloSim::Path>       : public GiGaHash<CaloSim::Path>  {} ;
  template<>
  struct hash<const CaloSim::Path> : public hash<CaloSim::Path>      {} ;
  template<>
  struct hash<LHCb::CaloCellID>          : public GiGaHash<LHCb::CaloCellID>     {} ;
  template<>
  struct hash<const LHCb::CaloCellID>    : public hash<LHCb::CaloCellID>         {} ;
  
#ifdef WIN32
  template <> 
  inline size_t hash_value ( const CaloSim::Path& cell ) 
  { hash<CaloSim::Path> () ( cell ) ; }
  template <> 
  inline size_t hash_value ( const LHCb::CaloCellID&    cell ) 
  { hash<LHCb::CaloCellID>    () ( cell ) ; }  
#endif 
  
};


// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM CALOSIMHASH_H
// ============================================================================
