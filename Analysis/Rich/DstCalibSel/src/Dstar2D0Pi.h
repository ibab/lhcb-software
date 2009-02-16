// $Id: Dstar2D0Pi.h,v 1.1.1.1 2009-02-16 14:11:47 jonrob Exp $
#ifndef DSTAR2D0PI_H 
#define DSTAR2D0PI_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class Dstar2D0Pi Dstar2D0Pi.h
 *  
 *
 *  @author Andrew Stephen Powell
 *  @date   2009-02-09
 */
class Dstar2D0Pi : public DVAlgorithm {
public: 
  /// Standard constructor
  Dstar2D0Pi( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Dstar2D0Pi( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Build No PID D*
  StatusCode MakeDstar2D0Pi(const LHCb::Particle::ConstVector&);

  double m_D0Mass;  // Nominal D0 Mass
  double m_DstarMass; // Nominal D* Mass
  double m_D0DstarMassDiff; // Nominal Mass Difference


  double m_DstarMassWin;
  double m_DstarPtCut;
  double m_VertexChi2Dstar;
  double m_diffDstarD0;

  // Retrieve StdHep ID for Pions and D0s:
  LHCb::ParticleID m_D0ID;
  LHCb::ParticleID m_DstarPlusID;
  LHCb::ParticleID m_DstarMinusID;
  LHCb::ParticleID m_DstarPID;
  

};
#endif // DSTAR2D0PI_H
