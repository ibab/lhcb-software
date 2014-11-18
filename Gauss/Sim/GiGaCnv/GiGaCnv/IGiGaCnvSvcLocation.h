// $Id: IGiGaCnvSvcLocation.h,v 1.1 2002-12-07 14:36:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGACNV_IGIGACNVSVCLOCATION_H 
#define GIGACNV_IGIGACNVSVCLOCATION_H 1
// Include files
// STD & STL 
#include <string>

/** @namespace IGiGaCnvSvcLocation GiGaCnv/IGiGaCnvSvcLocation.h
 *  
 *  The list of default types and names for major GiGa Conversion Services 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-05
 */
namespace IGiGaCnvSvcLocation 
{
  const std::string Geo      ( "GiGaGeo"         ) ;
  const std::string Kine     ( "GiGaKine"        ) ;  
  const std::string Hits     ( "GiGaHits"        ) ;
}

// ============================================================================
// The END 
// ============================================================================
#endif // GIGACNV_IGIGACNVSVCLOCATION_H
// ============================================================================
