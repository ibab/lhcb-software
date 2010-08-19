// $Id: CopyParticle2LHCbIDs.h,v 1.1 2010-08-19 15:34:31 jpalac Exp $
#ifndef COPYPARTICLE2LHCBIDS_H 
#define COPYPARTICLE2LHCBIDS_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/Particle2LHCbIDVectorMap.h"

class ITriggerTisTos;

/** @class CopyParticle2LHCbIDs CopyParticle2LHCbIDs.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2010-08-18
 */
class CopyParticle2LHCbIDs : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyParticle2LHCbIDs( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticle2LHCbIDs( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  void executeLocation(const std::string& particleLocation);

private:

  ITriggerTisTos* m_iTisTos;

};
#endif // COPYPARTICLE2LHCBIDS_H
