#ifndef      __GIGA_KINECONVERSIONSERVICE_GIGATrajCnvSVC_H__ 
#define      __GIGA_KINECONVERSIONSERVICE_GIGATrajCnvSVC_H__  1 

///
/// from STL 
#include <string> 

///
/// from GiGa 
#include "GiGa/GiGaCnvSvc.h" 
#include "GiGa/IGiGaTrajCnvSvc.h" 

///
/// forward declarations
///

template <class SERVICE> class SvcFactory; 

class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 


///
///  GiGaTrajCnvSvc: conversion service  for converting of Geant4 trajectories 
///                  (GiGaTrajectories) into LHCbEvent MCParticle/MCVertex structure  
///  
///  Author: Vanya Belyaev 
///  Date    22 Feb 2001 


class GiGaTrajCnvSvc:  public          GiGaCnvSvc   ,
                       virtual public IGiGaTrajCnvSvc 
{
  ///
  friend class SvcFactory<GiGaTrajCnvSvc>;
  ///  
 protected: 
  /// constructor
  GiGaTrajCnvSvc( const std::string&  ServiceName          , 
		  ISvcLocator*        ServiceLocator       );
  /// virtual destructor
  virtual ~GiGaTrajCnvSvc(){}; 
  ///
 public: 
  ///
  virtual StatusCode            initialize() ;
  virtual StatusCode            finalize  () ; 
  ///
};        
///

#endif  //   __GIGA_KINECONVERSIONSERVICE_GIGATrajCnvSVC_H__ 































