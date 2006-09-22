// $Id: TransporterFunctions.h,v 1.1 2006-09-22 14:53:59 jpalac Exp $
#ifndef TRANSPORTERFUNCTIONS_H 
#define TRANSPORTERFUNCTIONS_H 1

// Include files
#include "GaudiKernel/StatusCode.h"
#include "Kernel/GenericMatrixTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

/** @namespace DaVinciTransporter TransporterFunctions.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-09-22
 */
namespace DaVinciTransporter {

  /**
   *  Transport a composite Particle to specified z position.
   *  The transport matrix is
   *  
   *   1  0  0 dz/Pz   0     0    0
   *   0  1  0   0   dz/Pz   0    0
   *   0  0  1   0     0   dz/Pz  0
   *   0  0  0   1     0     0    0
   *   0 ....
   *   .
   *   .  
   * 
   *  @author Juan PALACIOS
   *  @date   2006-09-22
   */
  StatusCode transportComposite(const LHCb::Particle* particle, 
                                const double zNew,
                                LHCb::Particle& transParticle) 
  {
    using namespace Gaudi;

    const double dZ = particle->referencePoint().Z()-zNew;
    const double pZ = particle->momentum().Z();
    const double coef = dZ/pZ;

    // Transport matrix
    Matrix7x7 transMatrix =  Matrix7x7( ROOT::Math::SMatrixIdentity() );
    transMatrix(0,3) = coef;
    transMatrix(1,4) = coef;
    transMatrix(2,5) = coef;

    // Calculate the new covariance matrix
    const SymMatrix7x7 transCov = 
      ROOT::Math::Similarity<double, Matrix7x7::kRows, Matrix7x7::kCols> (transMatrix, particle->covMatrix() );

    // Update covariance matrices in particle
    transParticle.setPosCovMatrix    ( transCov.Sub<SymMatrix3x3>(0,0) );
    transParticle.setMomCovMatrix    ( transCov.Sub<SymMatrix4x4>(3,3) );
    transParticle.setPosMomCovMatrix ( transCov.Sub<Matrix4x3>(3,0) );

    // calculate reference point
    XYZPoint refPoint(particle->referencePoint());
    refPoint += dZ*particle->slopes();

    // update reference point
    transParticle.setReferencePoint(refPoint);
   
    return StatusCode::SUCCESS;
 
  }
  
}
#endif // TRANSPORTERFUNCTIONS_H
