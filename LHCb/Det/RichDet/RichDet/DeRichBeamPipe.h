
/** @file DeRichBeamPipe.h
 *
 *  Header file for detector description class : DeRichBeamPipe
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-11-27
 */

#ifndef RICHDET_DERICHBEAMPIPE_H
#define RICHDET_DERICHBEAMPIPE_H 1

// STL
#include <memory>

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidCons.h"

// Local
#include "RichDet/DeRichBase.h"

// LHCbMath
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"

// External declarations
extern const CLID CLID_DERichBeamPipe;

/** @class DeRichBeamPipe RichDet/DeRichBeamPipe.h
 *
 * Class for generic description of the BeamPipe (equivalent) in the Rich
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-11-27
 */
class DeRichBeamPipe : public DeRichBase
{

private:
  
  // Internally representation of a line
  using LINE = Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>;

public:

  /// Enum describing the various possible types of intersection
  enum BeamPipeIntersectionType
    {
      NoIntersection = 0, ///< Did not intersect the beam pipe at all
      FrontAndBackFace,   ///< Entered via the front face and left via the back face (i.e. totally inside the beampipe)
      FrontFaceAndCone,   ///< Entered via the front face and left via the cone surface
      BackFaceAndCone,    ///< Entered via the cone surafece and left via the backface
      ConeOnly            ///< Entered via the cone surafece and left via the cone surface
    };

public:

  /**
   * Constructor for this class
   */
  DeRichBeamPipe(const std::string & name = "");

  /**
   * Default destructor
   */
  virtual ~DeRichBeamPipe();

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

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

public:

  /**
   * Finds the entry and exit points of the beam pipe equivalent (central
   * tube in Rich2). For boolean solids
   * this is the first and last intersection point.
   *
   * @param[in]  position   The start point to ray trace from
   * @param[in]  direction  The vector to ray trace from the start point
   * @param[out] entryPoint The entry point to the beam-pipe volume
   * @param[out] exitPoint  The exit point from the beam-pipe volume
   *
   * @return Enum describing the status of the intersection
   */
  BeamPipeIntersectionType intersectionPoints( const Gaudi::XYZPoint& position,
                                               const Gaudi::XYZVector& direction,
                                               Gaudi::XYZPoint& entryPoint,
                                               Gaudi::XYZPoint& exitPoint ) const;

  /** Test if a given direction intersects the beam-pipe volume at all.
   *  Faster than intersectionPoints since it does not compute the intersection points
   *  in global coordinates.
   *  @param[in]  position    The start point to ray trace from
   *  @param[in]  direction   The vector to ray trace from the start point
   *  @return boolean indicating if the beam pipe was intersected or not
   *  @retval true  The beam pipe was intersected
   *  @retval false The beam pipe was NOT intersected
   */
  bool testForIntersection( const Gaudi::XYZPoint&  position,
                            const Gaudi::XYZVector& direction ) const;

public:

  /**
   * Convert the enum to text for easy reading
   */
  static std::string text(const DeRichBeamPipe::BeamPipeIntersectionType& type);

private:

  /// Fast check to see if a LINE is close to the beampipe or not
  inline bool isCloseBy( const LINE & line ) const
  {
    /// Get the point on the line at the beam pipe start
    const auto entP = Gaudi::Math::closestPoint( m_startP, line );
    bool isClose = ( entP.Perp2() < m_startRad2 );
    if ( !isClose )
    {
      /// Get the point on the line at the beam pipe end and try with that
      const auto endP = Gaudi::Math::closestPoint( m_endP, line );
      isClose = ( endP.Perp2() < m_endRad2 );
    }
    return isClose;
  }

private: // data

  const ISolid* m_solid = nullptr;  ///< solid used for the beam pipe

  /// A copy of the beam pipe cone that is solid (not hollow)
  std::unique_ptr<SolidCons> m_localCone;

  /// Half length of the cone along z
  double m_zHalfLength{0};  

  /// Radius squared at start point
  double m_startRad2{0}; 

  /// Radius squared at end point
  double m_endRad2{0}; 

  /// Position on the z axis for the start of the beampipe
  Gaudi::XYZPoint m_startP;

  /// Position on the z axis for the end of the beampipe
  Gaudi::XYZPoint m_endP;

};

//==============================================================================

inline std::ostream& operator << ( std::ostream& s,
                                   const  DeRichBeamPipe::BeamPipeIntersectionType& type )
{
  return s << DeRichBeamPipe::text( type );
}

#endif    // RICHDET_DERICHBEAMPIPE_H
