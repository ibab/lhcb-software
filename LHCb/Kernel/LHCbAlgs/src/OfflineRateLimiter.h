#ifndef OFFLINERATELIMITER_H 
#define OFFLINERATELIMITER_H 1

// Include files
// from Gaudi
#include "OfflineDeterministicPrescaler.h"
#include "Kernel/IRateFromTCK.h"
#include "RateFromCondDB.h"
/** @class OfflineRateLimiter OfflineRateLimiter.h
 *
 *  Algorithm to ``rate limit'' a line in the stripping. 
 *  It needs to calibrate to something else, i.e. an Hlt rate limited line.
 *
 * Usage:
 *
 * @verbatim
 * OfflineRateLimiter().HltLimiter = "Hlt1L0AnyRateLimitedPostScaler"
 * OfflineRateLimiter().Rate = 1. # Hz
 * @endverbatim
 *
 * Will reduce the rate of Hlt1L0AnyRateLimited to 1 Hz
 *
 * A special case is the NoBias in 2012 data:
 *
 * @verbatim
 * OfflineRateLimiter().UseCondition = True  
 * OfflineRateLimiter().HltLimiter = "Hlt1MBNoBiasODINFilter"
 * Offlineratelimiter().Prescaler = "Hlt1MBNoBiasPreScaler"
 * @endverbatim
 *
 * This will first try the conditions database and if not found will go to the Hlt1MBNoBiasODINFilter
 *
 *  @author Patrick Koppenburg
 *  @date   2011-08-05
 */
class OfflineRateLimiter : public OfflineDeterministicPrescaler{
public: 
  /// Standard constructor
  OfflineRateLimiter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OfflineRateLimiter( ); ///< Destructor
  virtual StatusCode execute ( ); ///< Algorithm execution

protected:

private:
  bool handleTCK();               ///< Check if TCK has changed
  void updateRateFromTCK();       ///< Check if TCK has changed, update rate
  void updatePrescaleFromTCK();   ///< Check if TCK has changed, update prescale
  void updateRateFromCondDB();    ///< Check if CondDB has changed
  void updateRate();              ///< Redo all calculations
  StatusCode initializeOnFirstEvent() ; ///< wait for event 1 to initialize

  IRateFromTCK* m_tckReader ;     ///< Property Config Service
  std::string m_hltLimiter ;      ///< Line wrt to which one wants to scale
  double m_rate ;                 ///< Target rate
  unsigned int m_tck ;            ///< Last TCK seen
  bool m_useCondDB ;              ///< Use Condition DB
  RateFromCondDB* m_condTriggerTool ; ///< Rate from CondDB
  bool m_fallback;                ///< Fallback to TCK if rate is zero (for instance in 2011 data)
  std::string m_prescaler;        ///< Name of prescaler, if any
  double m_storedRate ;           ///< Rate stored in TCK or CondDB
  double m_storedPrescale ;       ///< Prescale stored in TCK
  bool m_initialized ;            ///< has been initialized
};
#endif // OFFLINERATELIMITER_H
