// $Id: HltSelChecker.h,v 1.2 2007-07-13 08:51:11 pkoppenb Exp $
#ifndef HLTSELCHECKER_H 
#define HLTSELCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Kernel/Particle2MCLinker.h"

class IBackgroundCategory;

/** @class HltSelChecker HltSelChecker.h
 *  
 *  Generic simple Tuple filler
 *
 *  @author Patrick Koppenburg
 *  @date   2007-07-06
 */
class HltSelChecker : public DVAlgorithm {
  typedef LHCb::Particle::ConstVector  PartVec ;
  typedef LHCb::RecVertex::ConstVector RecVec;
  typedef PartVec::const_iterator P_iter ;
  typedef RecVec::const_iterator PV_iter ;
public: 
  /// Standard constructor
  HltSelChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  StatusCode saveP(std::string, const LHCb::Particle*, Tuple&) ;
  StatusCode saveMC(std::string, const LHCb::Particle*, Tuple&) ;
  StatusCode twoBody(std::string, const LHCb::Particle*, Tuple&);
  Gaudi::XYZPoint extrapolatedMC(double,const LHCb::MCParticle*);
  StatusCode printParticle(const LHCb::Particle* P);
  StatusCode printTrack(const LHCb::Track*);

private:
  IBackgroundCategory* m_background; ///< Background category @todo Restore
  bool m_doMC ; ///< do mC?
  bool m_doPrint ; ///< Print Particles
  Particle2MCLinker* m_pLinker;   ///<  Associator
};
#endif // HLTSELCHECKER_H
