// $Id: MCUtilityTool.h,v 1.1 2002-02-08 18:25:42 gcorti Exp $
#ifndef MCUTILITYTOOL_H 
#define MCUTILITYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IMCUtilityTool.h"


// Forward declarations
class IParticlePropertySvc;

/** @class MCUtilityTool MCUtilityTool.h
 *  This is an interface class for a generic Monte Carlo Utility
 *  Tool
 *
 *  @author Gloria Corti
 *  @date   14/10/2001
 */
class MCUtilityTool : public AlgTool,
                      virtual public IMCUtilityTool {
public:
  
  /// Standard Constructor
  MCUtilityTool(const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  /// Destructor 
  virtual ~MCUtilityTool( ); ///< Destructor
  

  /// Implementation of IMCUtilityTool interface.
  /// Print decay tree for a given particle
  virtual void printDecayTree( long depth, const std::string& prefix,
                       const MCParticle* mother);
  
  /// Match decay tree for a given particle
  virtual bool matchDecayTree( const MCParticle* mother, 
                               std::vector<long> daughtersID );

  /// Retrieve the history of a given particle
  virtual StatusCode history( const MCParticle* product,
                              std::vector<long>& parentsID );  

  /// Print the history of a given particle
  virtual void printHistory( const MCParticle* product );

protected:

  /// Return the ppSvc in order not to use the data member directly  
  IParticlePropertySvc* ppSvc();
  /// Return the print depth (same reason)
  long printingDepth();

private:
  
  IParticlePropertySvc* m_ppSvc;    ///< Reference to particle property service
  long                  m_depth;    ///< Depth of printing for tree
  

};
#endif // MCUTILITYTOOL_H
