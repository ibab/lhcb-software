// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaCnv_entries.cpp,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#define GIGACNV_GIGACNV_ENTRIES_CPP 1 

// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "GiGa/IGiGaSensDetFactory.h"
#define DECLARE_SDFACTORY(x)     extern const IGiGaSensDetFactory&     x##Factory; x##Factory.addRef();


DECLARE_FACTORY_ENTRIES( GiGaCnv ) 
{
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaGeomCnvSvc         ) ;
  DECLARE_CONVERTER  (  GiGaLVolumeCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaSurfaceCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaDetectorElementCnv ) ; 
  DECLARE_CONVERTER  (  GiGaIsotopeCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaElementCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaMixtureCnv         ) ; 
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaKineCnvSvc         ) ;
  DECLARE_CONVERTER  (  GiGaMCParticleCnv      ) ; 
  DECLARE_CONVERTER  (  GiGaMCVertexCnv        ) ; 
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaHitsCnvSvc         ) ;
  /// Algorithms 
  DECLARE_ALGORITHM  (  ParticleGunAlgorithm   ) ;
  DECLARE_ALGORITHM  (  GiGaMiscGetEventAlg    ) ;
  /// Sensitive Detector 
  DECLARE_SDFACTORY  (  GiGaSensDetPrint       ) ;

};

