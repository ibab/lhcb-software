// $Id: TrackEffChecker.h,v 1.2 2007-07-05 16:12:31 mneedham Exp $
#ifndef TRACKEFFCHECKER_H
#define TRACKEFFCHECKER_H 1
 
// Include files
 
// from Gaudi
#include "TrackCheckerBase.h"

#include "Event/Track.h"

namespace LHCb{
  class MCParticle;
}

/** @class TrackEffChecker TrackEffChecker.h "TrackCheckers/TrackEffChecker"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackEffChecker : public TrackCheckerBase {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackEffChecker( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackEffChecker();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm finalize */
  virtual StatusCode finalize();
  
 private:

  void ghostInfo(const LHCb::Tracks* tracks);

  void effInfo();

  void plots(const std::string& type, 
             const LHCb::Track* track ) const;

  void plots(const std::string& type, 
             const LHCb::MCParticle* part) const;

  double weightedMeasurementSum(const LHCb::Track* aTrack) const;

};

#endif // TRACKEFFCHECKER_H
