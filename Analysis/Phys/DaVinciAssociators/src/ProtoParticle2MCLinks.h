// $Id: ProtoParticle2MCLinks.h,v 1.1 2002-07-12 15:28:47 phicharp Exp $
#ifndef ProtoParticle2MCLinks_H 
#define ProtoParticle2MCLinks_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from Relations
#include "Relations/IAssociatorWeighted.h"

// from Event
#include "Event/TrStoredTrack.h"

// local
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"

/** @class ProtoParticle2MCLinks ProtoParticle2MCLinks.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/07/2002
 */
class ProtoParticle2MCLinks : public Algorithm {
  friend AlgFactory<ProtoParticle2MCLinks>;
  
public:
  /// Standard constructor
  ProtoParticle2MCLinks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProtoParticle2MCLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; }
  
protected:

private:
  std::vector<std::string> m_inputData;  ///< location of Particles to associate
  std::string m_outputTable; ///< location of relations table 
  IAssociatorWeighted<TrStoredTrack,MCParticle,double>* m_track2MCParticleAsct;
  std::string m_trackAsctName;
};
#endif // ProtoParticle2MCLinks_H
