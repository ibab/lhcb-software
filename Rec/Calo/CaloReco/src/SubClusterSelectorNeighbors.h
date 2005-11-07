// $Id: SubClusterSelectorNeighbors.h,v 1.2 2005-11-07 12:12:44 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// Revision 1.3  2002/04/07 18:15:04  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.2  2001/11/12 21:16:14  ibelyaev
//  new concrete sub-cluster selector is added
//
// Revision 1.1  2001/11/12 19:04:28  ibelyaev
//  update
// 
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
  
  /** standard initiliazation 
   *  @see SubClusterSelectorBase
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool   
   *  @return status code 
   */
  virtual StatusCode initialize() ;
  
  /** The main processing method 
   *  @see ICaloSubClusterTag
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode tag
  ( CaloCluster* cluster ) const ;

  /** The main processing method 
   *  @see ICaloSubClusterTag
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode untag
  ( CaloCluster* cluster ) const ;
  
protected:
  
  /** Standard Tool Constructor
   *  @param Type type of the tool (useless ? )
   *  @param Name name of the tool 
   *  @param Parent the tool parent 
   */
  SubClusterSelectorNeighbors( const std::string& Type   ,
                               const std::string& Name   ,
                               const IInterface*  Parent ); 
  
  /// destructor, virtual and protected 
  virtual ~SubClusterSelectorNeighbors( ); ///< Destructor

private:
  
  /// default  constructor isprivate 
  SubClusterSelectorNeighbors(); 
  /// copy     constructor isprivate 
  SubClusterSelectorNeighbors 
  ( const SubClusterSelectorNeighbors& );
  /// assignement operator isprivate 
  SubClusterSelectorNeighbors& operator=
  ( const SubClusterSelectorNeighbors& );
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // SUBCLUSTERSELECTORNEIGHBORS_H
// ============================================================================
