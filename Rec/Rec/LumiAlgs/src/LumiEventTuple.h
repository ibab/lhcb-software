// $Id: $
#ifndef LUMIEVENTTUPLE_H
#define LUMIEVENTTUPLE_H 1

#include <GaudiAlg/GaudiTupleAlg.h>

/** @class LumiEventTuple LumiEventTuple.h
 *
 *  \brief Write counters and ODIN data for lumi events to a tuple
 *
 *  Two behaviours exist, which are controlled by the ArrayColumns
 *  option. If false (default), each lumi counter is written in a
 *  column. If true, the keys and the counters are written in two
 *  array columns.
 *
 *  It is assumed (without any protection) that all events have the
 *  same lumi counters. This is guaranteed if events taken with 
 *  different TCKs are not mixed in the same job.
 *
 *  @author Rosen Matev
 *  @date   2015-05-05
 */
class LumiEventTuple : public GaudiTupleAlg {
public:

  LumiEventTuple(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LumiEventTuple();

  virtual StatusCode execute();

private:

  bool fillOdin(Tuples::Tuple& tuple, const LHCb::ODIN* odin);
  bool fillCounters(Tuples::Tuple& tuple, const LHCb::HltLumiSummary* summary);

  std::vector<unsigned short> m_keys;
  std::vector<unsigned short> m_values;

  // properties
  std::string m_inputLocation;
  std::string m_ntupleName;
  bool m_arrayColumns;
  bool m_onlyRandom;

};
#endif // LUMIEVENTTUPLE_H
