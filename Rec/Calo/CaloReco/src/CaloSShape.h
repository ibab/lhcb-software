// $Id: CaloSShape.h,v 1.4 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOSSHAPE_H 
#define CALOSSHAPE_H 1
// Include files
// STD & STL
#include <string>
#include <vector>
// CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloInterfaces/ICaloCorrection.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"

/** @class CaloSShape CaloSShape.h
 *  
 *  The concrete tool for implementation of 
 *  the famouse "S-Shape" correction for 
 *  electromagnetic showers. It is just 
 *  the reimplementation of F.Machefert's "Sshape"-tool.  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   21/03/2002
 */
class CaloSShape : 
  public virtual ICaloHypoTool ,
  public              CaloTool 
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSShape>;
public:
  
  /// containter of correction tools
  typedef  std::vector<ICaloCorrection*> Corrections ;
  /// containter of names
  typedef  std::vector<std::string>      Names       ; 
  
  /** initialization of the tool
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** finalization of the tool
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
  /** The main processing method
   * 
   *  Error codes 
   * 
   *  - 200 : CaloHypo* points to NULL
   *  - 201 : Invalid hypothesis 
   *  - 202 : Wrong number of clusters 
   *  - 203 : Cluster is invalid 
   *  - 204 : No 'seed' cell is found
   *  - 205 : 'Seed' is invalid 
   *  - 206 : Wrong index for calorimeter area 
   *  - 207 : Position was not cretaed by "correction" tool!
   * 
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
  
protected:
  
  /** Standard constructor
   *  @param  type actual type of the tool (what is it?)
   *  @param  name the name of the instance
   *  @param  parent pointer to parent object 
   */
  CaloSShape
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected) 
  virtual ~CaloSShape(); 
  
private:
  
  /// default constructor  is private
  CaloSShape();
  /// copy constructor     is private 
  CaloSShape
  ( const CaloSShape& );
  /// assignement operator is pprivate 
  CaloSShape& operator=
  ( const CaloSShape& );
  
private:
  
  /// vector of corrections 
  Names       m_areaCorrectionsTypeNames ;
  Corrections m_areaCorrections          ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOSSHAPE_H
// ============================================================================
