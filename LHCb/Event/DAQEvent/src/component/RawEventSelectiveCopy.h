// $Id: RawEventSelectiveCopy.h,v 1.1 2009-06-22 15:12:04 tskwarni Exp $
#ifndef TESTS_RAWEVENTSELECTIVECOPY_H 
#define TESTS_RAWEVENTSELECTIVECOPY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class RawEventSelectiveCopy RawEventSelectiveCopy.h
 *  Copies selected RawBanks to RawEvent at a new TES location. 
 *
 *  @author Tomasz Skwarnicki
 *  @date   2009-06-22
 */
class RawEventSelectiveCopy : public GaudiAlgorithm {
public: 
  /// Standard constructor
  RawEventSelectiveCopy( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawEventSelectiveCopy( );     ///< Destructor
  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute();         ///< Algorithm execution

protected:

private:

  StringArrayProperty m_banksToCopy;
  StringProperty      m_inputLocation;
  StringProperty      m_outputLocation;
  
  std::vector<LHCb::RawBank::BankType> m_bankTypes;

};


#endif // TESTS_RAWEVENTSELECTIVECOPY_H
