// ============================================================================
#ifndef EVENT_CLUSTERCELLID_H 
#define EVENT_CLUSTERCELLID_H 1
// ============================================================================
/// include files 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloDataFunctor.h"

// ============================================================================
/** @file
 *  The template specializations of CaloDataFunctor::CellID<TYPE>
 *  structure for class CaloCluster , class CaloHypo and class CaloParticle
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date 2004-10-22
 *
 * modif : 2005-11-04 O. Deschamps
 * Adapt to new Model : removing CaloParticle
 */
// ============================================================================

// ============================================================================
namespace LHCb
{
  
namespace CaloDataFunctor 
{
  
  // ==========================================================================  
  /** The specialisation of general CellID structure for class CaloCluster 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-10-22
   */
  template <>
  class CellID<LHCb::CaloCluster>
    : public std::unary_function<const LHCb::CaloCluster,LHCb::CaloCellID>
  {
  public:
    LHCb::CaloCellID operator() ( const LHCb::CaloCluster& cluster ) const 
    { return cluster.seed() ; }
  };
  // ==========================================================================
  
  // ==========================================================================  
  /** The specialisation of general CellID structure for class CaloCluster 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-10-22
   */
  template <>
  class CellID<LHCb::CaloHypo>
    : public std::unary_function<const LHCb::CaloHypo,LHCb::CaloCellID>
  {
  public:
    LHCb::CaloCellID operator() ( const LHCb::CaloHypo& hypo ) const 
    {
      if ( hypo.clusters().empty() ) { return LHCb::CaloCellID() ; }
      const LHCb::CaloCluster* cluster = *(hypo.clusters().begin()) ;
      return m_cell( cluster ) ;
    };
  private: 
    CellID<const LHCb::CaloCluster*> m_cell ;
  };
} // end of namespace CaloDataFunctor 
} // end of namespace LHCb
// ============================================================================
#endif // EVENT_CLUSTERCELLID_H
