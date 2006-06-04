
//-----------------------------------------------------------------------------
/** @file RichTrackSegment.h
 *
 *  Header file for tool interface : RichTrackSegment
 *
 *  CVS Log :-
 *  $Id: RichTrackSegment.h,v 1.6 2006-06-04 16:57:24 jonrob Exp $
 *
 *  @author Antonis Papanestis   Antonis.Papanestis@cern.ch
 *  @author Chris Jones          Christopher.Rob.Jones@cern.ch
 *  @date   2002-05-31
 *
 *  @todo Check if normalisation of axis vectors is really needed
 */
//-----------------------------------------------------------------------------

#ifndef RICHUTILS_RICHTRACKSEGMENT_H
#define RICHUTILS_RICHTRACKSEGMENT_H 1

// std include
#include <iostream>

// LHCbKernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/Transform3DTypes.h"

// geometry
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// richkernel
#include "RichKernel/BoostMemPoolAlloc.h"

/// General LHCb namespace
namespace LHCb
{

  //-----------------------------------------------------------------------------
  /** @class RichTrackSegment RichTrackSegment.h RichUtils/RichTrackSegment.h
   *
   *  RichTrackSegment is a track segment inside a radiator.
   *
   *  @author Antonis Papanestis   Antonis.Papanestis@cern.ch
   *  @author Chris Jones          Christopher.Rob.Jones@cern.ch
   *  @date   2002-05-31
   */
  //-----------------------------------------------------------------------------

  class RichTrackSegment : public Rich::BoostMemPoolAlloc<RichTrackSegment>
  {

  public: // helper classes

    //-----------------------------------------------------------------------------
    /** @class StateErrors RichTrackSegment.h RichUtils/RichTrackSegment.h
     *
     *  Helper class for RichTrackSegment to describe the segment errors.
     *
     *  @author Chris Jones          Christopher.Rob.Jones@cern.ch
     *  @date   2004-05-31
     */
    //-----------------------------------------------------------------------------
    class StateErrors
    {

    public:

      /// Copy constructor
      StateErrors( const StateErrors & errors ) :
        m_errX2  ( errors.errX2()  ),
        m_errY2  ( errors.errY2()  ),
        m_errTX2 ( errors.errTX2() ),
        m_errTY2 ( errors.errTY2() ),
        m_errP2  ( errors.errP2()  ) { }

      /// Constructor with explicit values
      StateErrors( const float errX2  = 0, ///< error on x squared
                   const float errY2  = 0, ///< error on y squared
                   const float errTX2 = 0, ///< error on x slope squared
                   const float errTY2 = 0, ///< error on y slope squared
                   const float errP2  = 0  ///< error on momentum squared
                   )
        : m_errX2  ( errX2  ),
          m_errY2  ( errY2  ),
          m_errTX2 ( errTX2 ),
          m_errTY2 ( errTY2 ),
          m_errP2  ( errP2  ) { }

      inline float errX2()  const { return m_errX2;  }  ///< Access the x error squared
      inline float errY2()  const { return m_errY2;  }  ///< Access the y error squared
      inline float errTX2() const { return m_errTX2; }  ///< Access the tx error squared
      inline float errTY2() const { return m_errTY2; }  ///< Access the ty error squared
      inline float errP2()  const { return m_errP2;  }  ///< Access the P error squared

      inline float errX()  const { return sqrt(errX2());  }  ///< Access the x error
      inline float errY()  const { return sqrt(errY2());  }  ///< Access the y error
      inline float errTX() const { return sqrt(errTX2()); }  ///< Access the tx error
      inline float errTY() const { return sqrt(errTY2()); }  ///< Access the ty error
      inline float errP()  const { return sqrt(errP2());  }  ///< Access the P error

      ///< send to std::ostream
      inline friend std::ostream& operator << ( std::ostream& s,
                                                const StateErrors& errs )
      {
        s << "{ errX2=" << errs.errX2() << " errY2=" << errs.errY2()
          << " errTX2=" << errs.errTX2() << " errTY2=" << errs.errTY2()
          << " errP2=" << errs.errP2() << " }";
        return s;
      }

    private:

