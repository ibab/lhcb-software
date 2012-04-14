#ifndef COPYPARTICLE2TISTOSDECISIONS_H 
#define COPYPARTICLE2TISTOSDECISIONS_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

class ITriggerTisTos;

namespace LHCb {
  class HltDecReports;
}


/** @class CopyParticle2TisTosDecisions CopyParticle2TisTosDecisions.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2010-09-28
 */
class CopyParticle2TisTosDecisions : public MicroDSTAlgorithm
{

public: 

  /// Standard constructor
  CopyParticle2TisTosDecisions( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticle2TisTosDecisions( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  void executeLocation(const std::string& particleLocation);

private:

  ITriggerTisTos* m_iTisTos;
  ITriggerTisTos* m_iTisTosL0;
  LHCb::HltDecReports* m_decReports;
  LHCb::HltDecReports* m_decReportsL0;
  std::string m_decReportsLocation;
  std::string m_decReportsLocationL0;
  
};
#endif // COPYPARTICLE2TISTOSDECISIONS_H
