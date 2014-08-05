// $Id$
#ifndef REMOVECLONES_H
#define REMOVECLONES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

/** @class RemoveClones RemoveClones.h
 *
 *  Filter the local storage to remove all clones of particles
 *
 *  @author Rob lambert
 *  @date   2008-06-05
 *
 *
 */
class ICheckOverlap;

//===========================================================================
/** RemoveClones is a useful tool to be called after FilterDesktop
 * It filters the local particle storage into just those particles
 * which are unique.
 * This alg uses the FindCloneTool by default, which
 * takes a vector of particles and returns a vector of all unique particles
 * So-called clones are removed.
 * A clone of 'B' is a particle for which shares *all* the same protos as 'B'.
 */
//===========================================================================

class RemoveClones : public DaVinciAlgorithm
{

public:
  /// Standard constructor
  RemoveClones( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RemoveClones( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  ICheckOverlap* m_checkOverlap ; ///<instance of check overlap tool
  std::string m_findCloneTool; ///<name of check overlap tool, set by "FindCloneTool"
  bool m_byPID; ///<whether to filter by the PID set by "FilterByPid"
  bool m_byAbsPID;///<whether to filter by the absPID set by "FilterByAbsPid"
  unsigned long long m_NpartIn;  ///<number of particles so far sent to the alg
  unsigned long long m_NpartOut; ///<number of unique particles so far written out by the alg

private:

  /// Filter a vector of particles into vectors with the same PID, or abs(ID).
  StatusCode FilterById( const LHCb::Particle::ConstVector& parts,
                         std::vector<LHCb::Particle::ConstVector> & particlesbyPID ) const;

};
#endif // REMOVECLONES_H
