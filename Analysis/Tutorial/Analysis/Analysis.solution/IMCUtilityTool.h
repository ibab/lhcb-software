// $Id: IMCUtilityTool.h,v 1.1.1.1 2001-10-19 17:06:05 gcorti Exp $
#ifndef TOOLSTUTORIAL_IMCUTILITYTOOL_H 
#define TOOLSTUTORIAL_IMCUTILITYTOOL_H 1

// Include files
#include "GaudiKernel/System.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include <string>

// Forward declarations
class MCParticle;

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IMCUtilityTool(440, 1, 0);

/** @class IMCUtilityTool IMCUtilityTool.h Tools/IMCUtilityTool.h
 *  This is an interface class for a generic Monte Carlo Utility
 *  Tool
 *
 *  @author Gloria Corti
 *  @date   14/10/2001
 */
class IMCUtilityTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMCUtilityTool; }

  /// Print decay tree for a given particle
  virtual void printDecayTree( long depth, const std::string& prefix,
                               const MCParticle* mother) = 0;
  
  /// Match decay tree for a given particle
  virtual bool matchDecayTree( const MCParticle* mother, 
                               std::vector<long> daughtersID ) = 0;


};
#endif // TOOLSTUTORIAL_IMCUTILITYTOOL_H