      float m_errX2;  ///< The x error squared
      float m_errY2;  ///< The y error squared
      float m_errTX2; ///< The tx error squared
      float m_errTY2; ///< The ty error squared
      float m_errP2;  ///< The P error squared

    };

    /** @class UseChordBetweenStates
     *
     *  Initialisation class used to identify the RichTrackSegment constructors.
     *
     *  This version uses chord between the entry and exit points.
     *
     *  @author Chris Jones          Christopher.Rob.Jones@cern.ch
     *  @date   2004-05-31
     */
    class UseChordBetweenStates { };

    /** @class UseAllStateVectors
     *
     *  Initialisation class used to identify the RichTrackSegment constructors.
     *
     *  This version uses full state information to define the segment direction
     *
     *  @author Chris Jones          Christopher.Rob.Jones@cern.ch
     *  @date   2004-05-31
     */
    class UseAllStateVectors    { };

  public:

    /// Delete and reset the current rotations
    inline void cleanUpRotations() const
    {
      if ( m_rotation  ) { delete m_rotation;  m_rotation  = 0; }
      if ( m_rotation2 ) { delete m_rotation2; m_rotation2 = 0; }
    }

    /// Two state, rich and radiator constructor. Uses the average of the entry and exit
    /// momenta and points as the middle (best) point
    /// Uses supplied momentum vectors at entrance and exit
    RichTrackSegment( const UseAllStateVectors,      ///< Tag to indentify this constructor
                      const Gaudi::XYZPoint& entrP,  ///< The entry point to the radiator
                      const Gaudi::XYZVector& entrV, ///< The momentum vector at entry to the radiator
                      const Gaudi::XYZPoint& exitP,  ///< The exit point from the radiator
                      const Gaudi::XYZVector& exitV, ///< The momentum vector at exit from the radiator
                      const Rich::RadiatorType rad,  ///< The radiator type
                      const Rich::DetectorType rich, ///< The detector type
                      const StateErrors entryErrors  = StateErrors(), ///< The segment errors at entry
                      const StateErrors exitErrors   = StateErrors()  ///< The segment errors at exit
                      )
      : m_entryPoint     ( entrP           ),
        m_middlePoint    ( Gaudi::XYZPoint ( (entrP.x()+exitP.x())/2,
                                             (entrP.y()+exitP.y())/2,
                                             (entrP.z()+exitP.z())/2 ) ),
        m_exitPoint      ( exitP           ),
        m_entryMomentum  ( entrV           ),
        m_middleMomentum ( (entrV+exitV)/2 ),
        m_exitMomentum   ( exitV           ),
        m_radiator       ( rad  ),
        m_rich           ( rich ),
        m_errorsEntry    ( entryErrors  ),
        m_errorsMiddle   ( Rich::Rich1Gas == rad ? exitErrors : entryErrors ),
        m_errorsExit     ( exitErrors   ),
        m_avPhotonEnergy ( 0 ),
        m_rotation       ( 0 ),
        m_rotation2      ( 0 ) { }

