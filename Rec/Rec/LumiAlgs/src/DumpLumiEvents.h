#ifndef DUMPLUMIEVENTS_H
#define DUMPLUMIEVENTS_H 1

#include "Event/ODIN.h"
#include "Event/HltLumiSummary.h"
#include "Event/RecVertex.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include <iostream>
#include <fstream>
#include <vector>

class KeyLocationPair {
public:
  KeyLocationPair(ulonglong k, ulonglong l) { key = k; location = l; };
  ulonglong key;
  ulonglong location;
};

/** @class DumpLumiEvents DumpLumiEvents.h
 *
 * Algorithm that dumps random events from RAW files.
 * Output is ASCII file with name specified by OutputFileName option.
 * For each event with nonzero random counter a line is written:
 *   odin_1,...,odin_N,;key_1,value_1,...key_M,value_M,[vtx_1,...,vtx_K,];
 * where odin_i are ODIN fields, (key_j,value_j) are lumi counter pairs,
 * vtx_k are "x,y,z,n_tr,chi2" and items in [] are optional. Output can be
 * optionally compressed and time-ordered (default). Intermediate non-ordered
 * data can be kept in memory (default) or in file. See description of class
 * members for more details.
 * Typical memory footprint of the simplest job with default settings
 * (in-memory intermediate data and without output of vertices) for 1h run
 * is just below 1GB.
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
  typedef std::vector<KeyLocationPair> KeyLocationPairs;

  void outputOdin(std::ostream& s, LHCb::ODIN* odin); ///<  Output ODIN values in format "odin_1,...,odin_N,;" (see implementation).
  void outputLumiSummary(std::ostream& s, LHCb::HltLumiSummary* hltLumiSummary); ///<  Output lumi summary in format "key_1,value_1,...,key_M,value_M,;".
  void outputVertices(std::ostream& s, LHCb::RecVertices* recVertices); ///< Output vertices in format "vtx_1,...,vtx_K,;", where vtx_i is "x,y,z,n_tr,chi2".

  void insertionSort();

  bool m_sort; ///< Whether to order events
  std::string m_outputFileName; ///< If filename end with .bz2, output will be compress, otherwise output is plain ASCII
  std::string m_recVerticesLocation; ///< If non-empty, vertices in this location will also be dumped
  std::string m_intermediateStreamFileName; ///< If non-empty, the intermediate non-sorted dump should be stored in file instead of memory
  unsigned int m_nEventsHint; ///< Hint for the expected number of events, used to reserve memory for m_keyLocationPairs

  bool m_dumpVertices;
  bool m_compressed;
  std::ostream* m_intermediateStream;
  std::ostream* m_outputStream;
  std::ofstream m_outputFile;
  KeyLocationPairs m_keyLocationPairs;
};
#endif
