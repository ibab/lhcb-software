#ifndef      __GIGA_KINECONVERSIONSERVICE_GIGAKINECNVSVC_H__ 
#define      __GIGA_KINECONVERSIONSERVICE_GIGAKINECNVSVC_H__  1 

///
/// from STL 
#include <string> 

///
/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 
#include "GiGa/IGiGaKineCnvSvc.h" 

///
/// forward declarations
///

template <class SERVICE> class SvcFactory; 

class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 

///
///  GiGaKineCnvSvc: conversion service  for converting of Gaudi 
///                  MCParticle/MCVertex structure into Geant4 primary event record  
///  
///  Author: Vanya Belyaev 
///  Date    7 Aug 2000 


class GiGaKineCnvSvc:  public          GiGaCnvSvc   ,
                       virtual public IGiGaKineCnvSvc 
{
  
  friend class SvcFactory<GiGaKineCnvSvc>;
  
 protected: 
  
  ///
  /// constructor
  /// 
  GiGaKineCnvSvc( const std::string&  ServiceName          , 
		  ISvcLocator*        ServiceLocator       );
  
  /// 
  /// virtual destructor
  ///
  
  virtual ~GiGaKineCnvSvc(){};
 
 public: 

  ///
  virtual StatusCode            initialize() ;
  virtual StatusCode            finalize  () ; 
  ///
  virtual IParticlePropertySvc* ppSvc     () { return GiGaCnvSvc::ppSvc(); } 
  ///
};        


#endif  //   __GIGA_KINECONVERSIONSERVICE_GIGAKINECNVSVC_H__ 































