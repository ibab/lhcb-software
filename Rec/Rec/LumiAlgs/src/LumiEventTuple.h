// $Id: $
#ifndef LUMIEVENTTUPLE_H
#define LUMIEVENTTUPLE_H 1

#include <GaudiAlg/GaudiTupleAlg.h>

/** @class LumiEventTuple LumiEventTuple.h
 *
 *  \brief Write counters and ODIN data for lumi events to a tuple
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

  std::vector<int> m_keys;
  std::vector<int> m_values;

  // properties
  std::string m_inputLocation;
  std::string m_ntupleName;
  bool m_arrayColumns;

};
#endif // LUMIEVENTTUPLE_H
