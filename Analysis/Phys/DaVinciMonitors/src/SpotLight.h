// $Id: SpotLight.h,v 1.1 2008-12-09 08:07:05 pkoppenb Exp $
#ifndef SPOTLIGHT_H 
#define SPOTLIGHT_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class SpotLight SpotLight.h
 *  
 *  Algorithm to be run at the end of the stripping and identifying
 *  very good candidate B events.
 *
 *  @author Patrick Koppenburg
 *  @date   2008-09-22
 */
class SpotLight : public DVAlgorithm {
public: 
  /// Standard constructor
  SpotLight( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SpotLight( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool spotLight(const LHCb::Particle *, int level) const; ///< look at a candidate
  LHCb::Particle::ConstVector headCandidates() const ; ///< keep only top of chain candidates

  IFilterCriterion* m_mother ;        ///< Mother cut
  IFilterCriterion* m_intermediate ;  ///< Intermediate state cuts
  IFilterCriterion* m_final ;         ///< Final state cuts
  IPrintDecay* m_print  ;             ///< print decay tree
  std::vector<std::string> m_algos ;               ///< Algorithms
  std::string m_selResults ;           ///< SelResult location
};
#endif // SPOTLIGHT_H