    /// Two state, rich and radiator constructor. Uses the average of the entry and exit
    /// momenta and points as the middle (best) point
    /// Uses the vector between the entry and exit points as the track direction
    RichTrackSegment( const UseChordBetweenStates,     ///< Tag to indentify this constructor
                      const Gaudi::XYZPoint& entrP,    ///< The entry point to the radiator
                      const Gaudi::XYZVector& entrV,   ///< The momentum vector at entry to the radiator
                      const Gaudi::XYZPoint& exitP,    ///< The exit point from the radiator
                      const Gaudi::XYZVector& exitV,   ///< The momentum vector at exit from the radiator
                      const Rich::RadiatorType rad,    ///< The radiator type
                      const Rich::DetectorType rich,   ///< The detector type
                      const StateErrors entryErrors  = StateErrors(), ///< The segment errors at entry
                      const StateErrors exitErrors   = StateErrors()  ///< The segment errors at exit
                      )
      : m_entryPoint     ( entrP           ),
        m_middlePoint    ( Gaudi::XYZPoint ( (entrP.x()+exitP.x())/2,
                                             (entrP.y()+exitP.y())/2,
                                             (entrP.z()+exitP.z())/2 ) ),
        m_exitPoint      ( exitP           ),
        m_entryMomentum  ( exitP - entrP   ),
        m_middleMomentum ( exitP - entrP   ),
        m_exitMomentum   ( exitP - entrP   ),
        m_radiator       ( rad  ),
        m_rich           ( rich ),
        m_errorsEntry    ( entryErrors  ),
        m_errorsMiddle   ( Rich::Rich1Gas == rad ? exitErrors : entryErrors ),
        m_errorsExit     ( exitErrors   ),
        m_avPhotonEnergy ( 0 ),
        m_rotation       ( 0 ),
        m_rotation2      ( 0 )
    {
      // set the correct magnitude of the momentum vectors
      if ( m_entryMomentum.mag2() > 0 )
      {
        m_entryMomentum  *= sqrt( entrV.mag2() / m_entryMomentum.mag2() );
      }
      if ( m_middleMomentum.mag2() > 0 )
      {
        m_middleMomentum *= ((sqrt(entrV.mag2())+sqrt(exitV.mag2()))/2) / sqrt(m_middleMomentum.mag2());
      }
      if ( m_exitMomentum.mag2() > 0 )
      {
        m_exitMomentum   *= sqrt( exitV.mag2() / m_exitMomentum.mag2() );
      }
    }

    /// Three state, rich and radiator constructor
    /// Uses supplied momentum vectors at entrance and exit points
    RichTrackSegment( const UseAllStateVectors,      ///< Tag to indentify this constructor
                      const Gaudi::XYZPoint& entrP,  ///< The entry point to the radiator
                      const Gaudi::XYZVector& entrV, ///< The momentum vector at entry to the radiator
                      const Gaudi::XYZPoint& middP,  ///< The mid point in the radiator
                      const Gaudi::XYZVector& middV, ///< The momentum vector at the radiator mid point
                      const Gaudi::XYZPoint& exitP,  ///< The exit point from the radiator
                      const Gaudi::XYZVector& exitV, ///< The momentum vector at exit from the radiator
                      const Rich::RadiatorType rad,  ///< The radiator type
                      const Rich::DetectorType rich, ///< The detector type
                      const StateErrors entryErrors  = StateErrors(), ///< The segment errors at entry
                      const StateErrors middleErrors = StateErrors(), ///< The segment errors at the mid point
                      const StateErrors exitErrors   = StateErrors()  ///< The segment errors at exit
                      )
      : m_entryPoint     ( entrP ),
        m_middlePoint    ( middP ),
        m_exitPoint      ( exitP ),
        m_entryMomentum  ( entrV ),
        m_middleMomentum ( middV ),
        m_exitMomentum   ( exitV ),
        m_radiator       ( rad   ),
        m_rich           ( rich  ),
        m_errorsEntry    ( entryErrors  ),
        m_errorsMiddle   ( middleErrors ),
        m_errorsExit     ( exitErrors   ),
        m_avPhotonEnergy ( 0 ),
        m_rotation       ( 0 ),
        m_rotation2      ( 0 ) { }

