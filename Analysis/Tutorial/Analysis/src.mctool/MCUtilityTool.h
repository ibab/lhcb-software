// $Id: MCUtilityTool.h,v 1.1.1.1 2001-10-19 17:06:05 gcorti Exp $
#ifndef TOOLSTUTORIAL_MCUTILITYTOOL_H 
#define TOOLSTUTORIAL_MCUTILITYTOOL_H 1

// Include files
#include "Analysis/IMCUtilityTool.h"    // Necessary for interface
#include "GaudiKernel/AlgTool.h"        // Necessary for inheritance

// Forward declarations
class IParticlePropertySvc;

/** @class MCUtilityTool MCUtilityTool.h Tools/MCUtilityTool.h
 *  This is an interface class for a generic Monte Carlo Utility
 *  Tool
 *
 *  @author Gloria Corti
 *  @date   14/10/2001
 */
class MCUtilityTool : public AlgTool,
                      virtual public IMCUtilityTool {
public:
  
  /// Query the interface
  virtual StatusCode queryInterface( const IID& riid, void** ppvInterface );

  /// Standard Constructor
  MCUtilityTool(const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  /// Destructor 
  ~MCUtilityTool( );
  

  /// Implementation of IMCUtilityTool interface.
  /// Print decay tree for a given particle
  void printDecayTree( long depth, const std::string& prefix,
                       const MCParticle* mother);
  
  /// Match decay tree for a given particle
  bool matchDecayTree( const MCParticle* mother, 
                       std::vector<long> daughtersID );

private:
  
  IParticlePropertySvc* m_ppSvc;    ///< Reference to particle property service
  long                  m_depth;    ///< Depth of printing for tree

};
#endif // TOOLSTUTORIAL_MCUTILITYTOOL_H
