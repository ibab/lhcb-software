// $Id: CaloSelectorAND.h,v 1.1 2002-04-27 19:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTOOLS_CALOSELECTORAND_H 
#define CALOTOOLS_CALOSELECTORAND_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// From CaloInterfaces
#include "CaloInterfaces/ICaloClusterSelector.h"

/** @class CaloSelectorAND CaloSelectorAND.h
 *   
 *  Helper concrete tool for selection of calocluster objects
 *  This selector selects the cluster if 
 *  all of its daughter selector select it!
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/04/2002
 */
class CaloSelectorAND : 
  public virtual ICaloClusterSelector ,
  public          CaloTool 
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectorAND>;
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
  CaloSelectorAND
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /// destructor (virtual and protected)
  virtual ~CaloSelectorAND() ; 
  
private:

  ///   default  constructor  is  private 
  CaloSelectorAND();
  ///   copy     constructor  is  private 
  CaloSelectorAND
  (const CaloSelectorAND& );
  ///   assignement operator  is  private 
  CaloSelectorAND& operator=
  (const CaloSelectorAND& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTOOLS_CALOSELECTORAND_H
// ============================================================================
