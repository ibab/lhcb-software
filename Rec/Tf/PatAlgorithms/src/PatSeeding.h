// $Id: PatSeeding.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATSEEDING_H
#define PATSEEDING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "TfKernel/TStationHitManager.h"

#include "PatSeedTool.h"
#include "PatSeedingTool.h"
#include "PatSeedTrack.h"
#include "PatKernel/PatForwardHit.h"
#include "Event/Track.h"
#include "TfKernel/RegionID.h"

  /** @class PatSeeding PatSeeding.h
   *
   *  Pat Seeding algorithm.
   *
   *  @author Olivier Callot
   *  @date   2006-10-13 Initial version
   *  @date   2007-08-20 Update for a-team framework 
   */

  class PatSeeding : public GaudiAlgorithm
  {

  public:

    /// Standard constructor
    PatSeeding( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PatSeeding( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
    template <class SELECTOR >
      void prepareHitsInWindow(const SELECTOR& selector) const;


  private:

    double m_tracking;
    double m_converting;
    std::string      m_outputTracksName;
    IPatSeedingTool*   m_seedingTool;

  };

#endif // PATSEEDING_H
