// $Id: CaloClusterisationAlg.h,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/04 20:25:16  ibelyaev
//  update for new Calo Event Model (v10r0)
//
// Revision 1.2  2001/11/25 16:25:31  ibelyaev
//  update for newer CaloKernel package
//
// ===========================================================================
#ifndef CALOCLUSTERISATIONALG_H 
#define CALOCLUSTERISATIONALG_H 1
// ===========================================================================
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloClusterisationAlg CaloClusterisationAlg.h 
 *  
 *  Simple 'a la FORTRAN' clusterisation Algorithm
 *
 *     - (1) locate local maxima  - "seeds"
 *     - (2) construct "clusters" - "seed"+"neighbours"
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   06/07/2001
 */

class CaloClusterisationAlg : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend AlgFactory<CaloClusterisationAlg>;
  
public:
  
  /** standard  Algorithm initialization
   *  @see CaloAlgoritnm
   *  @see     Algoritnm
   *  @see    IAlgoritnm
   *  @return status code 
   */
  virtual  StatusCode initialize ();
  
  /** standard  Algorithm execution
   *  @see CaloAlgoritnm
   *  @see     Algoritnm
   *  @see    IAlgoritnm
   *  @return status code 
   */
  virtual  StatusCode execute    ();  
  
  /** standard  Algorithm finalization
   *  @see CaloAlgoritnm
   *  @see     Algoritnm
   *  @see    IAlgoritnm
   *  @return status code 
   */
  virtual  StatusCode finalize   ();
  
protected:
  
  /** standard constructor
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @param  name    name of this concrete algorithm instance 
   *  @param  svc     pointer to Service Locator 
   */
  CaloClusterisationAlg
  ( const std::string& name , 
    ISvcLocator*       svc  );
  
  /// Destructor (virtual and protected)
  virtual ~CaloClusterisationAlg();

private:
  
  /// default  constructor is private   
  CaloClusterisationAlg();
  /// copy     constructor is private   
  CaloClusterisationAlg
  ( const CaloClusterisationAlg& );
  /// assignement operator is private   
  CaloClusterisationAlg& operator=
  ( const CaloClusterisationAlg& );

private:

  bool    m_useSeedECut  ;
  bool    m_useSeedEtCut ;
  double  m_threshold    ;

};

// ===========================================================================
// The End
// ===========================================================================
#endif ///< CALOCLUSTERISATIONALG_H
// ===========================================================================
