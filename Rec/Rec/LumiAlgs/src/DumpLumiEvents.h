#ifndef DUMPLUMIEVENTS_H
#define DUMPLUMIEVENTS_H 1

#include "Event/ODIN.h"
#include "Event/HltLumiSummary.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include <fstream>

/** @class DumpLumiEvents DumpLumiEvents.h
 *
 * Algorithm that dumps random events from RAW files.
 * Output is ASCII file with name specified by OutputFileName option.
 * For each event with nonzero random counter a line is written:
 *   odin_1,...,odin_N,;key_1,value_1,...key_M,value_M,;
 * where odin_i are ODIN fields and (key_j,value_j) are lumi counter pairs.
 * Events are not time-ordered!
 *
 *  @author Rosen Matev
 *  @date   2012-03-16
 */
class DumpLumiEvents: public GaudiAlgorithm {
public:
  DumpLumiEvents(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~DumpLumiEvents();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  void outputOdin(std::ostream& s, LHCb::ODIN* odin); ///<  Output ODIN values in format "odin_1,...,odin_N,;" (see implementation).
  void outputLumiSummary(std::ostream& s, LHCb::HltLumiSummary* hltLumiSummary); ///<  Output lumi summary in format "key_1,value_1,...,key_M,value_M,;".

  std::string m_outputFileName; ///< Filename of output ASCII file
  std::ofstream m_output;

};
#endif
