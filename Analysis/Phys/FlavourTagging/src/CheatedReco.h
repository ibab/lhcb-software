// $Id: CheatedReco.h,v 1.1 2003-06-13 08:35:05 odie Exp $
#ifndef CHEATEDRECO_H 
#define CHEATEDRECO_H 1

// Include files
// from STL
#include <string>
#include <map>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

class IMCDecayFinder;
class Particle;

/** @class CheatedReco CheatedReco.h User/CheatedReco.h
 *  Fake reconstruction of B from MC
 *
 *  @author Olivier Dormond
 *  @date   2003-02-05
 */
class CheatedReco : public DVAlgorithm {
public:
  /// Standard constructor
  CheatedReco( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheatedReco( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int m_count;

  IMCDecayFinder *m_mcDecayFinder;
	std::string m_key;
  std::vector<std::string> m_decays;
  std::vector<std::string> m_keys;
  std::map<std::string, long> m_pids;
  std::map<std::string, std::pair<std::vector<std::string>, std::string> > m_sources;
  std::map<std::string, Particle *> m_products;
  
  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  Particle2MCLinksAsct::IAsct* m_pAsctLinks; ///< Pointer to asso using links
};
#endif // CHEATEDRECO_H
