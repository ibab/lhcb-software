
//---------------------------------------------------------------------------
/** @file RichRecBackgroundEstiAvHPD.h
 *
 *  Header file for algorithm class : Rich::Rec::BackgroundEstiAvHPD
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEstiAvHPD.h,v 1.1 2007-03-09 22:48:41 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichRecBackgroundEstiAvHPD_H
#define RICHRECALGORITHMS_RichRecBackgroundEstiAvHPD_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class BackgroundEstiAvHPD RichRecBackgroundEstiAvHPD.h
     *
     *  Background estimation algorithm for RICH HPDs
     *
     *  Compares the expected signal yield in each HPD, based on the observed tracks
     *  and current set of mass hypotheses for those tracks, to the total signal in
     *  each HPD. Each pixel in that PD is then assigned the same background value.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class BackgroundEstiAvHPD : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      BackgroundEstiAvHPD( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~BackgroundEstiAvHPD();   ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    protected: // definitions

      // working maps
      typedef Rich::Map<LHCb::RichSmartID,double> PDsignals;
      typedef std::vector< PDsignals >            RichDataMap;

    protected: // methods

      /// General Rich Init.
      virtual StatusCode richInit();

      /// Fill the observed signals map
      virtual StatusCode fillObservedSignalMap();

      /// Fill the expected signals map
      virtual StatusCode fillExpectedSignalMap();

      /// Compute the overall RICH1/RICH2 backgrounds
      virtual StatusCode overallRICHBackgrounds();

      /// Compute the background term for each pixel
      virtual StatusCode pixelBackgrounds();

    private: // data members

      /// Pointers to expected track signal tool
      const IExpectedTrackSignal * m_tkSignal;

      /// Maximum number of iterations in background normalisation
      int m_maxBkgIterations;

    protected: // data

      /// Number of pixels per HPD without cathode acceptance
      double m_nPixelsPerPD;

      /// The observed signals in each HPD in each RICH
      RichDataMap m_obsPDsignals;

      /// The expected signals in each HPD in each RICH
      RichDataMap m_expPDsignals;

      /// The deduced background in each HPD in each RICH
      RichDataMap m_expPDbkg;

    };

  }
}

#endif // RICHRECALGORITHMS_RichRecBackgroundEstiAvHPD_H
