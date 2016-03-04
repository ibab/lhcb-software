
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
#include <array>

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidCons.h"

// Local
#include "RichDet/DeRichBase.h"

// LHCbMath
//#include "LHCbMath/Line.h"
//#include "LHCbMath/GeomFun.h"

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
  
  /// Internal representation of a line
  //using LINE = Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>;

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
   *  Finds the entry and exit points of the beam pipe equivalent (central
   *  tube in Rich2). For boolean solids
   *  this is the first and last intersection point.
   *
   *  @param[in]  start The start point of the vector to test
   *  @param[in]  end   The end point of the vector to test
   *  @param[out] entryPoint The entry point to the beam-pipe volume
   *  @param[out] exitPoint  The exit point from the beam-pipe volume
   *
   *  @return Enum describing the status of the intersection
   */
  BeamPipeIntersectionType intersectionPoints( const Gaudi::XYZPoint& start,
                                               const Gaudi::XYZPoint& end,
                                               Gaudi::XYZPoint& entryPoint,
                                               Gaudi::XYZPoint& exitPoint ) const;
  /** 
   *  Test if a given direction intersects the beam-pipe volume at all.
   *  Faster than intersectionPoints since it does not compute the intersection points
   *  in global coordinates.
   *
   *  @param[in]  start The start point of the vector to test
   *  @param[in]  end   The end point of the vector to test
   *
   *  @return boolean indicating if the beam pipe was intersected or not
   *  @retval true  The beam pipe was intersected
   *  @retval false The beam pipe was NOT intersected
   */
  bool testForIntersection( const Gaudi::XYZPoint& start,
                            const Gaudi::XYZPoint& end ) const;

public:

  /// Convert the enum to text for easy reading
  static std::string text(const DeRichBeamPipe::BeamPipeIntersectionType& type);

private:

  /// Returns the 'average' of two points
  template< class POINT >
  inline POINT average( const POINT& p1, const POINT& p2 ) const
  {
    return POINT( 0.5*(p1.x()+p2.x()),
                  0.5*(p1.y()+p2.y()),
                  0.5*(p1.z()+p2.z()) );
  }

  /// Test if the given start and end points are 'close' to the beampipe or not
  inline bool isCloseBy( const Gaudi::XYZPoint& start,
                         const Gaudi::XYZPoint& end ) const
  {
    return ( isCloseBy(start) || isCloseBy(end)
             // || isCloseBy(average(start,end))
             );
  }

  /// Test if the given point is 'close' to the beampipe or not
  inline bool isCloseBy( const Gaudi::XYZPoint& p ) const
  {
    // Get the closest z coord in the beam pipe
    const auto beamz  = ( p.z() > m_endPGlo.z()   ? m_endPGlo.z()   :
                          p.z() < m_startPGlo.z() ? m_startPGlo.z() :
                          p.z() );
    // Get (beam pipe axis (x,y) and R^2 at this point in z position
    const auto beamx  = ( m_m[0] * beamz ) + m_c[0];
    const auto beamy  = ( m_m[1] * beamz ) + m_c[1];
    const auto beamR2 = ( m_m[2] * beamz ) + m_c[2];
    const auto dist2  = ( std::pow(beamx-p.x(),2) + 
                          std::pow(beamy-p.y(),2) + 
                          std::pow(beamz-p.z(),2) );
    //info() << "Point " << p << endmsg;
    //info() << "Beam  " << beamx << " " << beamy << " " << beamz << endmsg;
    //info() << " -> Dist2 " << dist2 << " " << beamR2 << " " << ( dist2 < beamR2 ) << endmsg;
    return ( dist2 < beamR2 );
  }

private: // data

  // The RICH detector
  Rich::DetectorType m_rich = Rich::InvalidDetector;

  /// A copy of the beam pipe cone that is solid (not hollow)
  std::unique_ptr<SolidCons> m_localCone;

  /// Global position on the z axis for the start of the beampipe
  Gaudi::XYZPoint m_startPGlo;

  /// Global position on the z axis for the end of the beampipe
  Gaudi::XYZPoint m_endPGlo;

  // parameters for y = mx +c scaling of cone axis (x,y) and R^2 as a function of z
  std::array<double,3> m_m, m_c;

};

//==============================================================================

inline std::ostream& operator << ( std::ostream& s,
                                   const  DeRichBeamPipe::BeamPipeIntersectionType& type )
{
  return s << DeRichBeamPipe::text( type );
}

#endif    // RICHDET_DERICHBEAMPIPE_H
