// $Id: Bd2KstarMuMuAngleCalculator.h,v 1.1 2007-09-18 13:50:28 pkoppenb Exp $
#ifndef BD2KSTARMUMUANGLECALCULATOR_H 
#define BD2KSTARMUMUANGLECALCULATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVPartAngleCalculator.h"            // Interface


/** @class Bd2KstarMuMuAngleCalculator Bd2KstarMuMuAngleCalculator.h
 *  
 *  Calculates the three angles in a B->llKPi decay, eg B->MuMuK*
 *
 *  @author Thomas Blake
 *  @date   2007-08-02
 */

class Bd2KstarMuMuAngleCalculator : public GaudiTool, virtual public IP2VVPartAngleCalculator {
public: 
  /// Standard constructor
  Bd2KstarMuMuAngleCalculator( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~Bd2KstarMuMuAngleCalculator( ); ///< Destructor
  
  double calculateThetaL( const LHCb::Particle* particle );
  double calculateThetaK( const LHCb::Particle* particle );
  double calculatePhi( const LHCb::Particle* particle );
  
  StatusCode calculateAngles( const LHCb::Particle* particle, double& thetal, double& thetak, double& phi );
  
  StatusCode initialize() ;
  

protected:

private:
  
  
  IParticleDescendants* m_descendants ;
  IP2VVAngleCalculator* m_angle;
  
  //  int m_KPi;
  //  int m_Lepton;
  //  int m_DiLepton;
  
  //  bool m_reconstructDiLepton;
  //  bool m_nonResonant;
  
  
};
#endif // BD2KSTARMUMUANGLECALCULATOR_H
