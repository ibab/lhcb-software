// $Id: IP2VVPartAngleCalculator.h,v 1.1 2007-09-18 13:35:58 pkoppenb Exp $
#ifndef IBD2KSTARMUMUANGLECALCULATOR_H 
#define IBD2KSTARMUMUANGLECALCULATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IP2VVPartAngleCalculator ( "IP2VVPartAngleCalculator", 1, 0 );

/** @class IP2VVPartAngleCalculator IP2VVPartAngleCalculator.h
 *
 *  Calculates the three angles in a P->VV decay. The methods are called
 *  ThetaL, ThetaK and Phi to comply with the B->MuMuK* conventions,
 *  but tools for other decays could be implemented.
 *
 *  @author Thomas Blake
 *  @date   2007-08-02
 */
class IP2VVPartAngleCalculator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IP2VVPartAngleCalculator; }

  /// get theta_L (angle of the L+ wrt B in dilepton frame)
  virtual double calculateThetaL( const LHCb::Particle* mother) = 0 ;
  /// get theta_K (angle of the K+ wrt B in dilepton frame)
  virtual double calculateThetaK( const LHCb::Particle* mother ) = 0 ;
  /// get phi (angle of the K* and LL decay planes in dilepton frame)
  virtual double calculatePhi( const LHCb::Particle* mother ) = 0 ;
  /// get all three
  virtual StatusCode calculateAngles( const LHCb::Particle* mother, double& thetal, double& thetak, double& phi ) = 0 ;
  
protected:

private:

};
#endif // IBD2KSTRAMUMUANGLECALCULATOR_H
