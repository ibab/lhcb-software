// $Id: FlavourMonitorAlgorithm.h,v 1.4 2003-05-30 17:12:36 gcorti Exp $
#ifndef FLAVOURMONITORALGORITHM_H 
#define FLAVOURMONITORALGORITHM_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

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
  unsigned int m_fractions;
  unsigned int m_maxCollisions;
  std::vector<std::vector<std::vector<unsigned int> > > m_n_good;
  std::vector<std::vector<std::vector<unsigned int> > > m_n_wrong;
  std::vector<std::vector<std::vector<unsigned int> > > m_n_untagged;
  std::vector<std::vector<std::vector<unsigned int> > > m_n_fractions;
  std::vector<std::vector<unsigned int> > m_n_noB;
  std::vector<unsigned int> m_n_collisions;

  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  Particle2MCLinksAsct::IAsct* m_pAsctLinks; ///< Pointer to asso using links
};
#endif // FLAVOURMONITORALGORITHM_H
