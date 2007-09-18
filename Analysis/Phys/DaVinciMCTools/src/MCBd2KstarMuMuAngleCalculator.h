// $Id: MCBd2KstarMuMuAngleCalculator.h,v 1.1 2007-09-18 14:01:55 pkoppenb Exp $
#ifndef MCANGLECALCULATOR_H 
#define MCANGLECALCULATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IP2VVMCPartAngleCalculator.h"            // Interface


/** @class MCBd2KstarMuMuAngleCalculator MCBd2KstarMuMuAngleCalculator.h v1r3/MCBd2KstarMuMuAngleCalculator.h
 *  
 *  Calculates angles for a true B->mumuK* decay
 *
 *  @author Thomas Blake
 *  @date   2007-08-13
 */
class MCBd2KstarMuMuAngleCalculator : public GaudiTool, virtual public IP2VVMCPartAngleCalculator {
public: 
  /// Standard constructor
  MCBd2KstarMuMuAngleCalculator( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~MCBd2KstarMuMuAngleCalculator( ); ///< Destructor

  StatusCode calculateAngles( const LHCb::MCParticle* particle , double& thetal, double &thetak, double &phi );
  double calculateThetaL( const LHCb::MCParticle* particle  );
  double calculateThetaK( const LHCb::MCParticle* particle  );
  double calculatePhi( const LHCb::MCParticle* particle  );
  
  double calculateMass( const LHCb::MCParticle* particle );

  StatusCode initialize() ;

protected:
  
  void fillDescendants( const LHCb::MCParticle*, LHCb::MCParticle::ConstVector& , int);
  IP2VVAngleCalculator* m_angle;
  
    
private:
  int m_depth ;
  
};
#endif // MCANGLECALCULATOR_H
