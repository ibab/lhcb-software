// $Id: RawEventCombiner.h,v 1.1 2009/06/22 15:12:04 tskwarni Exp $
#ifndef LHCB_RAWEVENTSIMPLECOMBINER_H 
#define LHCB_RAWEVENTSIMPLECOMBINER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class RawEventSimpleCombiner RawEventSimpleCombiner.h
 *  Combines disparate raw events into one new location
 *  Based on RawEventSelectiveCopy from Tomasz
 *  Use if the split locations hold no raw event bank duplicates
 *
 *  @author Rob Lambert
 *  @date   2013-04-03
 */
class RawEventSimpleCombiner : public GaudiAlgorithm {
public: 
  /// Standard constructor
  RawEventSimpleCombiner( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawEventSimpleCombiner( );     ///< Destructor
  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute();         ///< Algorithm execution

protected:

private:

  std::vector<std::string> m_banksToCopy; //which banks to recombine, default ALL
  std::vector<std::string> m_inputLocations; //get the banks from where
  std::string              m_outputLocation; //where to put them, DEFAULT /Event/RawEvent
  
  std::vector<LHCb::RawBank::BankType> m_bankTypes;

};


#endif // TESTS_RAWEVENTSIMPLECOMBINER_H
