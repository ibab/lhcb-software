// $Id: CaloPhotonMomentum.h,v 1.2 2002-05-23 11:07:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOUTILS_CALOPHOTONMOMENTUM_H 
#define CALOUTILS_CALOPHOTONMOMENTUM_H 1
// Include files
// STD & STL 
#include <functional>
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
// forward declarations 
class CaloMomentum ;   // CaloEvent/Event 
class CaloPosition ;   // CaloEvent/Event

/** @struct CaloPhotonMomentum CaloPhotonMomentum.h
 *  
 *  Simple structure to extract the 4-momentum 
 *  (and the covarinace matrix!)
 *  form energy-position (and covariance matrix!)
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   21/03/2002
 */
class CaloPhotonMomentum:
  public std::unary_function<const CaloPosition,CaloMomentum*>
{
public:
  
  /** constructor 
   *  @param point null-point (the IP)
   */
  CaloPhotonMomentum( const HepPoint3D& point = HepPoint3D() )
    : m_null( point ) {};
  
  /// destructor 
  ~CaloPhotonMomentum(){};
  
  /** The only one essential method (functor interface)
   *  @exception CaloException for very tiny vector lengths
   *  @param  position CaloPosition object
   *  Ereturn new created CaloMomentum object
   */
  CaloMomentum* operator() ( const CaloPosition& position ) const;
  
  /** set new position of "null" vertex 
   *  @param null new position of nnull vertex 
   *  @return position fo null vertex 
   */
  void setNull( const HepPoint3D& null = HepPoint3D() ) { m_null = null ;}
  
private:
  
  HepPoint3D m_null ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOUTILS_CALOPHOTONMOMENTUM_H
// ============================================================================
