// $Id: CaloSimHash.h,v 1.7 2006-06-24 16:23:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOSIM_CALOSIMHASH_H 
#define CALOSIM_CALOSIMHASH_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/Hash.h"
#include "GaudiKernel/HashMap.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/CaloCellID.h"
// ============================================================================
// local 
// ============================================================================
#include "CaloSim.h"
// ============================================================================

namespace GaudiUtils 
{
  /** @class CaloSimHash CaloSimHash.h
   *  
   *  Simple hashing functions 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru  
   *  @author Alexei Berdiouguine
   *  @date   2002-07-09
   */
  template <>
  struct Hash<CaloSim::Path::value_type> : 
    public std::unary_function<const CaloSim::Path::value_type,size_t>
  {
    /** the only one essential method : actual "hash"
     *  @attention reinterpret_cast in the game!
     *  @param  id physical volume  
     *  @return hashed id 
     */
    size_t operator() ( const CaloSim::Path::value_type& id ) const 
    { return reinterpret_cast<long>( id ) ; } ;
  };
  
  template <>
  struct Hash<CaloSim::Path> : 
    public std::unary_function<const CaloSim::Path,size_t>
  {
    /** the only one essential method : actual "hash"
     *  @param  cell id to be hashed 
     *  @return hashed id 
     */
    inline size_t operator() ( const CaloSim::Path& cell ) const 
    {
      size_t res     = 0 ;
      for ( CaloSim::Path::const_iterator i = cell.begin() ; cell.end() != i ; ++i ) 
      { res = ( res << 4 ) ^ m_hash(*i) ; }
      return res ;
    };
  private:
    GaudiUtils::Hash<CaloSim::Path::value_type> m_hash ;
  };
  
  template <>
  struct Hash<LHCb::CaloCellID> : 
    public std::unary_function<const LHCb::CaloCellID,size_t>
  {
    /** the only one essential method : actual "hash"
     *  @param  cell id to be hashed 
     *  @return hashed id 
     */
    inline size_t operator() ( const LHCb::CaloCellID& cell ) const 
    { return cell.index() ; }
  } ;
  
}


// ============================================================================
// The END 
// ============================================================================
#endif // CALOSIM CALOSIMHASH_H
// ============================================================================
