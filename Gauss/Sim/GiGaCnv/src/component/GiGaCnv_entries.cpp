// $Id: GiGaCnv_entries.cpp,v 1.4 2001-11-19 18:27:00 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/08/12 17:24:51  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.2  2001/07/15 20:45:09  ibelyaev
// the package restructurisation
// 
// ============================================================================
#define GIGACNV_GIGACNV_ENTRIES_CPP 1 
// ============================================================================

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
  DECLARE_CONVERTER  (  GiGaCatalogCnv         ) ; 
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaKineCnvSvc         ) ;
  DECLARE_CONVERTER  (  GiGaMCParticleCnv      ) ; 
  DECLARE_CONVERTER  (  GiGaMCVertexCnv        ) ; 
  /// service and it s converters 
  DECLARE_SERVICE    (  GiGaHitsCnvSvc         ) ;
  /// Algorithms 
  DECLARE_ALGORITHM  (  GiGaMiscGetEventAlg    ) ;
};

// ============================================================================
// The End 
// ============================================================================
