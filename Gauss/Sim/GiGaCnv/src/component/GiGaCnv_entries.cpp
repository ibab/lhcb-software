/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
#define GIGACNV_GIGACNV_ENTRIES_CPP 1 
/// ===========================================================================

// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"

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
  DECLARE_ALGORITHM  (  GiGaMiscGetEventAlg    ) ;
};

