// $Id: ClusterSpreadTool.h,v 1.6 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2009/05/10 15:20:36  ibelyaev
//  improve the numerical stability & add more statistical printout
//
//
// ============================================================================
#ifndef CALORECO_CLUSTERSPREADTOOL_H 
#define CALORECO_CLUSTERSPREADTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Property.h"
// ============================================================================
// CaloInterfaces
// ============================================================================
#include  "CaloInterfaces/ICaloClusterTool.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include  "GaudiAlg/GaudiTool.h"
// ============================================================================
// CaloUtil 
// ============================================================================
#include  "CaloUtils/SpreadEstimator.h"
// ============================================================================
/** @class ClusterSpreadTool ClusterSpreadTool.h
 *  
 *  Concrete tool for estimation of the 
 *  effective cluster size ("spread")
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/11/2001
 */
// ============================================================================
class ClusterSpreadTool:
  public virtual ICaloClusterTool ,
  public                 GaudiTool 
{
  // ==========================================================================
  /// friend class for instantiation
  friend struct ToolFactory<ClusterSpreadTool>;
  // ==========================================================================
public:
  // ==========================================================================  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode initialize ();
  // ==========================================================================  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode finalize   ();
  // ==========================================================================  
  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloCluster* cluster ) const ;
  // ==========================================================================  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloCluster* cluster ) const ;
  // ==========================================================================  
protected:
  // ==========================================================================  
  /** Standard constructor
   *  @param type tool type (useless)
   *  @param name tool name
   *  @param parent pointer to parent object (service, algorithm or tool)  
   */
  ClusterSpreadTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  // ==========================================================================  
  /// destructor, virtual and protected 
  virtual ~ClusterSpreadTool();
  // ==========================================================================    
private:
  // ==========================================================================
  /// default constructor   is private
  ClusterSpreadTool();
  /// copy    constructor   is private
  ClusterSpreadTool
  ( const ClusterSpreadTool& );
  /// assignement operator  is private
  ClusterSpreadTool& operator=
  ( const ClusterSpreadTool& );
  // ==========================================================================
private:
  // ==========================================================================  
  SpreadEstimator    m_estimator ;
  std::string m_detData;
  const DeCalorimeter* m_det;
  // ==========================================================================
};
// ============================================================================
// The End 
// ============================================================================
#endif // CALORECO_CLUSTERSPREADTOOL_H
// ============================================================================
