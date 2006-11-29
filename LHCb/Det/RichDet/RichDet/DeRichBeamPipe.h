
/** @file DeRichBeamPipe.h
 *
 *  Header file for detector description class : DeRichBeamPipe
 *
 *  CVS Log :-
 *  $Id: DeRichBeamPipe.h,v 1.1 2006-11-29 10:36:20 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-11-27
 */

#ifndef RICHDET_DERICHBEAMPIPE_H
#define RICHDET_DERICHBEAMPIPE_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidCons.h"

// LHCbKernel

// External declarations
extern const CLID& CLID_DERichBeamPipe;


/** @namespace DeRichLocation
 *
 *  Namespace for the xml location of the detector elements RichSystem
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichBeamPipeLocation {

  /// RichSystem location in transient detector store
  static const std::string& Rich1BeamPipe = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1BeamPipe";
  static const std::string& Rich2BeamPipe = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2BeamPipe";
}

/** @class DeRichBeamPipe DeRichBeamPipe.h
 *
 * Class for generic description of the BeamPipe (equivalent) in the Rich
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-11-27
 */
class DeRichBeamPipe: public DetectorElement {

public:

  enum BeamPipeIntersectionType {
    NoIntersection = 0,
    FrontAndBackFace,
    FrontFaceAndCone,
    BackFaceAndCone,
    ConeOnly
  };

  /**
   * Constructor for this class
   */
  DeRichBeamPipe();

  /**
   * Default destructor
   */
  virtual ~DeRichBeamPipe();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Finds the entry and exit points of the beam pipe equivalent (central
   * tube in Rich2). For boolean solids
   * this is the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  BeamPipeIntersectionType intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                               const Gaudi::XYZVector& vGlobal,
                                               Gaudi::XYZPoint& entryPoint,
                                               Gaudi::XYZPoint& exitPoint ) const;

  /**
   * Convert the enum to text for easy reading
   */
  static std::string text(const  DeRichBeamPipe::BeamPipeIntersectionType& type);


private: // functions


private: // data

  const ISolid* m_solid;  ///< solid used for the beam pipe

  /// A copy of the beam pipe cone that is solid (not hollow)
  SolidCons* m_localCone;

  double m_zHalfLength;  ///< Half lenght of the cone along z

};

//==============================================================================

inline std::ostream& operator << ( std::ostream& s,
                                   const  DeRichBeamPipe::BeamPipeIntersectionType& type )
{
  s << DeRichBeamPipe::text( type );
  return s;
}

#endif    // RICHDET_DERICHBEAMPIPE_H
