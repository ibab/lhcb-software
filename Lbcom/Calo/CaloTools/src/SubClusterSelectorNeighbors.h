// $Id: SubClusterSelectorNeighbors.h,v 1.1 2001-11-12 19:04:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTOOLS_SUBCLUSTERSELECTORNEIGHBORS_H 
#define CALOTOOLS_SUBCLUSTERSELECTORNEIGHBORS_H 1
// Include files
// CaloTools 
#include "SubClusterSelectorBase.h"

/** @class SubClusterSelectorNeighbors SubClusterSelectorNeighbors.h
 *  
 *  Very simple concrete "sub-cluster selector".
 *  It is just tag/select "neighbor" digits around
 *  the seed cell. Definition of "neighbor" is 
 *  form DeCalorimeter class.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   12/11/2001
 */

class SubClusterSelectorNeighbors 
  : public SubClusterSelectorBase
{
  /// friend factory for instantiation
  friend class ToolFactory<SubClusterSelectorNeighbors>;
  
public:
  
  /** The main processing method (functor interface) 
   *
   *  Error codes:  
   *    - 225   cluster points to NULL
   *    - 226   empty cell/digit container for given cluster 
   *    - 227   SeedCell is not found 
   *    - 228   Seed points to NULL
   *
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const ;
  
protected:
  
  /** Standard Tool Constructor
   *  @param Type type of the tool (useless ? )
   *  @param Name name of the tool 
   *  @param Parent the tool parent 
   */
  SubClusterSelectorNeighbors( const std::string& Type   ,
                               const std::string& Name   ,
                               const IIntrerface* Parent ); 
  
  /** destructor, virtual and protected 
   */
  virtual ~SubClusterSelectorNeighbors( ); ///< Destructor
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORNEIGHBORS_H
// ============================================================================
