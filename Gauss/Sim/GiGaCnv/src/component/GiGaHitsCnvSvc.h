/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/15 20:45:10  ibelyaev
/// the package restructurisation
/// 
/// ===========================================================================
#ifndef      GIGA_GIGAHITSCNVSVC_H 
#define      GIGA_GIGAHITSCNVSVC_H  1 
/// ===========================================================================
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

/** @class GiGaHitsCnvSvc GiGaHitsCnvSvc.h
 *    
 *  Convertersion service for convertiong Geant4 hits info Gaudi hits 
 *  
 *  @author  Vanya Belyaev
 *  @date    23/02/2001
 */

class GiGaHitsCnvSvc:  public          GiGaCnvSvcBase   ,
                       virtual public IGiGaHitsCnvSvc 
{
  ///
  friend class SvcFactory<GiGaHitsCnvSvc>;
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
  ///

  /** retrieve the relation table between Geant4 track/trajectory 
   *  identifiers and the converted MCParticle objects 
   *  @return the reference to relation table  
   */
  virtual GiGaKineRefTable&      table    ()  ;
  
protected: 

  /** standard constructor 
   *  @param ServiceName     name of the service
   *  @param ServiceLocator  pointer to service locator
   */
  GiGaHitsCnvSvc( const std::string&  ServiceName          , 
                  ISvcLocator*        ServiceLocator       );
  /// destructor 
  virtual ~GiGaHitsCnvSvc();
  ///
  
  /** accessor to kine conversion service 
   *  @return pointer to Kine conversion service 
   */
  IGiGaKineCnvSvc*  kineSvc() const { return m_kineSvc ; }
  
private:
  
  /// no copy constructor 
  GiGaHitsCnvSvc();
  /// no copy constructor
  GiGaHitsCnvSvc( const GiGaHitsCnvSvc& );
  /// no assignment
  GiGaHitsCnvSvc& operator=( const GiGaHitsCnvSvc& );
  
private:
  
  std::string       m_kineSvcName;
  IGiGaKineCnvSvc*  m_kineSvc;

};


/// ===========================================================================
#endif  ///<   GIGACNV_GIGAHITSCNVSVC_H 
/// ===========================================================================































