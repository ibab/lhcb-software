// $Id: CaloDeleteObjectsAlg.h,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/04 14:41:34  ibelyaev
//  add new algorithms for recalibration of Ecal
//
// Revision 1.3  2002/06/15 12:53:13  ibelyaev
//  version update
//
// ============================================================================
#ifndef CALOEX_CALODeleteObjectsAlg_H 
#define CALOEX_CALODeleteObjectsAlg_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloDeleteObjectsAlg CaloDeleteObjectsAlg.h
 *  
 *  Simple algorithm for deletion of DataObjects
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloDeleteObjectsAlg : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloDeleteObjectsAlg>;

public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloDeleteObjectsAlg( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloDeleteObjectsAlg();
  
private:

  /// default  construstor  is  private 
  CaloDeleteObjectsAlg(); 
  /// copy     construstor  is  private 
  CaloDeleteObjectsAlg
  ( const CaloDeleteObjectsAlg& );
  /// assignement operator  is  private 
  CaloDeleteObjectsAlg& operator=
  ( const CaloDeleteObjectsAlg& );
  
private:
  
  typedef std::vector<std::string>  Addresses ;
  Addresses m_addresses ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALODeleteObjectsAlg_H
// ============================================================================