    /// Three state, rich and radiator constructor
    /// Uses supplied momentum vectors at entrance and exit
    /// Uses the vectors between the entry, middle and exit points as the track direction
    RichTrackSegment( const UseChordBetweenStates,   ///< Tag to indentify this constructor
                      const Gaudi::XYZPoint& entrP,  ///< The entry point to the radiator
                      const Gaudi::XYZVector& entrV, ///< The momentum vector at entry to the radiator
                      const Gaudi::XYZPoint& middP,  ///< The mid point in the radiator
                      const Gaudi::XYZVector& middV, ///< The momentum vector at the radiator mid point
                      const Gaudi::XYZPoint& exitP,  ///< The exit point from the radiator
                      const Gaudi::XYZVector& exitV, ///< The momentum vector at exit from the radiator
                      const Rich::RadiatorType rad,  ///< The radiator type
                      const Rich::DetectorType rich, ///< The detector type
                      const StateErrors entryErrors  = StateErrors(), ///< The segment errors at entry
                      const StateErrors middleErrors = StateErrors(), ///< The segment errors at the mid point
                      const StateErrors exitErrors   = StateErrors()  ///< The segment errors at exit
                      )
      : m_entryPoint     ( entrP ),
        m_middlePoint    ( middP ),
        m_exitPoint      ( exitP ),
        m_entryMomentum  ( middP - entrP ),
        m_middleMomentum ( exitP - entrP ),
        m_exitMomentum   ( exitP - middP ),
        m_radiator       ( rad   ),
        m_rich           ( rich  ),
        m_errorsEntry    ( entryErrors  ),
        m_errorsMiddle   ( middleErrors ),
        m_errorsExit     ( exitErrors   ),
        m_avPhotonEnergy ( 0 ),
        m_rotation       ( 0 ),
        m_rotation2      ( 0 )
    {
      // set the correct magnitude of the momentum vectors
      if ( m_entryMomentum.mag2() > 0 )
      {
        m_entryMomentum  *= sqrt( entrV.mag2() / m_entryMomentum.mag2() );
      }
      if ( m_middleMomentum.mag2() > 0 )
      {
        m_middleMomentum *= sqrt( middV.mag2() / m_middleMomentum.mag2() );
      }
      if ( m_exitMomentum.mag2() > 0 )
      {
        m_exitMomentum   *= sqrt( exitV.mag2() / m_exitMomentum.mag2() );
      }
    }

    /// Standard constructor
    RichTrackSegment()
      : m_radiator       ( Rich::InvalidRadiator ),
        m_rich           ( Rich::InvalidDetector ),
        m_avPhotonEnergy ( 0                     ),
        m_rotation       ( 0                     ),
        m_rotation2      ( 0                     ) { }

    /// Destructor
    ~RichTrackSegment( ) { cleanUpRotations(); }

    /** Update the track using a given transformation matrix and a fixed point
     *
     *  @param rotPnt Fixed point for transformation
     *  @param trans  Transformation matrix to apply to the segment
     */
    void updateState( const Gaudi::XYZPoint & rotPnt,
                      const Gaudi::Transform3D & trans );

    /** Calculates the theta and phi angles of a direction with respect to
     *  the segment direction
     *
     *  @param direction Direction to which to calculate the angles for this segment
     *  @param theta The angle between input direction and the segment
     *  @param phi   The azimuthal angle of the direction around the segment
     */
    void angleToDirection ( const Gaudi::XYZVector& direction,
                            double& theta,
                            double& phi ) const;

    /** Creates a vector at an given angle and azimuth to the track segment
     *
     *  @param theta The angle between this track segment and the created vector
     *  @param phi   The azimuthal angle of the vector to this track segment
     *
     *  @return The vector at the given theta and phi angles to this track segment
     */
    Gaudi::XYZVector vectorAtThetaPhi ( const double theta,
                                        const double phi ) const;

    /** Calculates the path lenth of a track segment.
     *  @returns The total length of the track inside the radiator
     */
    inline double pathLength() const
    {
      return sqrt((entryPoint()-middlePoint()).mag2()) + sqrt((middlePoint()-exitPoint()).mag2());
    }

    /// Returns the segment entry point to the radiator
    inline const Gaudi::XYZPoint& entryPoint() const
    {
      return m_entryPoint;
    }

    /// Returns the segment mid-point in the radiator
    inline const Gaudi::XYZPoint& middlePoint() const
    {
      return m_middlePoint;
    }

    /// Returns the segment exit point from the radiator
    inline const Gaudi::XYZPoint& exitPoint() const
    {
      return m_exitPoint;
    }

    // need to double check this is correct...
    /// Returns the z coordinate at a given fractional distance along segment
    inline double zCoordAt( const double fraction ) const
    {
      return fraction*exitPoint().z() + (1-fraction)*entryPoint().z();
    }

