/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef      GIGA_GIGAKINECNVSVC_H 
#define      GIGA_GIGAKINECNVSVC_H  1 
/// ===========================================================================
/// from STL 
#include <string> 
/// from GiGa 
#include "GiGa/GiGaCnvSvcBase.h" 
#include "GiGa/IGiGaKineCnvSvc.h" 
/// from GiGaCnv  
///
template <class SERVICE> 
class SvcFactory; 
///
class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 

/** @class GiGaGeomCnvSvc GiGaGeomCnvSvc.h GiGa/GiGaGeomCnvSvc.h
    
    conversion service  for converting of Gaudi 
    MCParticle/MCVertex structure into Geant4 primary event record  
    and vice versa 
    
    @author  Vanya Belyaev
    @date    07/08/2000
*/


class GiGaKineCnvSvc:  public          GiGaCnvSvcBase ,
                       virtual public IGiGaKineCnvSvc 
{
  ///
  friend class SvcFactory<GiGaKineCnvSvc>;
  ///
protected: 
  /// constructor
  GiGaKineCnvSvc( const std::string& , ISvcLocator* );
  /// virtual destructor
  virtual ~GiGaKineCnvSvc(){};
  /// 
public: 
  ///
  virtual StatusCode            initialize() ;
  virtual StatusCode            finalize  () ; 
  ///
  virtual IParticlePropertySvc* ppSvc     () 
  { return GiGaCnvSvcBase::ppSvc(); } 
  ///
};        
///


#endif  //   __GIGA_KINECONVERSIONSERVICE_GIGAKINECNVSVC_H__ 































