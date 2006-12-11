
//-----------------------------------------------------------------------------
/** @file ProtoParticleTupleAlg.h
 *
 * Header file for algorithm ProtoParticleTupleAlg
 *
 * CVS Log :-
 * $Id: ProtoParticleTupleAlg.h,v 1.1 2006-12-11 20:48:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-15
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ProtoParticleTupleAlg_H 
#define GLOBALRECO_ProtoParticleTupleAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// Event
#include "Event/ProtoParticle.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
#include "Event/MCParticle.h"

// Tool interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
/** @class ProtoParticleTupleAlg ProtoParticleTupleAlg.h
 *  
 *  Simple algorithm to produce an ntuple containing the ProtoParticle
 *  PID information. Useful for tuning purposes.
 *  
 *  @author Chris Jones
 *  @date   2006-11-15
 */
//-----------------------------------------------------------------------------

class ProtoParticleTupleAlg : public GaudiTupleAlg
{

public: 

  /// Standard constructor
  ProtoParticleTupleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProtoParticleTupleAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_protoPath;    ///< Location in TES of ProtoParticles

  /// Use RICH tool to get MCParticle associations for Tracks (To avoid Linker details)
  const Rich::Rec::MC::IMCTruthTool * m_truth;

};

#endif // GLOBALRECO_ProtoParticleTupleAlg_H 
