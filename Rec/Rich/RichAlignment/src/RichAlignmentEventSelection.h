// $Id: $
#ifndef RICHALIGNMENT_EVENTSELECTION_H
#define RICHALIGNMENT_EVENTSELECTION_H 1

// STL
#include <map>

// base class
#include "RichRecBase/RichRecAlgBase.h"

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/RichRecStatus.h"

// Interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"

namespace Rich
{
  namespace Rec
  {

    /** @class EventSelection EventSelection.h
     *
     *  Selects events for the Rich mirror alignment
     *
     *  @author Thomas Rex Hampson
     *  @date   2010-02-09
     */

    class EventSelection : public AlgBase
    {

    public:

      /// Mirror number definition
      typedef unsigned int MirrorNumber;

      /// Standard constructor
      EventSelection( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~EventSelection( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    private: // data

      int m_richTemp;    ///< which rich detector to monitor
      Rich::DetectorType m_rich;

      int m_maxUsedTracks;

      // T Hampson
      // declare the arrays and other stuff
      //======================================

      typedef std::map<unsigned long long,unsigned long long> IDToCount;
      IDToCount m_combiMap; ///< Count photons for each mirror pair, current event
      IDToCount m_totalMap; ///< Count photons for each mirror pair, overall

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

#endif // RICHALIGNMENT_EVENTSELECTION_H
