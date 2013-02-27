// $Id: MCBs2JpsiPhiAngleCalculator.h,v 1.3 2010-04-22 11:52:53 gcowan Exp $
#ifndef MCANGLECALCULATOR_H
#define MCANGLECALCULATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVMCPartAngleCalculator.h"            // Interface


/** @class MCBs2JpsiPhiAngleCalculator MCBs2JpsiPhiAngleCalculator.h v1r3/MCBs2JpsiPhiAngleCalculator.h
 *
 *  Calculates helicity and transversity angles for a true B->J/psiPhi decay
 *
 *  @author Greig Cowan
 *  @date   2008-05-12
 */
class MCBs2JpsiPhiAngleCalculator : public GaudiTool, virtual public IP2VVMCPartAngleCalculator {
public:
  /// Standard constructor
  MCBs2JpsiPhiAngleCalculator( const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  virtual ~MCBs2JpsiPhiAngleCalculator( ); ///< Destructor

  StatusCode calculateAngles( const LHCb::MCParticle* particle ,
                              double &thetal, double &thetak, double &phi );

  double calculateThetaL( const LHCb::MCParticle* particle );
  double calculateThetaK( const LHCb::MCParticle* particle );
  double calculatePhi( const LHCb::MCParticle* particle );

  StatusCode calculateTransversityAngles( const LHCb::MCParticle* particle ,
                                          double &Theta_tr,
                                          double &Phi_tr,
                                          double &Theta_V );

  double calculateTransThetaTr( const LHCb::MCParticle* particle );
  double calculateTransPhiTr( const LHCb::MCParticle* particle );
  double calculateTransThetaV( const LHCb::MCParticle* particle );

  double calculateMass( const LHCb::MCParticle* particle );

protected:

  void fillDescendants( const LHCb::MCParticle*, LHCb::MCParticle::ConstVector&, int);
  IP2VVAngleCalculator* m_angle;

private:
  int m_depth;
  StatusCode getParticles(const LHCb::MCParticle*&,
                          LHCb::MCParticle::ConstVector&,
                          const LHCb::MCParticle*&,
                          const LHCb::MCParticle*&,
                          const LHCb::MCParticle*&,
                          const LHCb::MCParticle*&);
};
#endif // MCANGLECALCULATOR_H
