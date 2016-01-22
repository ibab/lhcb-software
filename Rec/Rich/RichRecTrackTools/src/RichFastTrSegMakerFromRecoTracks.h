
//-----------------------------------------------------------------------------
/** @file RichFastTrSegMakerFromRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::FastTrSegMakerFromRecoTracks
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   23/08/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichFastTrSegMakerFromRecoTracks_H
#define RICHRECTRACKTOOLS_RichFastTrSegMakerFromRecoTracks_H 1

// base class
#include "RichBaseTrSegMakerFromRecoTracks.h"

// RichKernel
#include "RichKernel/RichTrackSegment.h"
#include "RichKernel/IRichRayTracing.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class FastTrSegMakerFromRecoTracks RichFastTrSegMakerFromRecoTracks.h
     *
     *  Tool to create RichTrackSegments from Tracks.
     *  Uses a course but fast description of the radiator media to
     *  find the segment entry and exit state information.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   23/08/2004
     *
     *  @todo Derive simple radiator description from RICH detector elements
     */
    //-----------------------------------------------------------------------------

    class FastTrSegMakerFromRecoTracks : public BaseTrSegMakerFromRecoTracks
    {

    public: // Methods for Gaudi Framework

      /// Standard Constructor
      FastTrSegMakerFromRecoTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

      /// Standard Destructor
      virtual ~FastTrSegMakerFromRecoTracks( );

      // Initialization of the tool after creation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Create RichTrackSegments for a given tracking object
      int constructSegments( const ContainedObject* obj,
                             std::vector<LHCb::RichTrackSegment*>& segments ) const;

    private: // methods

      /// Locates the correct state for a given radiator
      const LHCb::State * stateAt( const LHCb::Track * track,
                                   const Rich::RadiatorType rad ) const;

      /** Computes the best directon and states for a given radiator
       *
       *  @param track   Pointer to a TrgTrack object
       *  @param rad     The radiator type
       *  @param states  Vector for the entry and exit states
       *
       *  @return If appropriate states where found
       */
      bool stateInfo( const LHCb::Track * track,
                      const Rich::RadiatorType rad,
                      std::vector<const LHCb::State*> & states ) const;

      /** Checks the state against the radiator (x,y) boundary conditions
       *
       *  @param point The position to check
       *  @param rad   The radiator volume to check against
       *
       *  @return Boolean indicating if the pooint passed(true) or failed(false) the boundary checks
       */
      bool checkBoundaries( const Gaudi::XYZPoint & point,
                            const Rich::RadiatorType rad ) const;

    private: // data

      /// Ray tracing tool
      const IRayTracing* m_rayTracing = nullptr;

      /// Nominal z positions of states at RICHes
      std::vector<double> m_nomZstates;

      /// Allowable tolerance on state z positions
      std::vector<double> m_zTolerance;

      // sanity checks on state information
      std::vector<double> m_minStateDiff;

      /// Radiator entry planes
      std::vector<Gaudi::Plane3D> m_entryPlanes;

      /// Radiator exit planes
      std::vector<Gaudi::Plane3D> m_exitPlanes;

      /// Radiator entry z positions
      std::vector<double> m_entryZ;

      /// Radiator exit z positions
      std::vector<double> m_exitZ;

      /// Maximum extent of the radiators in x
      std::vector<double> m_maxX;

      /// maximum extent of the radiators in y
      std::vector<double> m_maxY;

      /// Square of the radius of the hole in the radiator around the beampipe
      std::vector<double> m_minR2;

      typedef std::vector< Rich::RadiatorType > Radiators;
      Radiators m_rads; ///< Vector of active radiators

      /// typedef of array of DeRichRadiators
      typedef std::vector<const DeRichRadiator *> DeRadiators;
      /// Array of radiators
      DeRadiators m_deRads;

      /// Job option to define type of track segments to create
      std::string m_trSegTypeJO;

      /// Type of track segments to create
      LHCb::RichTrackSegment::SegmentType m_trSegType;

    };

    inline bool
    FastTrSegMakerFromRecoTracks::checkBoundaries( const Gaudi::XYZPoint & point,
                                                   const Rich::RadiatorType rad ) const
    {
      return ( fabs(point.x()) < m_maxX[rad] &&
               fabs(point.y()) < m_maxY[rad] &&
               (m_minR2[rad]<0.01 || (point.x()*point.x() + point.y()*point.y()) > m_minR2[rad])
               );
    }

  }
}

#endif // RICHRECTRACKTOOLS_RichFastTrSegMakerFromRecoTracks_H
