
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleTupleAlg.h
 *
 * Header file for algorithm ChargedProtoParticleTupleAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleTupleAlg.h,v 1.2 2007-05-31 14:24:03 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-11-15
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleTupleAlg_H
#define GLOBALRECO_ChargedProtoParticleTupleAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// Event
#include "Event/ProtoParticle.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
#include "Event/MCParticle.h"

// Tool interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoParticleTupleAlg ChargedProtoParticleTupleAlg.h
 *
 *  Simple algorithm to produce an ntuple containing the charged ProtoParticle
 *  PID information. Useful for tuning purposes.
 *
 *  @author Chris Jones
 *  @date   2006-11-15
 */
//-----------------------------------------------------------------------------

class ChargedProtoParticleTupleAlg : public GaudiTupleAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleTupleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleTupleAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_protoPath;    ///< Location in TES of ProtoParticles

  /// Use RICH tool to get MCParticle associations for Tracks (To avoid Linker details)
  const Rich::Rec::MC::IMCTruthTool * m_truth;

};

#endif // GLOBALRECO_ChargedProtoParticleTupleAlg_H
