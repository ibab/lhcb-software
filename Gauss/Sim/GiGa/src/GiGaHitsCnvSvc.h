#ifndef      GIGA_GIGAHitsCNVSVC_H 
#define      GIGA_GIGAHitsCNVSVC_H  1 
/// STL 
#include <string> 
/// GiGa 
#include "GiGa/GiGaCnvSvcBase.h" 
#include "GiGa/IGiGaHitsCnvSvc.h" 
///
template <class SERVICE> 
class SvcFactory; 
///
class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 

/** @class GiGaGeomCnvSvc GiGaGeomCnvSvc.h GiGa/GiGaGeomCnvSvc.h
    
    Convertersion service for convertiong Geant4 hits info Gaudi hits 
    
    @author  Vanya Belyaev
    @date    23/02/2001
*/


///
class GiGaHitsCnvSvc:  public          GiGaCnvSvcBase   ,
                       virtual public IGiGaHitsCnvSvc 
{
  ///
  friend class SvcFactory<GiGaHitsCnvSvc>;
  ///
protected: 
  ///
  GiGaHitsCnvSvc( const std::string&  ServiceName          , 
		  ISvcLocator*        ServiceLocator       );
  ///
  virtual ~GiGaHitsCnvSvc(){};
  ///
public: 
  ///
  virtual StatusCode            initialize() ;
  ///
  virtual StatusCode            finalize  () ; 
  ///
};        
///

#endif  //   GIGA_GIGAHitsCNVSVC_H 































