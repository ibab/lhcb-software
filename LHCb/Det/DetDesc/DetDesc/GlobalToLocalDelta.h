// $Id: GlobalToLocalDelta.h,v 1.9 2007-07-11 16:36:35 jpalac Exp $
#ifndef DETDESC_GLOBALTOLOCALDELTA_H 
#define DETDESC_GLOBALTOLOCALDELTA_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"

// forward declarations
class IDetectorElement;
class IGeometryInfo;

/** @file  GlobalToLocalDelta.h
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
   *         Misaligned position matrix = globalDelta*M_nominal_global

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
   *         Misaligned position matrix = globalDelta*M_nominal_global

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


  /**
   * Obtain the local delta transformation starting from a global position transformation.
   * @param  DE detector element in question.
   * @param  globalTransform 3D transformation that describes 
   *         the total "misaligned" global matrix T'
   *         Misaligned position matrix T' = T * globalDelta^-1 * globalDelta'

   * @return 3D transformation that globalDelta' applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-07-11
   * @author Juan Palacios juan.palacios@nikhef.nl
   *
  */
  const Gaudi::Transform3D localDeltaFromGlobalTransform(const IDetectorElement* DE,
                                                         const Gaudi::Transform3D& globalTransform);


  /**
   * Obtain the local delta transformation starting from a global position transformation.
   * @param  GI the IGeometryInfo of the detector element in question
   * @param  globalTransform 3D transformation that describes 
   *         the total "misaligned" global matrix T'
   *         Misaligned position matrix T' = T * globalDelta^-1 * globalDelta'

   * @return 3D transformation that globalDelta' applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-07-11
   * @author Juan Palacios juan.palacios@nikhef.nl
   *
  */
  const Gaudi::Transform3D localDeltaFromGlobalTransform(const IGeometryInfo* GI,
                                                         const Gaudi::Transform3D& globalTransform);

    /**
   * Obtain the local delta transformation starting from a position transformation 
   * from an arbitrary parent.
   * @param  parentDE the IDetectorElement of the parent with respect to which the position
   *         is taken
   * @param  DE detector element in question.
   * @param  globalTransform 3D transformation that describes 
   *         the total "misaligned" global matrix T_k'
   *         Misaligned position matrix T_k' = T_k * globalDelta^-1 * globalDelta'

   * @return 3D transformation that globalDelta' applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-07-11
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @todo Add check for parentDE really being a parent of DE
   *
  */
  const Gaudi::Transform3D localDeltaFromParentTransform(const IDetectorElement* parentDE,
                                                         const IDetectorElement* DE,
                                                         const Gaudi::Transform3D& globalTransform);


  /**
   * Obtain the local delta transformation starting from a position transformation
   * from an arbitrary parent.
   * @param  parentGI the IGeometryInfo of the parent with respect to which the position
   *         is taken
   * @param  GI the IGeometryInfo of the detector element in question
   * @param  globalTransform 3D transformation that describes 
   *         the total "misaligned" global matrix T'
   *         Misaligned position matrix T_k' = T_k * globalDelta^-1 * globalDelta'

   * @return 3D transformation that globalDelta' applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-07-11
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @todo Add check for parentGI really being a parent of GI
   *
  */
  const Gaudi::Transform3D localDeltaFromParentTransform(const IGeometryInfo* ParentGI,
                                                         const IGeometryInfo* GI,
                                                         const Gaudi::Transform3D& globalTransform);

  
}
#endif // DETDESC_GLOBALTOLOCALDELTA_H
