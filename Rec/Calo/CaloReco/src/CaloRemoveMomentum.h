// $Id: CaloRemoveMomentum.h,v 1.1.1.1 2002-11-13 20:46:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/04 14:46:16  ibelyaev
//  add new tools and options for recalibration of Ecal
//
// Revision 1.2  2002/07/22 08:19:43  ibelyaev
//  fix trivial bug
//
// Revision 1.1  2002/07/21 18:56:18  ibelyaev
//  add new energy correction
//
// Revision 1.1  2002/06/21 11:02:47  ibelyaev
//  update in S-,L- and E-corrections
//
// ============================================================================
#ifndef CALOEX_CALORemoveMomentum_H 
#define CALOEX_CALORemoveMomentum_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"

/** @class CaloRemoveMomentum CaloRemoveMomentum.h
 *
 *  The simplest energy correction for removal 
 *  CaloMomentum object form CaloHypo object  (needed for recalibration)
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */

class CaloRemoveMomentum: 
  public virtual ICaloHypoTool , 
  public              CaloTool  
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloRemoveMomentum>;
  
  typedef std::vector<double>  Params            ;
  
public:
  
  /** The main processing method
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const ;
  
  /** standard initialization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard finalization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service xor tool)
   */
  CaloRemoveMomentum
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloRemoveMomentum() ;
  
private:

  ///  default  constructor   is   private
  CaloRemoveMomentum();
  ///  copy     constructor   is   private
  CaloRemoveMomentum( const CaloRemoveMomentum& );
  ///  assignement operator   is   private
  CaloRemoveMomentum& operator=( const CaloRemoveMomentum& );
  
private:
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALORemoveMomentum_H
// ============================================================================
