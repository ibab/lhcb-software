#ifndef ASSOCIATORS_VeloCluster2MCParticleASCT_H 
#define ASSOCIATORS_VeloCluster2MCParticleASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/VeloCluster.h"
#include "Event/MCParticle.h"

// Associators
#include "Relations/AssociatorWeighted.h"
#include "Relations/RelationWeighted2D.h"

static const std::string& VeloCluster2MCParticleAsctLocation = 
 "Rec/Relations/VeloClusters2MCParticles"; 

/** @class VeloCluster2MCParticleAsct VeloCluster2MCParticleAsct.h Associators/VeloCluster2MCParticleAsct.h
 *  
 *
 *  @author Chris Parkes
 *  @date   22/05/2002
 */

class VeloCluster2MCParticleAsct: 
  public AssociatorWeighted<VeloCluster,MCParticle,double> {

  friend class ToolFactory<VeloCluster2MCParticleAsct>;
  
public:
  /// Define data types
  typedef RelationWeighted2D<VeloCluster,MCParticle,double> Table;
  typedef OwnType                                   Asct;
    
protected:

  /// Standard constructor
  VeloCluster2MCParticleAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent ): 
  Asct( type, name, parent) {
    setProperty( "Location", VeloCluster2MCParticleAsctLocation );
    setProperty( "AlgorithmType", "VeloCluster2MCParticleAlg" );
    setProperty( "AlgorithmName", "VeloCluster2MCParticle" );
  }; 

  /// Destructor
  virtual ~VeloCluster2MCParticleAsct() {}; ///< Destructor

private:

};
#endif // ASSOCIATORS_VeloCluster2MCParticleASCT_H


