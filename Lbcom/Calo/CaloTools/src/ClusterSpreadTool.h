// $Id: ClusterSpreadTool.h,v 1.3 2001-12-09 14:33:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/25 15:50:40  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.1  2001/11/23 11:44:51  ibelyaev
//  new tool for calculation of cluster spread
// 
// ============================================================================
#ifndef CALOTOOLS_CLUSTERSPREADTOOL_H 
#define CALOTOOLS_CLUSTERSPREADTOOL_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/Property.h"
// CaloInterfaces
#include  "CaloInterfaces/ICaloClusterTool.h"
// CaloKernel
#include  "CaloKernel/CaloTool.h"
// CaloUtil 
#include  "CaloUtils/SpreadEstimator.h"

/** @class ClusterSpreadTool ClusterSpreadTool.h
 *  
 *  Concrete tool for estimation of the 
 *  effective cluster size ("spread")
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/11/2001
 */

class ClusterSpreadTool:
  public virtual ICaloClusterTool ,
  public                 CaloTool 
{
  /// friend class for instantiation
  friend class ToolFactory<ClusterSpreadTool>;
  
public:
  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode initialize ();
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode finalize   ();
  
  /** query interface method  
   *  @param  iiD  unique interface identifier 
   *  @param  pI   placeholder for interface 
   *  @return status code 
   */
  virtual StatusCode queryInterface 
  ( const InterfaceID& iiD ,
    void**             pI  );
  
  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloCluster* cluster ) const ;
  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const ;
  
  /** The main processing method with hypothesis 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @param hypo    processing hypothesis 
   *  @return status code 
   */  
  virtual StatusCode process    
  ( CaloCluster* cluster                   , 
    const CaloHypotheses::Hypothesis& hypo ) const ;
  
protected:
  
  /** Standard constructor
   *  @param type tool type (useless)
   *  @param name tool name
   *  @param parent pointer to parent object (service, algorithm or tool)  
   */
  ClusterSpreadTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
  /** destructor, virtual and protected 
   */
  virtual ~ClusterSpreadTool();
  
private:
  
  SpreadEstimator    m_estimator ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTOOLS_CLUSTERSPREADTOOL_H
// ============================================================================
