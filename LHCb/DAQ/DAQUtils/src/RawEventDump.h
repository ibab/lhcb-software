// $Id: RawEventDump.h,v 1.1 2005/10/14 12:48:50 cattanem Exp $
#ifndef TESTS_RAWEVENTDUMP_H 
#define TESTS_RAWEVENTDUMP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class RawEventDump RawEventDump.h tests/RawEventDump.h
 *  Dump a RawEvent
 *
 *  @author Markus Frank
 *  @date   2005-10-13
 */
class RawEventDump : public GaudiAlgorithm {
public: 
  /// Standard constructor
  RawEventDump( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawEventDump( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  bool acceptBank(LHCb::RawBank::BankType i);
  bool m_dump; ///< Property "DumpData". If true, full bank contents are dumped
  std::vector<std::string> m_banks; // RawBanks to be dumped  (default ALL banks)
};
#endif // TESTS_RAWEVENTDUMP_H
