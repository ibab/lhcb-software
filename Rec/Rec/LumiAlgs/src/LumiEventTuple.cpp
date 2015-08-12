#include <boost/numeric/conversion/cast.hpp>
#include <GaudiAlg/Tuple.h>
#include <GaudiAlg/TupleObj.h>
#include <Event/ODIN.h>
#include <Event/LumiCounters.h>
#include <Event/LumiMethods.h>
#include <Event/HltLumiSummary.h>
#include "LumiEventTuple.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiEventTuple
//
// 2015-05-05 Rosen Matev
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( LumiEventTuple )

LumiEventTuple::LumiEventTuple(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty("HltLumiSummaryLocation",
    m_inputLocation = LHCb::HltLumiSummaryLocation::Default);
  declareProperty("ArrayColumns", m_arrayColumns = false);
  declareProperty("NTupleName", m_ntupleName = "ntuple");
}

LumiEventTuple::~LumiEventTuple() {
}

StatusCode LumiEventTuple::execute() {
  auto* summary = get<LHCb::HltLumiSummary>(m_inputLocation);
  // skip event if not randomly triggered
  unsigned int random = summary->info(LHCb::LumiCounters::Random, -1);
  if (random != LHCb::LumiMethods::RandomMethod) return StatusCode::SUCCESS;

  auto* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);

  auto tuple = nTuple(m_ntupleName);

  bool test = true;
  test &= fillOdin(tuple, odin);
  test &= fillCounters(tuple, summary);
  test &= tuple->write();


  if (!test) {
    error() << "Failed filling/writing tuple" << endmsg;
  }
  return StatusCode(test);
}

bool LumiEventTuple::fillOdin(Tuples::Tuple& tuple, const LHCb::ODIN* odin) {
  bool test = true;
  test &= tuple->column("run", odin->runNumber());
  test &= tuple->column("gps", odin->gpsTime());
  test &= tuple->column("evt", odin->eventNumber());
  test &= tuple->column("orbit", odin->orbitNumber());
  test &= tuple->column("step", odin->calibrationStep());
  test &= tuple->column("bcid", odin->bunchId());
  test &= tuple->column("bx", odin->bunchCrossingType());
  return test;
}

bool LumiEventTuple::fillCounters(Tuples::Tuple& tuple, const LHCb::HltLumiSummary* summary) {
  bool test = true;
  if (m_arrayColumns) {
    m_keys.clear();
    m_values.clear();
    for (const auto& kvp : summary->extraInfo()) {
      m_keys.push_back(kvp.first);
      m_values.push_back(kvp.second);
    }
    tuple->farray("lc_key", m_keys, "lc_n", m_keys.size());
    tuple->farray("lc_value", m_values, "lc_n", m_values.size());
  } else {
    for (const auto& kvp : summary->extraInfo()) {
      int value = kvp.second;
      if (value < 0) value = value & 0xFFFF; // replicate behaviour of HltLumiWriter and HltLumiSummaryDecoder
      test &= tuple->column("lc_" + std::to_string(kvp.first), boost::numeric_cast<unsigned short>(value));
    }
  }
  return test;
}