    /// Returns the best space point for segment at a given fractional distance along segment.
    /// Zero gives the entry point, one gives the exit point
    inline Gaudi::XYZPoint bestPoint( const double fractDist ) const
    {
      if ( zCoordAt(fractDist) < middlePoint().z() )
      {
        const double midFrac1 =
          sqrt( (entryPoint()-middlePoint()).mag2() / (entryPoint()-exitPoint()).mag2() );
        return entryPoint() + (fractDist/midFrac1)*(middlePoint()-entryPoint());
      }
      else
      {
        const double midFrac2 =
          sqrt( (middlePoint()-exitPoint()).mag2() / (entryPoint()-exitPoint()).mag2() );
        return middlePoint() + ((fractDist-midFrac2)/midFrac2)*(exitPoint()-middlePoint());
      }
    }

    /// Returns the best estimate of the average point in the radiator
    /// Equivalent to RichTrackSegment::bestPoint(0.5), but more efficient
    inline const Gaudi::XYZPoint& bestPoint() const
    {
      return m_middlePoint;
    }

    /// Returns the momentum vector at entry
    inline const Gaudi::XYZVector& entryMomentum() const
    {
      return m_entryMomentum;
    }

    // Returns the momentum vector at the mid point
    inline const Gaudi::XYZVector& middleMomentum() const
    {
      return m_middleMomentum;
    }

    /// Returns the momentum vector at exit
    inline const Gaudi::XYZVector& exitMomentum() const
    {
      return m_exitMomentum;
    }

    /// Returns the best direction for segment at a given fractional distance along segment
    inline Gaudi::XYZVector bestMomentum( const double fractDist ) const
    {
      if ( zCoordAt(fractDist) < middlePoint().z() )
      {
        const double midFrac =
          sqrt((entryPoint()-exitPoint()).mag2())*fractDist / sqrt((entryPoint()-middlePoint()).mag2());
        return entryMomentum()*(1-midFrac) + middleMomentum()*midFrac;
      }
      else
      {
        const double midFrac =
          sqrt((entryPoint()-exitPoint()).mag2())*fractDist/sqrt((middlePoint()-exitPoint()).mag2()) - 1;
        return middleMomentum()*(1-midFrac) + exitMomentum()*midFrac;
      }
    }

    /// Returns the best estimate of the average momentum vector for the entire segment
    /// Equivalent to RichTrackSegment::bestMomentum(0.5) but more efficient
    inline const Gaudi::XYZVector& bestMomentum() const
    {
      return m_middleMomentum;
    }

    /// Returns the radiator type
    inline Rich::RadiatorType radiator() const
    {
      return m_radiator;
    }

    /// Returns the detector type
    inline Rich::DetectorType rich() const
    {
      return m_rich;
    }

    /// Set the entry state
    inline void setEntryState( const Gaudi::XYZPoint& point,
                               const Gaudi::XYZVector& dir )
    {
      m_entryPoint     = point;
      m_entryMomentum  = dir;
    }

    /// Set the Middle state
    inline void setMiddleState( const Gaudi::XYZPoint& point,
                                const Gaudi::XYZVector& dir )
    {
      m_middlePoint    = point;
      m_middleMomentum = dir;
    }

    /// Set the exit state
    inline void setExitState( const Gaudi::XYZPoint& point,
                              const Gaudi::XYZVector& dir )
    {
      m_exitPoint      = point;
      m_exitMomentum   = dir;
    }

    /// Set the radiator type
    inline void setRadiator(const Rich::RadiatorType rad)
    {
      m_radiator = rad;
    }

    /// Set the rich detector
    inline void setRich( const Rich::DetectorType det )
    {
      m_rich = det;
    }

    /// Returns the entry errors
    inline const StateErrors & entryErrors() const
    {
      return m_errorsEntry;
    }

    /// Returns the middle point errors
    inline const StateErrors & middleErrors() const
    {
      return m_errorsMiddle;
    }

    /// Returns the exit errors
    inline const StateErrors & exitErrors() const
    {
      return m_errorsExit;
    }

    /// Returns the average observable photon energy
    inline double avPhotonEnergy() const
    {
      return m_avPhotonEnergy;
    }

    /// Sets the average observable photon energy
    inline void setAvPhotonEnergy( const double energy )
    {
      m_avPhotonEnergy = energy;
    }

    /// Reset segment after information update
    void reset() const;

  private:  // methods

    /// Access the rotation matrix 1
    inline const Gaudi::Rotation3D & rotationMatrix() const
    {
      if ( !m_rotation ) computeRotationMatrix();
      return *m_rotation;
    }

