// $Id: CaloHypoToolSequence.h,v 1.1 2002-04-27 19:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOTOOLS_CALOHYPOTOOLSEQUENCE_H 
#define CALOTOOLS_CALOHYPOTOOLSEQUENCE_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"

/** @class CaloHypoToolSequence CaloHypoToolSequence.h
 *  
 *  Helper concrete tool for CaloHypo processing
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */
class CaloHypoToolSequence : 
  public virtual ICaloHypoTool ,
  public                 CaloTool 
{  
  /// friend factory for instantiation
  friend class ToolFactory<CaloHypoToolSequence>;
public:

  /// container of types&names 
  typedef std::vector<std::string>        Names     ;
  /// container of selectors 
  typedef std::vector<ICaloHypoTool*>     HypoTools ;

public:

  /** The main processing method 
   *  @see ICaloHypoTool
   *  @param Hypo pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* Hypo ) const ;
  
  /** The main processing method (functor interface) 
   *  @see ICaloHypoTool
   *  @param Hypo pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* Hypo ) const ;

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
  CaloHypoToolSequence 
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// desctructor (virtual and protected)
  virtual ~CaloHypoToolSequence();

private:
  
  ///  default  constructor  is  private 
  CaloHypoToolSequence () ;
  ///  copy     constructor  is  private
  CaloHypoToolSequence 
  (const CaloHypoToolSequence& );
  ///  assignement operator  is  private
  CaloHypoToolSequence& operator= 
  (const CaloHypoToolSequence& );
  
private:
  
  Names       m_hypoToolsTypeNames ;
  HypoTools   m_hypoTools          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTOOLS_CALOHYPOTOOLSEQUENCE_H
// ============================================================================
