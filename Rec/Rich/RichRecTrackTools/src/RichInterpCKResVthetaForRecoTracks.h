
//---------------------------------------------------------------------------------------
/** @file RichInterpCKResVthetaForRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::InterpCKResVthetaForRecoTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHINTERPCKRESVTHETAFORRECOTRACKS_H
#define RICHRECTOOLS_RICHINTERPCKRESVTHETAFORRECOTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// from Gaudi
#include "GaudiKernel/MsgStream.h"

// Event model
#include "Event/RichRecSegment.h"

// RichKernel
#include "RichKernel/RichMap.h"

// RichDet
#include "RichDet/Rich1DTabFunc.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------------
    /** @class InterpCKResVthetaForRecoTracks RichInterpCKResVthetaForRecoTracks.h
     *
     *  Tool to calculate the Cherenkov angle resolution. This implementation is
     *  for reconstructed Tracks and uses an interpolation of the error against the
     *  cherenkov angle theta
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //---------------------------------------------------------------------------------------

    class InterpCKResVthetaForRecoTracks : public Rich::Rec::ToolBase,
                                           virtual public ICherenkovResolution
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      InterpCKResVthetaForRecoTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

      /// Destructor
      virtual ~InterpCKResVthetaForRecoTracks() {};

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Photon resolution
      double ckThetaResolution( LHCb::RichRecSegment * segment,
                                const Rich::ParticleIDType id = Rich::Pion ) const;

    private: //methods

      /// Create and return on demand the required interpolator
      const Rich1DTabFunc * getInterp( const Rich::RadiatorType rad,
                                       const Rich::Rec::Track::Type track ) const;

    private:  // Private data

      /// Pointer to RichCherenkovAngle interface
      const ICherenkovAngle * m_ckAngle = nullptr;

      /// type for map of interpolators
      typedef std::pair<const Rich::RadiatorType, const Rich::Rec::Track::Type> InterKey;
      typedef Rich::Map< InterKey, const Rich1DTabFunc * > Interps;
      typedef Rich::Map< InterKey, std::vector<std::pair<double,double> > > InterJoData;

      /// Interpolators
      mutable Interps m_ckRes;

      /// Job opts data
      mutable InterJoData m_joData;

    };

  }
}

#endif // RICHRECTOOLS_RICHINTERPCKRESVTHETAFORRECOTRACKS_H
