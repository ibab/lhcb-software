// $Id: $
#ifndef TESFINGERPRINT_H
#define TESFINGERPRINT_H 1

#include <map>

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IDataSelector.h"

class IDataStoreLeaves;

/** @class TESFingerPrint TESFingerPrint.h
 *
 *
 *  @author Illya Shapoval
 *  @date   2011-05-11
 */
class TESFingerPrint : public GaudiAlgorithm {
public:
  /// Standard constructor
  TESFingerPrint( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TESFingerPrint( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Pointer to the (public) tool used to retrieve the objects in a file.
  IDataStoreLeaves *m_leavesTool;

  /// Collection of objects being selected
  IDataSelector m_objects;

  /// Level of the heuristic analysis
  std::string m_heur_level;

  /// Counter map of nodes occurrences
  std::map<std::string, long> m_stat_map;
  /// Counter map of containers and their contents occurrences
  std::map<std::string, std::map<int,long> > m_cont_stat_map;
  /// File name for the TES finger print output
  std::string m_output_file_name;

};
#endif // TESFINGERPRINT_H
