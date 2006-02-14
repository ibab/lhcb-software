// $Id: LbAppInit.h,v 1.4 2006-02-14 15:25:40 cattanem Exp $
#ifndef LBAPPINIT_H 
#define LBAPPINIT_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class IRndmEngine;
class IRndmGenSvc;

/** @class LbAppInit LbAppInit.h
 *  
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-21
 */
class LbAppInit : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LbAppInit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LbAppInit( ); ///< Destructor

  virtual StatusCode initialize();     ///< Algorithm initialization
  virtual StatusCode execute   ();     ///< Algorithm execution
  virtual StatusCode finalize  ();     ///< Algorithm finalization

protected:
  // Return number of events processed
  int  eventCounter()
  { 
    return m_eventCounter; 
  }
  // Return name of application being run
  const std::string& appName() 
  {
    return m_appName;
  }
  // Return version of application being run
  const std::string& appVersion() 
  {
    return m_appVersion;
  }

  /** Print the run number, event number and optional vector of seeds
   *  @param[in] evt event number
   *  @param[in] run run number
   *  @param[in] seeds (optional) vector of seeds
   */
  void printEventRun( longlong evt, int run, std::vector<long int> *seeds = 0 );

  /** Initialize the random number engine with the given seeds
   *  @param[in] seeds Vector of seeds
   *  @return    StatusCode
   */
  StatusCode initRndm( std::vector<long int>& seeds );

  /** Get a vector of seeds. First three seeds are built from seed1 and seed2
   *  Last seed is hash including seed1, seed2 and name()
   *  If m_singleSeed is true, only last seed is returned 
   *  @param[in] seed1 First  seed (typically run number)
   *  @param[in] seed2 Second seed (typically event number)
   *  @return    vector of seeds
   */
  std::vector<long int> getSeeds( unsigned int seed1, ulonglong seed2 );

private:
  /// Property to skip some random numbers (default is zero)
  int  m_skipFactor;

  /// Property to use only one seed (default is false)
  bool m_singleSeed;

  // Member data
  IRndmEngine*  m_engine;       ///< Pointer to random number engine
  IRndmGenSvc*  m_randSvc;      ///< Pointer to random number service
  int  m_eventCounter;          ///< Number of events processed
  int  m_eventMax;     ///< Number of events requested (ApplicationMgr.EvtMax)
  std::string   m_appName;      ///< Application Name
  std::string   m_appVersion;   ///< Application Version
};

#endif // LBAPPINIT_H
