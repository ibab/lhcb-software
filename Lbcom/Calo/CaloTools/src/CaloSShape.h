// $Id: CaloSShape.h,v 1.1 2002-04-07 18:15:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
public:
  
  /// containter of correction tools
  typedef  std::vector<ICaloCorrection*> Corrections;
  
public:
  
  /** Standard constructor
   *  @param  type actual type of the tool (what is it?)
   *  @param  name the name of the instance
   *  @parent pointer to parent object 
   */
  CaloSShape( const std::string& type   , 
              const std::string& name   ,
              const IInterface*  parent );
  
  /** destructor (virtual) 
   */
  virtual ~CaloSShape(); 
  
  /** initialization of the tool 
   *  @return status code 
   */
  virtual StatusCode initialize() ;
  
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
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const ;
  
private:
  
  Corrections m_tools;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOSSHAPE_H
// ============================================================================
