
//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.h
 *
 * Header file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ProtoPTupleAlg.h,v 1.1 2006-11-15 16:42:46 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ProtoPTupleAlg_H 
#define GLOBALRECO_ProtoPTupleAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// Event
#include "Event/ProtoParticle.h"
#include "Event/MCParticle.h"

// Tool interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
/** @class ProtoPTupleAlg ProtoPTupleAlg.h
 *  
 *  Simple algorithm to produce an ntuple containing the ProtoParticle
 *  DLL information. Useful for tuning purposes.
 *  
 *  @author Chris Jones
 *  @date   2006-11-15
 */
//-----------------------------------------------------------------------------

class ProtoPTupleAlg : public GaudiTupleAlg
{

public: 

  /// Standard constructor
  ProtoPTupleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProtoPTupleAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string m_protoPath;    ///< Location in TES of ProtoParticles

  /// Use RICH tool to get MCParticle associations for Tracks (To avoid Linker details)
  const IRichRecMCTruthTool * m_truth;

};

#endif // GLOBALRECO_ProtoPTupleAlg_H 
