// $Id: HltRecChecker.h,v 1.1.1.1 2006-09-25 09:45:14 hernando Exp $
#ifndef HLTRECCHECKER_H 
#define HLTRECCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltCommon/HltAlgorithm.h"
#include "Event/MCParticle.h"

/** @class HltRecChecker HltRecChecker.h
 *  
 *
 *  @author Hugo Ruiz Perez
 *  @author Jose A. Hernando
 *  @date   2006-05-24
 */
class HltRecChecker : public HltAlgorithm {
public: 
  /// Standard constructor
  HltRecChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRecChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  
  int getAncestors(const LHCb::MCParticle& mcpar, 
                   std::vector<LHCb::MCParticle*>& ancestors);
  
  const LHCb::MCParticle& rootMother(const LHCb::MCParticle& par);

  int iquark(const LHCb::MCParticle& par) {
    if (par.particleID().hasBottom()) return 1;
    else if (par.particleID().hasCharm()) return 2;
    return 3;
  }

  int tosTis(bool tos,bool tis) {
    if (tos && tis) return 0;
    else if (tos && !tis) return 1;
    else if (!tos && tis) return 2;
    else if (!tos & !tis) return 3;
  }
  

protected:

  int quark();

  void checkQuark();
  
  void checkTracks();
  
  void checkVertices();  

protected:

  std::string m_linkName;

};
#endif // HLTMCMONITOR_H
