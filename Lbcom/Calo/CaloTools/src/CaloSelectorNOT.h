// $Id: CaloSelectorNOT.h,v 1.1 2002-04-27 19:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTOOLS_CALOSELECTORNOT_H 
#define CALOTOOLS_CALOSELECTORNOT_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// From CaloInterfaces
#include "CaloInterfaces/ICaloClusterSelector.h"

/** @class CaloSelectorNOT CaloSelectorNOT.h
 *   
 *  Helper concrete tool for selection of calocluster objects
 *  This selector selects the cluster if 
 *  none  of its daughter selector select it!
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */
class CaloSelectorNOT : 
  public virtual ICaloClusterSelector ,
  public          CaloTool 
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectorNOT>;
public:
  /// container of types&names 
  typedef std::vector<std::string>           Names     ;
  /// container of selectors 
  typedef std::vector<ICaloClusterSelector*> Selectors ;

public:

  /** "select"/"preselect" method 
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const CaloCluster* cluster ) const  ;

  /** "select"/"preselect" method (functor interface)
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator () 
    ( const CaloCluster* cluster ) const  ;
  
  /** stNOTard initialization of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see CaloTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** stNOTard finalization  of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see CaloTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** StNOTard constructor
   *  @see CaloTool
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent   
   */
  CaloSelectorNOT
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /// destructor (virtual NOT protected)
  virtual ~CaloSelectorNOT() ; 
  
private:

  ///   default  constructor  is  private 
  CaloSelectorNOT();
  ///   copy     constructor  is  private 
  CaloSelectorNOT
  (const CaloSelectorNOT& );
  ///   assignement operator  is  private 
  CaloSelectorNOT& operator=
  (const CaloSelectorNOT& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTOOLS_CALOSELECTORNOT_H
// ============================================================================
