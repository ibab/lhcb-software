// $Id: CaloClusterToolSequence.h,v 1.1 2002-04-27 19:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTOOLS_CALOCLUSTERTOOLSEQUENCE_H 
#define CALOTOOLS_CALOCLUSTERTOOLSEQUENCE_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"

/** @class CaloClusterToolSequence CaloClusterToolSequence.h
 *  
 *  Helper concrete tool for CaloCluster processing
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */
class CaloClusterToolSequence : 
  public virtual ICaloClusterTool ,
  public                 CaloTool 
{  
  /// friend factory for instantiation
  friend class ToolFactory<CaloClusterToolSequence>;
public:

  /// container of types&names 
  typedef std::vector<std::string>           Names        ;
  /// container of selectors 
  typedef std::vector<ICaloClusterTool*>     ClusterTools ;

public:

  /** The main processing method 
   *  @see ICaloClusterTool
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloCluster* cluster ) const ;
  
  /** The main processing method (functor interface) 
   *  @see ICaloClusterTool
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const ;

  /** standard initialization of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see CaloTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** standard finalization  of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see CaloTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;

protected:

  /** Standard constructor
   *  @see CaloTool
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent   
   */ 
  CaloClusterToolSequence 
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// desctructor (virtual and protected)
  virtual ~CaloClusterToolSequence();

private:
  
  ///  default  constructor  is  private 
  CaloClusterToolSequence () ;
  ///  copy     constructor  is  private
  CaloClusterToolSequence 
  (const CaloClusterToolSequence& );
  ///  assignement operator  is  private
  CaloClusterToolSequence& operator= 
  (const CaloClusterToolSequence& );
  
private:
  
  Names          m_clusterToolsTypeNames ;
  ClusterTools   m_clusterTools          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTOOLS_CALOCLUSTERTOOLSEQUENCE_H
// ============================================================================
