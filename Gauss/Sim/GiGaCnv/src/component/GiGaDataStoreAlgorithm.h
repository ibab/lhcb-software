// $Id: GiGaDataStoreAlgorithm.h,v 1.2 2002-12-07 14:36:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:43  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
// ============================================================================
#ifndef GIGACNV_GiGaDataStoreALGORITHM_H 
#define GIGACNV_GiGaDataStoreALGORITHM_H 1
// Include files
// from STD & STL
#include <string>
#include <vector>
// from GaudiKernel
#include "GaudiKernel/Algorithm.h"
/// forward declaration
template <class ALGORITHM> 
class AlgFactory   ;              ///< from GaudiKernel
class IGiGaCnvSvc  ;              ///< from GiGaCnv


/** @class GiGaDataStoreAlgorithm GiGaDataStoreAlgorithm.h
 *  
 *  Simple algorithm to trigger the registration of 
 *  IOpaqueAddresses gor GiGa objects in Gaudi Data Store  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   19/01/2002
 */

class GiGaDataStoreAlgorithm : public Algorithm 
{
  
  /// friend factory for instantiation 
  friend class AlgFactory<GiGaDataStoreAlgorithm>;
  
  typedef std::vector<std::string>     Names    ;
  typedef std::vector<IGiGaCnvSvc*>    Services ;
  
protected: 
  
  /** Standard constructor
   *  @param Name algorithm name 
   *  @param SvcLoc pointer to Service Locator
   */
  GiGaDataStoreAlgorithm( const std::string& Name   , 
                        ISvcLocator*       SvcLoc );
  
  /// destructor
  virtual ~GiGaDataStoreAlgorithm();
  
public:
  
  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode initialize();    
  
  /** standard execution method 
   *  @return status code 
   */
  virtual StatusCode execute   ();    
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode finalize  ();    
  
private:
  
  /** default constructor is private!
   */
  GiGaDataStoreAlgorithm() ;
  
  /** copy constructor is private!
   *  @param copy 
   */
  GiGaDataStoreAlgorithm
  ( const GiGaDataStoreAlgorithm& copy );
  
  /** assignement is private!
   *  @param copy 
   */
  GiGaDataStoreAlgorithm& operator=
  ( const GiGaDataStoreAlgorithm& copy );
  
private:
  
  /// names of giga conversion services  
  Names              m_names    ;
  /// giga conversion services 
  Services           m_services ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // GIGACNV_GiGaDataStoreALGORITHM_H
// ============================================================================
