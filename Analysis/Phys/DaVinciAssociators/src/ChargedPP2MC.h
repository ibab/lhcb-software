// $Id: ChargedPP2MC.h,v 1.1 2002-10-02 07:06:26 phicharp Exp $
#ifndef ChargedPP2MC_H 
#define ChargedPP2MC_H 1

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

/** @class ChargedPP2MC ChargedPP2MC.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   10/07/2002
 */
class ChargedPP2MC : public Algorithm {
  friend AlgFactory<ChargedPP2MC>;
  
public:
  /// Standard constructor
  ChargedPP2MC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedPP2MC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Location of table in TES
  std::string outputTable() { return m_outputTable; }
  
protected:

private:
  std::vector<std::string> m_inputData;  ///< location of Particles to associate
  std::string m_outputTable; ///< location of relations table 
  Tr2MCPartAsct* m_track2MCParticleAsct;
  std::string m_trackAsctName;
};
#endif // ChargedPP2MC_H
