
//-----------------------------------------------------------------------------
/** @file RichIsolatedTrackTool.h
 *
 *  Header file for tool : Rich::Rec::IsolatedTrackTool
 *
 *  @author Susan Haines  Susan.Carol.Haines@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/02/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichIsolatedTrackTool_H
#define RICHRECTRACKTOOLS_RichIsolatedTrackTool_H 1

#include <cmath>

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichCherenkovResolution.h"

//RICH kernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichMap.h"

// Event
#include "Event/RichRecSegment.h"
#include "Event/RichRecTrack.h"
#include "Event/Track.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    //-----------------------------------------------------------------------------
    /** @class IsolatedTrackTool RichIsolatedTrackTool.h
     *
     *  Utility tool which finds isolated RICH tracks.
     *
     *  @author Susan Haines
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/02/2008
     */
    //-----------------------------------------------------------------------------

    class IsolatedTrackTool : public Rich::Rec::ToolBase,
                              virtual public IIsolatedTrack
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      IsolatedTrackTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~IsolatedTrackTool(){}

      virtual StatusCode initialize();  ///< Initialization

    public: // methods (and doxygen comments) inherited from public interface

      // Is this segment isolated
      virtual bool isIsolated( const LHCb::RichRecSegment * segment,
                               const Rich::ParticleIDType pid ) const;

      // Is this segment isolated
      virtual bool isIsolated( const LHCb::RichRecSegment * segment ) const;

      // Is this track isolated
      virtual bool isIsolated( const LHCb::RichRecTrack * track,
                               const Rich::ParticleIDType pid ) const;

      // Is this track isolated
      virtual bool isIsolated( const LHCb::RichRecTrack * track ) const;

      // Is this track isolated
      virtual bool isIsolated( const LHCb::Track * track,
                               const Rich::ParticleIDType pid ) const;

      // Is this track isolated
      virtual bool isIsolated( const LHCb::Track * track ) const;

    private:

      /** Test the given cut, and keep a summary tally of the result
       *  @param desc   Cut description
       *  @param rad    RICH radiator
       *  @param result Was the cut passed or failed
       *  @return result
       */
      inline bool testCut( const std::string & desc, 
                           const Rich::RadiatorType rad,
                           const bool result ) const
      {
        if ( !m_abortEarly )
        {
          counter(Rich::text(rad)+" '"+desc+"'") += ( result ? 1.0 : 0.0 );
        }
        return result;
      }

    private:

      //pointers to tool instances
      mutable const ICherenkovAngle * m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
      const IGeomTool * m_geomTool;               ///< Pointer to geometry tool
      const Rich::Rec::IGeomEff * m_geomEffic;    ///< Pointer to RichGeomEff tool

      bool m_abortEarly; ///< Reject tracks as soon as possible

      std::vector<double> m_sizemomcut; ///< Minimum momentum
      std::vector<double> m_sizegeocut; ///< Minimum geom. eff.
      std::vector<double> m_sizesepcut; ///< Minimum seperation between segment centres
      std::vector<double> m_sizeXexit;  ///< Panel boundary X cut
      std::vector<double> m_sizeYexit;  ///< Panel boundary Y cut
      std::vector<double> m_sizephotonassoccut; ///< Fraction of multiple associated pixel(photons) cut
      std::vector<double> m_sizeringwidth; ///< width of ring (in radians) around expect ring CK theta
      std::vector<double> m_sizephicut;    ///< Maximum fraction of photons in each phi region
      std::vector<unsigned int> m_nPhiRegions; ///< Number of phi regions to use
      std::vector<double> m_sizehitregioncut; ///< Maximum fraction of hits outside CK ring
      std::vector<unsigned int> m_minSegPhotons; ///< Minimum number of photons reconstructed for each segment

      std::vector<double> m_maxROI;     ///< Max hit radius of interest around track centres
      std::vector<double> m_ckThetaMax; ///< Scaling parameter - Max CK theta point
      std::vector<double> m_sepGMax;    ///< Scaling parameter - Max separation point
      std::vector<double> m_scale;      ///< Internal cached parameter for speed

    };

  }
}

#endif // RICHRECTRACKTOOLS_RichIsolatedTrackTool_H
