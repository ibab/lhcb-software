// $Id: CaloLCorrection.h,v 1.1 2002-06-14 17:46:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOEX_CALOLCorrection_H 
#define CALOEX_CALOLCorrection_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"

/** @class CaloLCorrection CaloLCorrection.h
 *
 *  Logitudinal correction.
 *  The actual "code" is imported from Olivier Deschamps code 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */

class CaloLCorrection: 
  public virtual ICaloHypoTool , 
  public              CaloTool  
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloLCorrection>;
  
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
   *  @param parent  pointer to parent object (algorithm, service or tool)
   */
  CaloLCorrection
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloLCorrection() ;
  
private:

  ///  default  constructor   is   private
  CaloLCorrection();
  ///  copy     constructor   is   private
  CaloLCorrection( const CaloLCorrection& );
  ///  assignement operator   is   private
  CaloLCorrection& operator=( const CaloLCorrection& );
  
private:
  
  // critical energy 
  double  m_ec                                ;
  // radiation length (mm/X0)
  double  m_x0                                ;
  // preshower depth in X0 
  double  m_tprs                              ;
  // shower barycenter wrt maximum 
  double  m_tbar                              ;
  // allowed hypos
  typedef std::vector<int> Hypotheses         ;
  Hypotheses               m_hypos            ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOLCorrection_H
// ============================================================================
