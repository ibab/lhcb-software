// $Id: GiGaDataStoreAlgorithm.h,v 1.3 2004-02-20 19:12:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGACNV_GiGaDataStoreALGORITHM_H 
#define GIGACNV_GiGaDataStoreALGORITHM_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
/// forward declaration
// ============================================================================
template <class ALGORITHM> 
class AlgFactory   ;              ///< from GaudiKernel
class IGiGaCnvSvc  ;              ///< from GiGaCnv
// ============================================================================


/** @class GiGaDataStoreAlgorithm GiGaDataStoreAlgorithm.h
 *  
 *  Simple algorithm to trigger the registration of 
 *  IOpaqueAddresses gor GiGa objects in Gaudi Data Store  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   19/01/2002
 */

class GiGaDataStoreAlgorithm : public GaudiAlgorithm 
{
  
  /// friend factory for instantiation 
  friend class AlgFactory<GiGaDataStoreAlgorithm>;
public:
  
  typedef std::vector<std::string>     Names    ;
  typedef std::vector<IGiGaCnvSvc*>    Services ;
  
protected: 
  
  /** Standard constructor
   *  @param Name algorithm name 
   *  @param SvcLoc pointer to Service Locator
   */
  GiGaDataStoreAlgorithm 
  ( const std::string& Name   , 
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
