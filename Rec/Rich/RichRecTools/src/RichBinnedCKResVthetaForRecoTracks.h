
//-----------------------------------------------------------------------------
/** @file RichBinnedCKResVthetaForRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::BinnedCKResVthetaForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichBinnedCKResVthetaForRecoTracks.h,v 1.4 2007-02-02 10:10:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHBINNEDCKRESVTHETAFORTRSTOREDTRACKS_H
#define RICHRECTOOLS_RICHBINNEDCKRESVTHETAFORTRSTOREDTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class BinnedCKResVthetaForRecoTracks RichBinnedCKResVthetaForRecoTracks.h
     *
     *  Tool to calculate the Cherenkov angle resolution. This version is
     *  for reconstructed Tracks and implements an approach that uses simple bins in the
     *  cherenkov angle theta space and also varies according to the track algorithm type.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class BinnedCKResVthetaForRecoTracks : public Rich::Rec::ToolBase,
                                           virtual public ICherenkovResolution
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      BinnedCKResVthetaForRecoTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

      /// Destructor
      virtual ~BinnedCKResVthetaForRecoTracks() {};

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Photon resolution
      double ckThetaResolution( LHCb::RichRecSegment * segment,
                                const Rich::ParticleIDType id = Rich::Pion ) const;

    private: // methods

      // Photon resolution
      bool ckThetaResolution_Imp( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType id,
                                  double & res ) const;

    private: // Private data

      /// Pointer to RichCherenkovAngle interface
      const ICherenkovAngle * m_ckAngle;

      typedef std::vector<double> BinData;
      /// The averged resolutions in each bin, for each track type
      BinData m_theerr[Rich::NRadiatorTypes][Rich::Rec::Track::NTrTypes];

      typedef std::vector<std::pair<double,double> > BinEdges;
      /// The boundaries for the resolution bins
      BinEdges m_binEdges[Rich::NRadiatorTypes];

      /// make sure all hypo resolutions are the same
      bool m_normalise;

    };

  }
}

#endif // RICHRECTOOLS_RICHBINNEDCKRESVTHETAFORTRSTOREDTRACKS_H
