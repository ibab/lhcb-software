// $Id: MCBd2KstarMuMuAngleCalculator.h,v 1.4 2010-05-04 21:51:54 tblake Exp $
#ifndef MCANGLECALCULATOR_H
#define MCANGLECALCULATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVMCPartAngleCalculator.h"            // Interface
#include "MCInterfaces/IMCDecayFinder.h"
#include "Kernel/IParticlePropertySvc.h"


/** @class MCBd2KstarMuMuAngleCalculator MCBd2KstarMuMuAngleCalculator.h v1r3/MCBd2KstarMuMuAngleCalculator.h
 *
 *  Calculates the three true angles in a \f$ B_{d} \rightarrow K^{+} \pi^{-} \mu^{+} \mu^{-} \f$ decay.
 *  Angles are given in both the helicity and transversity bases.
 *
 *
 *  The Helicity Basis for \f$ B_{d} \rightarrow K^{*}(892)0 \mu^{+} \mu^{-} \f$
 *  is defined by three angles \f$ \theta_{L} \f$, \f$ \theta_{K} \f$ and \f$ \phi \f$.
 *
 *  These angles are defined as:
 *
 *  \f$ \theta_{L} \f$ as the angle between the \f$ {\mu^{+}}(\mu^{-}) \f$ and the direction
 *  opposite the \f$ {B_{d}}(\bar{B_{d}}) \f$ in the rest frame of the \f$ {\mu^{+}\mu^{-}} \f$.
 *  Equivalently this is the angle between the \f$ {\mu^{+}} \f$ in the \f$ {\mu^{+}\mu^{-}} \f$ rest
 *  frame and the direction of the \f$ {\mu^{+}\mu^{-}} \f$ in the B rest-frame.
 *
 *  \f$ \theta_{K} \f$ as the angle between the \f$ K \f$ in the \f$ {K^{*}} \f$ frame and
 *  the \f$ K^{*} \f$ in the B rest-frame.
 *
 *  \f$ \phi \f$ is defined in the B rest-frame as the angle between the planes defined by the
 *  \f$ {\mu^{+}}(\mu^{-}) \f$ and the \f$ K \pi \f$.
 *
 *  @author Thomas Blake, Greig Cowan
 *  @date   2010-05-04
 */

class MCBd2KstarMuMuAngleCalculator : public GaudiTool, virtual public IP2VVMCPartAngleCalculator
{

public:
  /// Standard constructor
  MCBd2KstarMuMuAngleCalculator( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);

  virtual ~MCBd2KstarMuMuAngleCalculator( ); ///< Destructor

  StatusCode calculateAngles( const LHCb::MCParticle* particle ,
                              double& thetal, double &thetak, double &phi );

  double calculateThetaL( const LHCb::MCParticle* particle  );
  double calculateThetaK( const LHCb::MCParticle* particle  );
  double calculatePhi( const LHCb::MCParticle* particle  );

  StatusCode calculateTransversityAngles( const LHCb::MCParticle* particle ,
                                          double &Theta_tr,
                                          double &Phi_tr,
                                          double &Theta_phi_tr );

  double calculateTransThetaTr( const LHCb::MCParticle* particle  );
  double calculateTransPhiTr( const LHCb::MCParticle* particle  );
  double calculateTransThetaV( const LHCb::MCParticle* particle  );

  double calculateMass( const LHCb::MCParticle* particle );

  StatusCode initialize() ;

protected:

  StatusCode daughters( const LHCb::MCParticle* mother );
  bool hasMother( const LHCb::MCParticle* particle,
                  const std::vector<unsigned int>& ids ) const;

private:

  const LHCb::MCParticle* m_pMuPlus;
  const LHCb::MCParticle* m_pMuMinus;
  const LHCb::MCParticle* m_pK;
  const LHCb::MCParticle* m_pPi;

  IMCDecayFinder* m_mcDecayFinder;
  LHCb::IParticlePropertySvc* m_ppSvc;

  std::vector< std::string > m_MuOrigin;
  std::vector< std::string > m_KPiOrigin;

  std::vector< unsigned int > m_MuOriginID;
  std::vector< unsigned int > m_KPiOriginID;

};

#endif // MCANGLECALCULATOR_H
