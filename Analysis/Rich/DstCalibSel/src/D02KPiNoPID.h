// $Id: D02KPiNoPID.h,v 1.2 2009-05-03 14:49:33 jonrob Exp $
#ifndef D02KPINOPID_H 
#define D02KPINOPID_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

/** @class D02KPiNoPID D02KPiNoPID.h
 *  
 *
 *  @author Andrew Stephen Powell
 *  @date   2009-02-03
 */
class D02KPiNoPID : public DVAlgorithm {
public: 
  /// Standard constructor
  D02KPiNoPID( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~D02KPiNoPID( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// Build No PID D0
  StatusCode MakeD02KPi(const LHCb::Particle::ConstVector&);

  long m_D0ID_NoPid;  // Assign JetSet ID number to D0 candidate
  long m_D0barID_NoPid;  // Assign JetSet ID number to D0~ candidate
  double m_D0Mass;  // Nominal D0 Mass

  double m_D0MassWin;  // Mass window for pi/K combination
  double m_ptD0MomCut;  // Transverse momentum cut on D0 candidate
  double m_VertexChiD0;  // Unconstrained combined vertex fit
  double m_D0PtCut;  // Transverse momentum cut on D0
  double m_D0IPChi2; // IP Chi2 Cut 
    
  // Retrieve SdtHep ID for pions and Kaons:
  LHCb::ParticleID m_PiPlusID;  
  LHCb::ParticleID m_KPlusID;  
  LHCb::ParticleID m_D0ID;  
  LHCb::ParticleID m_D0barID;
  LHCb::ParticleID m_D0PID;
  

};
#endif // D02KPINOPID_H
