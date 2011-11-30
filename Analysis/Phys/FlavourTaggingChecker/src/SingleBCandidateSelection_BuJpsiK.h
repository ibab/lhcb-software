#ifndef USER_SingleBCandidateSelection_BuJpsiK_H 
#define USER_SingleBCandidateSelection_BuJpsiK_H 1

// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IParticleDescendants.h"
#include "ITaggingUtilsChecker.h"

class IDistanceCalculator;
class DVAlgorithm;
class IPVReFitter;

// from local

/** @class SingleBCandidateSelection_BuJpsiK SingleBCandidateSelection_BuJpsiK.h 
 *  
 *  Select events looking for the one candidate with best chi2
 *
 *  @author Marco Musy
 *  @date   02/11/2010
 */

class SingleBCandidateSelection_BuJpsiK : public DVAlgorithm {

 public: 
  /// Standard constructor
  SingleBCandidateSelection_BuJpsiK( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SingleBCandidateSelection_BuJpsiK( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 private:
  const LHCb::Particle* findID(unsigned int id, LHCb::Particle::ConstVector& v, std::string s="");
  
  double get_MINIPCHI2(const LHCb::Particle* p, int opt);
  double get_BPVVDCHI2(const LHCb::Particle* B, const LHCb::Particle* P);
  double get_MIPDV(const LHCb::Particle* p);
  //void get_MIPDV(const LHCb::Particle* p, double MyIP, double MyIPChi2);

  std::string m_inputLocation;
  IParticleDescendants* m_descend;
  ITaggingUtilsChecker* m_util;
  IPVReFitter*          m_pvReFitter;

};
//===========================================================================//
#endif // USER_SingleBCandidateSelection_BuJpsiK_H
