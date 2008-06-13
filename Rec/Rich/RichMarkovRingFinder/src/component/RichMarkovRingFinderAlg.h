
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.h
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.h,v 1.33 2008-06-13 12:44:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

#ifndef RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H
#define RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H 1

// STD
#include <sstream>
#include <fstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// RichRecBase
#include "RichRecBase/RichRecHistoAlgBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichSmartIDTool.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// Ring Finder includes
#include "GenericRingFinder/GenericInput.h"
#include "GenericRingFinder/GenericInferrer.h"
#include "GenericRingFinder/GenericResults.h"
#include "Rich/CrudeSampler.h"
#include "Rich/NimTypeRichModel.h"
#include "Utils/MessageHandler.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------
    /** @class RichMarkovRingFinderAlg RichMarkovRingFinderAlg.h
     *
     *  Trackless ring finder using a Markov Chaion Monte Carlo method
     *
     *  Uses the "Ring Finding Library" developed by C.G.Lester
     *  (lester@hep.phy.cam.ac.uk)
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2005-08-09
     */
    //-----------------------------------------------------------------------

    class RichMarkovRingFinderAlg : public RichRecHistoAlgBase,
                                    virtual public Lester::IMessageHandler
    {

    public:

      /// Standard constructor
      RichMarkovRingFinderAlg( const std::string& name,
                               ISvcLocator* pSvcLocator,
                               const Rich::DetectorType rich,
                               const Rich::Side panel,
                               const Rich::RadiatorType rad );

      virtual ~RichMarkovRingFinderAlg( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

    public: // lester messaging

      /// Intrecepts messages from the 'Lester' code and sends them to
      /// the standard Gaudi messaging system
      virtual void lesterMessage( const Lester::MessageLevel level,
                                  const std::string & message ) const;

    protected:

      /// Returns the RICH Detector
      inline const Rich::DetectorType rich() const { return m_rich; }

      /// Returns the RICH panel
      inline const Rich::Side panel() const { return m_panel; }

      /// Returns the RICH radiator
      inline const Rich::RadiatorType rad() const { return m_rad; }

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
      bool addDataPoints( GenRingF::GenericInput & input ) const;

      /// Save rings to TES
      StatusCode saveRings( const GenRingF::GenericInput & input,
                            const GenRingF::GenericResults & output ) const;

      /// Fill the ring points in the final reconstructed rings
      void buildRingPoints( LHCb::RichRecRing * ring,
                            const unsigned int nPoints = 100 ) const;

      /// Add references to given ring to associated pixels
      void addRingToPixels( LHCb::RichRecRing * ring ) const;

      /// Create data text files for standalone ring finder
      StatusCode dumpToTextfile() const;

    private: // data

      // tool pointers
      const ICherenkovAngle * m_ckAngle;  ///< Pointer to Cherenkov angle tool
      const ISmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

      const Rich::DetectorType m_rich;  ///< Which RICH Detector
      const Rich::Side         m_panel; ///< Which RICH Panel
      const Rich::RadiatorType m_rad;   ///< Which RICH radiator

      /// Pointer to the sampler (ring finder)
      Lester::CrudeSampler * m_sampler;

      /// Location of all output rings in TES
      std::string m_ringLocation;

      /// Job option to turn on dumping of data to text files, 
      /// for standalone ring finder application
      bool m_dumpText;

      /// Min probability to associate a pixel to a ring
      double m_minAssProb;

      /// Maximum number of hits to try ring finding with
      unsigned int m_maxHitsEvent;

      /// data scale factor, to turn local coordinates into angles (in rad)
      double m_scaleFactor;

      /// Maximum distance from Markov ring centre of a pixel to be considered as associated to that ring
      double m_maxPixelSep;

    };

    /**
     * @class  Rich1TopPanelMarkovRingFinderAlg RichMarkovRingFinderAlg.h
     * @brief  Standalone ring finder for RICH1 top panel using Markov Chains
     *
     * @author Chris Jones   Christopher.Rob.Jones@cern.ch
     * @date   2003-12-02
     */
    class Rich1TopPanelMarkovRingFinderAlg : public RichMarkovRingFinderAlg
    {
    public:
      /// Default Constructor
      Rich1TopPanelMarkovRingFinderAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator )
        : RichMarkovRingFinderAlg( name, pSvcLocator,
                                   Rich::Rich1, Rich::top, Rich::Rich1Gas )
      { }
    };

    /**
     * @class  Rich1BottomPanelMarkovRingFinderAlg RichMarkovRingFinderAlg.h
     * @brief  Standalone ring finder for RICH1 bottom panel using Markov Chains
     *
     * @author Chris Jones   Christopher.Rob.Jones@cern.ch
     * @date   2003-12-02
     */
    class Rich1BottomPanelMarkovRingFinderAlg : public RichMarkovRingFinderAlg
    {
    public:
      /// Default Constructor
      Rich1BottomPanelMarkovRingFinderAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator )
        : RichMarkovRingFinderAlg( name, pSvcLocator,
                                   Rich::Rich1, Rich::bottom, Rich::Rich1Gas )
      { }
    };

    /**
     * @class  Rich2RightPanelMarkovRingFinderAlg RichMarkovRingFinderAlg.h
     * @brief  Standalone ring finder for RICH2 right panel using Markov Chains
     *
     * @author Chris Jones   Christopher.Rob.Jones@cern.ch
     * @date   2003-12-02
     */
    class Rich2RightPanelMarkovRingFinderAlg : public RichMarkovRingFinderAlg
    {
    public:
      /// Default Constructor
      Rich2RightPanelMarkovRingFinderAlg( const std::string& name,
                                          ISvcLocator* pSvcLocator )
        : RichMarkovRingFinderAlg( name, pSvcLocator,
                                   Rich::Rich2, Rich::right, Rich::Rich2Gas )
      { }
    };

    /**
     * @class  Rich2LeftPanelMarkovRingFinderAlg RichMarkovRingFinderAlg.h
     * @brief  Standalone ring finder for RICH2 left panel using Markov Chains
     *
     * @author Chris Jones   Christopher.Rob.Jones@cern.ch
     * @date   2003-12-02
     */
    class Rich2LeftPanelMarkovRingFinderAlg : public RichMarkovRingFinderAlg
    {
    public:
      /// Default Constructor
      Rich2LeftPanelMarkovRingFinderAlg( const std::string& name,
                                         ISvcLocator* pSvcLocator )
        : RichMarkovRingFinderAlg( name, pSvcLocator,
                                   Rich::Rich2, Rich::left, Rich::Rich2Gas )
      { }
    };

  }
}

#endif // RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H
