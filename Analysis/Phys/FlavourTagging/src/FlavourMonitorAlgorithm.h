// $Id: FlavourMonitorAlgorithm.h,v 1.1 2002-09-03 08:22:07 odie Exp $
#ifndef FLAVOURMONITORALGORITHM_H 
#define FLAVOURMONITORALGORITHM_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCAsct.h"

/** @class FlavourMonitorAlgorithm FlavourMonitorAlgorithm.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   2002-07-15
 */
class FlavourMonitorAlgorithm : public DVAlgorithm {
public:
  /// Standard constructor
  FlavourMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FlavourMonitorAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Vector of locations of the tags to monitor
  std::vector<std::string> m_tags_locations; 

  /// Statistics accumulator
  std::vector<unsigned int> m_n_good, m_n_wrong, m_n_untagged, m_n_noB;

  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  Particle2MCAsct::IAsct* m_pAsctLinks; ///< Pointer to associator using links
};
#endif // FLAVOURMONITORALGORITHM_H
