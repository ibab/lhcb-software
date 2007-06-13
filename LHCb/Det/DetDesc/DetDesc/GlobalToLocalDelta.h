// $Id: GlobalToLocalDelta.h,v 1.6 2007-06-13 16:36:55 jpalac Exp $
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
 *  @author Juan Palacios juan.palacios@nikhef.nl
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
   * @author Juan Palacios juan.palacios@nikhef.nl
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
   * @author Juan Palacios juan.palacios@nikhef.nl
   *
  */
  const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                            const Gaudi::Transform3D& globalDelta);


  /**
   * Transform the nine 3D transformation parameters from the global frame into the local one.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 2007-06-07
   *
   *
   * @param DE Pointer to the IDetectorElement of the "local" detector element.
   * @param trans global 3D translation parameters. Gets filled with local parameters.
   * @param rot   global 3D ZYX rotation parameters. Gets filled with local parameters.
   * @param pivot global 3D pivot parameters. Gets filled with local pivot parameters. These
   *              are constant and get taken directly from the conditions database.
   *
   */
  void localDeltaParameters(const IDetectorElement* DE,
                            std::vector<double>& trans,
                            std::vector<double>& rot,
                            std::vector<double>& pivot);

  /**
   * Transform the nine 3D transformation parameters from the global frame into the local one.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 2007-06-07
   *
   * @param GI Pointer to the IGeometryInfo of the "local" detector element.
   * @param trans global 3D translation parameters. Gets filled with local parameters.
   * @param rot   global 3D ZYX rotation parameters. Gets filled with local parameters.
   * @param pivot global 3D pivot parameters. Gets filled with local pivot parameters. These
   *              are constant and get taken directly from the conditions database.
   *
   */
  void localDeltaParameters(const IGeometryInfo* GI,
                            std::vector<double>& trans,
                            std::vector<double>& rot,
                            std::vector<double>& pivot);

  
}
#endif // DETDESC_GLOBALTOLOCALDELTA_H
