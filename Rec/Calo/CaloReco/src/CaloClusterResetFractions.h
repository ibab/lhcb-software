// $Id: CaloClusterResetFractions.h,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/04/07 18:14:59  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.3  2001/12/09 14:33:08  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.2  2001/11/25 15:50:40  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.1  2001/11/08 20:07:04  ibelyaev
//  new tools are added into  the package
// 
// ============================================================================
#ifndef CALOTOOLS_CALOCLUSTERRESETFRACTIONS_H 
#define CALOTOOLS_CALOCLUSTERRESETFRACTIONS_H 1
// Include files
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"

/** @class CaloClusterResetFractions CaloClusterResetFractions.h
 *  
 *  This concrete Tool reset energy fractions and weigths,
 *  which are set by any energy shareing procedure, thus
 *  it is "undo" for energy-shareing procedure 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/11/2001
 */

class CaloClusterResetFractions : 
  public virtual ICaloClusterTool ,
  public                 CaloTool
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloClusterResetFractions>;
  
public:
  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode    initialize ();
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
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
  
protected:

  /** Standard constructor
   *  @param type   tool type 
   *  @param name   tool name 
   *  @param parent tool parent 
   */
  CaloClusterResetFractions
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor, protected and virtual
  virtual ~CaloClusterResetFractions();
  
private:
  
  /// default constructor is private 
  CaloClusterResetFractions() ;
  /// copy    constructor is private 
  CaloClusterResetFractions
  ( const CaloClusterResetFractions& );
  /// sssignement opereator s private 
  CaloClusterResetFractions& operator=
  ( const CaloClusterResetFractions& );
  
private:
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOCLUSTERRESETFRACTIONS_H
// ============================================================================
