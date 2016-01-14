
//---------------------------------------------------------------------------
/** @file RichRecBackgroundEstiAvHPD.h
 *
 *  Header file for algorithm class : Rich::Rec::BackgroundEstiAvHPD
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichRecBackgroundEstiAvHPD_H
#define RICHRECTOOLS_RichRecBackgroundEstiAvHPD_H 1

// Base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecStatus.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichRecBase/IRichPixelBackgroundEsti.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGeomEff.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class BackgroundEstiAvHPD RichRecBackgroundEstiAvHPD.h
     *
     *  Background estimation tool for RICH HPDs
     *
     *  Compares the expected signal yield in each HPD, based on the observed tracks
     *  and current set of mass hypotheses for those tracks, to the total signal in
     *  each HPD. Each pixel in that HPD is then assigned the same background value.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class BackgroundEstiAvHPD : public Rich::Rec::ToolBase,
                                virtual public IPixelBackgroundEsti
    {

    public:

      /// Standard constructor
      BackgroundEstiAvHPD( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

      virtual ~BackgroundEstiAvHPD();   ///< Destructor

      virtual StatusCode initialize();  // initialization

    public:

      // Compute the pixel background estimates using all tracks
      virtual void computeBackgrounds( ) const;

      // Compute the pixel background estimates using the given tracks only
      virtual void computeBackgrounds( const LHCb::RichRecTrack::Vector & tracks ) const;

      // Compute the pixel background estimates using the given track only
      virtual void computeBackgrounds( const LHCb::RichRecTrack * track ) const;

    protected: // definitions

      // working maps
      typedef Rich::Map<LHCb::RichSmartID,double> PDsignals;
      typedef std::vector< PDsignals >            RichDataMap;

    protected: // methods

      /// General Rich Init.
      void richInit() const;

      /// Fill the observed signals map
      void fillObservedSignalMap() const;

      /// Fill the expected signals map
      void fillExpectedSignalMap( const LHCb::RichRecTrack::Vector & tracks ) const;

      /// Fill the expected signals map
      virtual void fillExpectedSignalMap( const LHCb::RichRecTrack * track ) const;

      /// Fill the expected signals map
      void fillExpectedSignalMap() const;

      /// Compute the overall RICH1/RICH2 backgrounds
      virtual void overallRICHBackgrounds() const;

      /// Compute the background term for each pixel
      virtual void pixelBackgrounds() const;

    private: // data members

      /// Pointers to expected track signal tool
      const IExpectedTrackSignal * m_tkSignal = nullptr;

      /// Geometrical efficiency tool
      const IGeomEff * m_geomEff = nullptr;

      /// Maximum number of iterations in background normalisation
      int m_maxBkgIterations;

    protected: // data

      /// Number of pixels per HPD without cathode acceptance
      double m_nPixelsPerPD;

      /// The observed signals in each HPD in each RICH
      mutable RichDataMap m_obsPDsignals;

      /// The expected signals in each HPD in each RICH
      mutable RichDataMap m_expPDsignals;

      /// The deduced background in each HPD in each RICH
      mutable RichDataMap m_expPDbkg;

      /** Minimum pixel background value. 
       *  Any value below this will be set to this value */
      double m_minPixBkg;

      /** Ignore the expected signal when computing the background terms.
          Effectively, will assume all observed hists are background */
      bool m_ignoreExpSignal;

      /** Min HPD background level for setting background levels */
      double m_minHPDbckForInc;

    };

  }
}

#endif // RICHRECTOOLS_RichRecBackgroundEstiAvHPD_H
