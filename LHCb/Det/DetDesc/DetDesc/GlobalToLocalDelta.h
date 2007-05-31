// $Id: GlobalToLocalDelta.h,v 1.1 2007-05-31 14:46:55 jpalac Exp $
#ifndef DETDESC_GLOBALTOLOCALDELTA_H 
#define DETDESC_GLOBALTOLOCALDELTA_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"

// forward declarations
class IDetectorElement;
class IGeometryInfo;

/** @namespace DetDesc  GlobalToLocalDelta.h DetDesc/GlobalToLocalDelta.h
 *  
 *  Free functions to go from a 3D global "delta" matrix to the
 *  corresponding matrix in the frame of the detector element's 
 *  parent.
 *
 *  @author Juan Palacios
 *  @date   2007-05-31
 *  @todo Put this and other tools in some other place?
 */
namespace DetDesc {


  /**
   * @param  DE detector element in question.
   * @param  globalDelta 3D transformation that takes 
   *         global nominal position matrix and applies misalignment:
   *         Misalinged position matrix = globalDelta*M_nominal_global
   * @return 3D transformation that applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-05-31
   * @author Juan Palacios juan.palacios@cern.ch
   *
  */
  const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                            const Gaudi::Transform3D& globalDelta);  
  
  /**
   * @param  GI IGeometryInfo from detector element in question.
   * @param  globalDelta 3D transformation that takes 
   *         global nominal position matrix and applies misalignment:
   *         Misalinged position matrix = globalDelta*M_nominal_global
   * @return 3D transformation that applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-05-31
   * @author Juan Palacios juan.palacios@cern.ch
   *
  */
  const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                            const Gaudi::Transform3D& globalDelta);

}
#endif // DETDESC_GLOBALTOLOCALDELTA_H
