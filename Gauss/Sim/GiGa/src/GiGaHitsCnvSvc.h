#ifndef      __GIGA_HitsCONVERSIONSERVICE_GIGAHitsCNVSVC_H__ 
#define      __GIGA_HitsCONVERSIONSERVICE_GIGAHitsCNVSVC_H__  1 

///
/// from STL 
#include <string> 

///
/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 
#include "GiGa/IGiGaHitsCnvSvc.h" 

///
/// forward declarations
///

template <class SERVICE> class SvcFactory; 

class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 

///
///  GiGaHitsCnvSvc: conversion service  for converting of Gaudi 
///                  MCParticle/MCVertex structure into Geant4 primary event record  
///  
///  Author: Vanya Belyaev 
///  Date    7 Aug 2000 

///
class GiGaHitsCnvSvc:  public          GiGaCnvSvc   ,
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
  virtual IParticlePropertySvc* ppSvc     () { return GiGaCnvSvc::ppSvc(); } 
  ///
};        
///
#endif  //   __GIGA_HitsCONVERSIONSERVICE_GIGAHitsCNVSVC_H__ 































