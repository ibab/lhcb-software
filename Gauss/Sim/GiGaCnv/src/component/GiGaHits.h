// $Id: GiGaHits.h,v 1.1 2002-12-07 14:36:26 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef      GIGA_GiGaHits_H 
#define      GIGA_GiGaHits_H  1 
// ============================================================================
/// STL 
#include <string> 
/// GiGaCnv 
#include "GiGaCnv/IGiGaHitsCnvSvc.h" 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
///
template <class SERVICE> 
class SvcFactory; 
///
class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 
class    IGiGaKineCnvSvc;

/** @class GiGaHits GiGaHits.h
 *    
 *  Convertersion service for convertiong Geant4 hits info Gaudi hits 
 *  
 *  @author  Vanya Belyaev
 *  @date    23/02/2001
 */

class GiGaHits:  
  virtual public IGiGaHitsCnvSvc  ,
  public          GiGaCnvSvcBase   
{
  ///
  friend class SvcFactory<GiGaHits>;
  ///
public: 

  /** initialize the service
   *  @return status code
   */ 
  virtual StatusCode            initialize() ;

  /** finalize the service
   *  @return status code
   */ 
  virtual StatusCode            finalize  () ; 
  
  /** retrieve the relation table between Geant4 track/trajectory 
   *  identifiers and the converted MCParticle objects 
   *  @return the reference to relation table  
   */
  virtual GiGaKineRefTable&      table    ()  ;
  
  /** query the interface
   *  @param ID unique interface identifier 
   *  @param II placeholder for interface 
   *  @return status code 
   */
  virtual StatusCode queryInterface( const InterfaceID& ID , 
                                     void**             II );
    
protected: 

  /** standard constructor 
   *  @param ServiceName     name of the service
   *  @param ServiceLocator  pointer to service locator
   */
  GiGaHits( const std::string&  ServiceName          , 
                  ISvcLocator*        ServiceLocator       );
  /// destructor 
  virtual ~GiGaHits();
  ///
  
  /** accessor to kine conversion service 
   *  @return pointer to Kine conversion service 
   */
  IGiGaKineCnvSvc*  kineSvc() const { return m_kineSvc ; }
  
private:
  
  /// no copy constructor 
  GiGaHits();
  /// no copy constructor
  GiGaHits( const GiGaHits& );
  /// no assignment
  GiGaHits& operator=( const GiGaHits& );
  
private:
  
  std::string       m_kineSvcName;
  IGiGaKineCnvSvc*  m_kineSvc;

};


// ============================================================================
#endif  ///<   GIGACNV_GiGaHits_H 
// ============================================================================































