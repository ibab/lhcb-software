
//-----------------------------------------------------------------------------
/** @file RichENNRingFinderAlg.h
 *
 *  Header file for ENN Ring finder algorithms
 *
 *  CVS Log :-
 *  $Id: RichENNRingFinderAlg.h,v 1.2 2009-05-22 15:33:23 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/05/2009
 */
//-----------------------------------------------------------------------------

#ifndef RICHENNRINGFINDER_RichENNRingFinderAlg_H
#define RICHENNRINGFINDER_RichENNRingFinderAlg_H 1

// STD
#include <sstream>
#include <fstream>
#include <map>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// RichKernel
#include "RichKernel/RichHPDIdentifier.h"

// RichRecBase
#include "RichRecBase/RichRecHistoAlgBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// ENN ring finder
#include "ENNRingFinder.h"

namespace Rich
{
  namespace Rec
  {
    namespace ENNRingFinder
    {

      //-----------------------------------------------------------------------
      /** @class AlgBase RichENNRingFinderAlg.h
       *
       *  Trackless ring finder using a ENN method
       *
       *  See http://www.gsi.de/documents/DOC-2005-Sep-83-1.pdf for details
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   21/05/2009
       */
      //-----------------------------------------------------------------------

      template < class FINDER >
      class AlgBase : public RichRecHistoAlgBase
      {

      public:

        /// Standard constructor
        AlgBase( const std::string& name,
                 ISvcLocator* pSvcLocator,
                 const Rich::DetectorType rich,
                 const Rich::Side panel,
                 const Rich::RadiatorType rad );

        virtual ~AlgBase( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution
        virtual StatusCode finalize  ();    ///< Algorithm finalization

      protected:

        /// Returns the RICH Detector
        inline Rich::DetectorType rich() const { return m_rich; }

        /// Returns the RICH panel
        inline Rich::Side panel() const { return m_panel; }

        /// Returns the RICH radiator
        inline Rich::RadiatorType rad() const { return m_rad; }

      private: // methods

        /// General RICH reco initialisations
        StatusCode richInit();

        /// Run the ring finder
        StatusCode runRingFinder();

        /// Get rings at given location
        inline LHCb::RichRecRings * getRings( const std::string & location ) const
        {
          return getOrCreate<LHCb::RichRecRings,LHCb::RichRecRings>(location);
        }

        /// Adds data points to the the ring finder input
        bool addDataPoints() const;

        /// Save rings to TES
        StatusCode saveRings() const;

        /// Fill the ring points in the final reconstructed rings
        void buildRingPoints( LHCb::RichRecRing * ring,
                              const unsigned int nPoints = 100 ) const;

        /// Add references to given ring to associated pixels
        void addRingToPixels( LHCb::RichRecRing * ring ) const;

      private: // data

        // tool pointers
        const ISmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

        const Rich::DetectorType m_rich;  ///< Which RICH Detector
        const Rich::Side         m_panel; ///< Which RICH Panel
        const Rich::RadiatorType m_rad;   ///< Which RICH radiator

        /// The ENN ring finder
        FINDER * m_finder;

        /// Location of all output rings in TES
        std::string m_ringLocation;

        /// Min probability to associate a pixel to a ring
        double m_minAssProb;

        /// Maximum number of hits to try ring finding with
        unsigned int m_maxHitsEvent;

        /** Maximum number of hits per HPD to try ring finding with.
         *  HPDs which exceed this are rejected */
        unsigned int m_maxHitsHPD;

        /// data scale factor, to turn local coordinates into angles (in rad)
        double m_scaleFactor;

        /** Maximum distance from ENN ring centre of a pixel to be considered
         *  as associated to that ring */
        double m_maxPixelSep;

        /// Hit Sigma for ring finder
        double m_hitSigma;

        /// Min number of hits per ring for ring finder
        int m_minHitsPerRing;

        /// Minimum ring radius
        double m_minRingRadius;

        /// Maximum ring radius
        double m_maxRingRadius;

        /// Max Chi^2 per hit on ring
        double m_maxRingChi2;

        /// Min ring purity
        double m_minRingPurity;

      };

      /// The type of finder for RICH1
      typedef Finder Rich1Finder;

      /// The type of finder for RICH2
      typedef Finder Rich2Finder;

      /**
       * @class  Rich1TopPanel RichENNRingFinderAlg.h
       * @brief  Standalone ring finder for RICH1 top panel using ENN Chains
       *
       * @author Chris Jones   Christopher.Rob.Jones@cern.ch
       * @date   2003-12-02
       */
      class Rich1TopPanel : public AlgBase<Rich1Finder>
      {
      public:
        /// Default Constructor
        Rich1TopPanel( const std::string& name,
                       ISvcLocator* pSvcLocator )
          : AlgBase<Rich1Finder>( name, pSvcLocator,
                                  Rich::Rich1, Rich::top, Rich::Rich1Gas )
        { }
      };

      /**
       * @class  Rich1BottomPanel RichENNRingFinderAlg.h
       * @brief  Standalone ring finder for RICH1 bottom panel using ENN Chains
       *
       * @author Chris Jones   Christopher.Rob.Jones@cern.ch
       * @date   2003-12-02
       */
      class Rich1BottomPanel : public AlgBase<Rich1Finder>
      {
      public:
        /// Default Constructor
        Rich1BottomPanel( const std::string& name,
                          ISvcLocator* pSvcLocator )
          : AlgBase<Rich1Finder>( name, pSvcLocator,
                                  Rich::Rich1, Rich::bottom, Rich::Rich1Gas )
        { }
      };

      /**
       * @class  Rich2RightPanel RichENNRingFinderAlg.h
       * @brief  Standalone ring finder for RICH2 right panel using ENN
       *
       * @author Chris Jones   Christopher.Rob.Jones@cern.ch
       * @date   2003-12-02
       */
      class Rich2RightPanel : public AlgBase<Rich2Finder>
      {
      public:
        /// Default Constructor
        Rich2RightPanel( const std::string& name,
                         ISvcLocator* pSvcLocator )
          : AlgBase<Rich2Finder>( name, pSvcLocator,
                                  Rich::Rich2, Rich::right, Rich::Rich2Gas )
        { }
      };

      /**
       * @class  Rich2LeftPanel RichENNRingFinderAlg.h
       * @brief  Standalone ring finder for RICH2 left panel using ENN
       *
       * @author Chris Jones   Christopher.Rob.Jones@cern.ch
       * @date   2003-12-02
       */
      class Rich2LeftPanel : public AlgBase<Rich2Finder>
      {
      public:
        /// Default Constructor
        Rich2LeftPanel( const std::string& name,
                        ISvcLocator* pSvcLocator )
          : AlgBase<Rich2Finder>( name, pSvcLocator,
                                  Rich::Rich2, Rich::left, Rich::Rich2Gas )
        { }
      };

    }
  }
}

#endif // RICHENNRINGFINDER_RichENNRingFinderAlg_H
