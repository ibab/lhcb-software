// $Id: CnvPhotonParticleMaker.h,v 1.1 2003-04-08 17:22:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef DAVINCITOOLS_CNVPHOTONPARTICLEMAKER_H 
#define DAVINCITOOLS_CNVPHOTONPARTICLEMAKER_H 1
// ============================================================================
// Include files
// local 
#include "PhotonParticleMaker.h"
// ============================================================================

// ============================================================================
/** @class CnvPhotonParticleMaker CnvPhotonParticleMaker.h
 *
 *  Particle maker for converted photons 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-08
 */
// ============================================================================
class CnvPhotonParticleMaker : public PhotonParticleMaker
{  
  /// friend class for instantiation 
  friend class ToolFactory<CnvPhotonParticleMaker> ;
protected:
  
  /** Standard constructor
   *  @param type   tool type
   *  @param name   tool name 
   *  @param parent tool parent
   */
  CnvPhotonParticleMaker
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  // destructor 
  virtual ~CnvPhotonParticleMaker() ;
  
private:
  
  // the default constructor  is disabled 
  CnvPhotonParticleMaker() ;
  // the copy constructor     is disabled
  CnvPhotonParticleMaker           ( const CnvPhotonParticleMaker& );
  // the assignement operator is disabled
  CnvPhotonParticleMaker& operator=( const CnvPhotonParticleMaker& );
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CNVPHOTONPARTICLEMAKER_H
// ============================================================================
