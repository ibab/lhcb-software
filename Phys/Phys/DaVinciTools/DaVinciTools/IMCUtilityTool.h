// $Id: IMCUtilityTool.h,v 1.1 2002-02-08 18:05:37 gcorti Exp $
#ifndef DAVINCITOOLS_IMCUTILITYTOOL_H
#define DAVINCITOOLS_IMCUTILITYTOOL_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class MCParticle;

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IMCUtilityTool("IMCUtilityTool", 1, 0);

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
                               const MCParticle* mother ) = 0;

  /// Match decay tree (1-step) for a given particle
  virtual bool matchDecayTree( const MCParticle* mother,
                               std::vector<long> daughtersID ) = 0;

  /// Retrive the history of a given particle
  virtual StatusCode history( const MCParticle* product,
                              std::vector<long>& parentsID ) = 0;

  /// Print history of a given particle
  virtual void printHistory( const MCParticle* product ) = 0;

};
#endif // DAVINCITOOLS_IMCUTILITYTOOL_H
