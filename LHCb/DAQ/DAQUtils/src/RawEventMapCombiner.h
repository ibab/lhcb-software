// $Id: RawEventCombiner.h,v 1.1 2009/06/22 15:12:04 tskwarni Exp $
#ifndef LHCB_RAWEVENTMAPCOMBINER_H 
#define LHCB_RAWEVENTMAPCOMBINER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class RawEventMapCombiner RawEventMapCombiner.h
 *  Combines disparate raw events into one new location
 *  Based on RawEventSelectiveCopy from Tomasz
 *  Use if the split locations possibly hold duplicates of certain raw banks
 *
 *  @author Rob Lambert
 *  @date   2013-04-03
 */
class RawEventMapCombiner : public GaudiAlgorithm {
public: 
  /// Standard constructor
  RawEventMapCombiner( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawEventMapCombiner( );     ///< Destructor
  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute();         ///< Algorithm execution

protected:

private:

  std::map<std::string, std::string> m_banksToCopy; //which banks to recombine, from where, (bank, location)
  std::string              m_outputLocation; //where to put them, DEFAULT /Event/RawEvent
  
  std::map<LHCb::RawBank::BankType, std::string> m_bankTypes; //translation of m_banksToCopy

};


#endif // LHCB_RAWEVENTMAPCOMBINER_H
