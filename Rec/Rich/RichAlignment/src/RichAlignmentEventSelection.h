// $Id: $
#ifndef EVENTSELECTION_H
#define EVENTSELECTION_H 1

// Include files
// base class
#include "RichRecBase/RichRecTupleAlgBase.h"


// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Interfaces
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"

// Kernel
#include "RichKernel/BoostArray.h"

// boost
#include "boost/lexical_cast.hpp"
#include <boost/foreach.hpp>

/** @class EventSelection EventSelection.h
 *
 *
 *  @author Thomas Rex Hampson
 *  @date   2010-02-09
 */
namespace Rich
{
  namespace Rec
  {
    class EventSelection : public RichRecTupleAlgBase
    {

    public:

      /// Mirror number definition
      typedef unsigned int MirrorNumber;

      /// Standard constructor
      EventSelection( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~EventSelection( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution
      virtual StatusCode finalize  ();    // Algorithm finalization

    private: // data

      int m_richTemp;    ///< which rich detector to monitor
      Rich::DetectorType m_rich;

      int m_maxUsedTracks;

      // T Hampson
      // declare the arrays and other stuff
      //======================================
      
      static const unsigned int nRich1Comb = 24;
      static const unsigned int nRich2Comb = 170;

      double m_combiCountR2[nRich2Comb];
      double m_combiCountR1[nRich1Comb];

      // store the total number of mirror photons for the job/subjob
      double m_totalPhotonsR2[nRich2Comb];
      double m_totalPhotonsR1[nRich1Comb];

      //======================================

      // to avoid bias towards small angles use only a photons in the expected range
      double m_deltaThetaRange;

      // particle type when fixed
      int m_particleType;
      Rich::ParticleIDType m_pType;

      // keep totals of particle types
      std::vector<int> m_pTypes;

      // tools
      const ITrackSelector* m_trSelector;        ///< Track selector
      const ICherenkovAngle* m_ckAngle;  ///< Pointer to RichCherenkovAngle tool

    };

  }
}

#endif // EVENTSELECTION_H