    /// Access the rotation matrix 2
    inline const Gaudi::Rotation3D & rotationMatrix2() const
    {
      if ( !m_rotation2 ) computeRotationMatrix2();
      return *m_rotation2;
    }

    /// Compute the rotation matrix
    void computeRotationMatrix() const;

    /// Compute the rotation matrix
    void computeRotationMatrix2() const;

  private:  // private data

    Gaudi::XYZPoint m_entryPoint;       ///< Entry point into radiator volume
    Gaudi::XYZPoint m_middlePoint;      ///< Exit point into radiator volume
    Gaudi::XYZPoint m_exitPoint;        ///< middle point in radiator volume

    Gaudi::XYZVector m_entryMomentum;   ///< Momentum vector at entry
    Gaudi::XYZVector m_middleMomentum;  ///< Momentum vector at middlePoint
    Gaudi::XYZVector m_exitMomentum;    ///< Momentum vector at exit

    Rich::RadiatorType m_radiator; ///< Rich radiator
    Rich::DetectorType m_rich;     ///< Rich detector

    StateErrors m_errorsEntry;     ///< Errors for entry state
    StateErrors m_errorsMiddle;    ///< Errors for entry state
    StateErrors m_errorsExit;      ///< Errors for entry state

    /** The average observable photon energy for this segment
     * @todo Quick fix for DC04. Need to review to if this can be done in a better way
     *       without the need for this variable.
     */
    double m_avPhotonEnergy;

    // Some variables for internal caching of information for speed

    /** Rotation matrix used to calculate the theta and phi angles between
     *  this track segment and a given direction.
     *  Created on demand as required.
     */
    mutable Gaudi::Rotation3D * m_rotation;

    /** Rotation matrix used to create vectors at a given theta and phi angle
     *  to this track segment.
     *  Created on demand as required
     */
    mutable Gaudi::Rotation3D * m_rotation2;

  };

} // end LHCb namespace

inline void LHCb::RichTrackSegment::computeRotationMatrix() const
{
  m_rotation = new Gaudi::Rotation3D( rotationMatrix2().Inverse() );
}

inline void
LHCb::RichTrackSegment::angleToDirection( const Gaudi::XYZVector & direction,
                                          double & theta,
                                          double & phi ) const
{
  // create vector in track reference frame
  const Gaudi::XYZVector rotDirection = rotationMatrix() * direction;
  // get the angles
  theta = rotDirection.theta();
  phi   = rotDirection.phi();
  // correct phi
  if ( phi < 0 ) phi += 2.0*M_PI;
}

inline Gaudi::XYZVector
LHCb::RichTrackSegment::vectorAtThetaPhi( const double theta,
                                          const double phi ) const
{
  const double sinTheta = sin(theta);
  return rotationMatrix2() * Gaudi::XYZVector( sinTheta*cos(phi),
                                               sinTheta*sin(phi),
                                               cos(theta) );
}

inline void LHCb::RichTrackSegment::reset() const
{
  cleanUpRotations();
}

/// Implement ostream << method for RichTrackSegment
inline std::ostream& operator << ( std::ostream& s,
                                   const LHCb::RichTrackSegment& segment )
{
  s << "{ " << std::endl
    << " entryPoint:\t" << segment.entryPoint() << std::endl
    << " middlePoint:\t" << segment.middlePoint() << std::endl
    << " exitPoint:\t" << segment.exitPoint() << std::endl
    << " entryMomentum:\t" << segment.entryMomentum() << std::endl
    << " middleMomentum:\t" << segment.middleMomentum() << std::endl
    << " exitMomentum:\t" << segment.exitMomentum() << std::endl
    << " radiator:\t" << Rich::text( segment.radiator() ) << std::endl
    << " rich:\t" << Rich::text( segment.rich() ) << std::endl
    << " entryErrors:\t" << segment.entryErrors() << std::endl
    << " middleErrors:\t" << segment.middleErrors() << std::endl
    << " exitErrors:\t" << segment.exitErrors() << std::endl
    << " } ";
  return s;
}

#endif // RICHUTILS_RICHTRACKSEGMENT_H
