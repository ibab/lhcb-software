// $Id: $
#ifndef OFFLINERATELIMITER_H 
#define OFFLINERATELIMITER_H 1

// Include files
// from Gaudi
#include "OfflineRateLimiter.h"
#include "OfflineDeterministicPrescaler.h"
#include "Kernel/IRateFromTCK.h"
/** @class OfflineRateLimiter OfflineRateLimiter.h
 *
 *  Algorithm to ``rate limit'' a line in the stripping. 
 *  It needs to calibrate to something else, i.e. an Hlt rate limited line.
 *
 * Usage:
 *
 * @verbatim
 * OfflineRateLimiter().HltLimiter = "Hlt1MBNoBiasODINFilter"
 * OfflineRateLimiter().Rate = 1.
 * @endverbatim
 *
 * Will reduce the rate of Hlt1MBNoBias to 1 Hz
 *
 *  @author Patrick Koppenburg
 *  @date   2011-08-05
 */
class OfflineRateLimiter : public OfflineDeterministicPrescaler{
public: 
  /// Standard constructor
  OfflineRateLimiter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OfflineRateLimiter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  void handle();

  IRateFromTCK* m_tckReader ; ///< Property Config Service
  std::string m_hltLimiter ;  ///< Line wrt to which one wants to scale
  double m_rate ;             ///< Target rate
  unsigned int m_tck ;        ///< Last TCK seen
  
};
#endif // OFFLINERATELIMITER_H
