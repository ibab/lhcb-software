// $Id: GiGaCnv_entries.cpp,v 1.6 2002-12-07 14:36:25 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define GIGACNV_GIGACNV_ENTRIES_CPP 1 
// ============================================================================

// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( GiGaCnv ) 
{
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaGeo                ) ;
  DECLARE_CONVERTER  (  GiGaLVolumeCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaSurfaceCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaDetectorElementCnv ) ; 
  DECLARE_CONVERTER  (  GiGaIsotopeCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaElementCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaMixtureCnv         ) ; 
  DECLARE_CONVERTER  (  GiGaCatalogCnv         ) ; 
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaKine               ) ;
  DECLARE_CONVERTER  (  GiGaMCParticleCnv      ) ; 
  DECLARE_CONVERTER  (  GiGaMCVertexCnv        ) ; 
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaHits               ) ;
  /// Algorithms 
  DECLARE_ALGORITHM  (  GiGaMiscGetEventAlg    ) ;
  DECLARE_ALGORITHM  (  GiGaDataStoreAlgorithm ) ;
};

// ============================================================================
// The End 
// ============================================================================
