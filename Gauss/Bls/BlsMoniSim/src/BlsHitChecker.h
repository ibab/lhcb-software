// $Id: BlsHitChecker.h,v 1.1.1.2 2010-03-10 17:38:47 vtalanov Exp $
#ifndef BLSHITCHECKER_H 
#define BLSHITCHECKER_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/MCHit.h"

// from STL
#include <map>

/** @class BlsHitChecker BlsHitChecker.h
 *  
 *
 *  @author Vadim Talanov
 *  @date   2010-02-06
 */

class BlsHitChecker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  BlsHitChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BlsHitChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  LHCb::MCHits* m_blsMCHits;
    std::string m_blsHitsLocation;
           bool m_blsAOn;
           bool m_blsCOn;
    std::string m_blsHTitlePrefix;

         double m_blsHEntryXMin;
         double m_blsHEntryXMax;
  unsigned long m_blsHEntryXNbins;
         double m_blsHEntryYMin;
         double m_blsHEntryYMax;
  unsigned long m_blsHEntryYNbins;
         double m_blsHEntryZMin;
         double m_blsHEntryZMax;
  unsigned long m_blsHEntryZNbins;
         double m_blsHEntryTimeOffset;
         double m_blsHEntryTimeMin;
         double m_blsHEntryTimeMax;
  unsigned long m_blsHEntryTimeNbins;
         double m_blsHTrackEnDepMin;
         double m_blsHTrackEnDepMax;
  unsigned long m_blsHTrackEnDepNbins;
         double m_blsHTrackLengthMin;
         double m_blsHTrackLengthMax;
  unsigned long m_blsHTrackLengthNbins;
         double m_blsHEventNumTracksMin;
         double m_blsHEventNumTracksMax;
  unsigned long m_blsHEventNumTracksNbins;
         double m_blsHEventNumMin;
         double m_blsHEventNumMax;
  unsigned long m_blsHEventNumNbins;

  unsigned long m_blsHits;
  unsigned long m_blsTracks;

   typedef std::multimap < const LHCb::MCParticle*,
                            LHCb::MCHit* >
                            t_blsMCParticle2MCHitMultimap;

};
#endif // BLSHITCHECKER_H
