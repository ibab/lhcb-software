#ifndef ASSOCIATORS_VeloCluster2MCHitASCT_H 
#define ASSOCIATORS_VeloCluster2MCHitASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/VeloCluster.h"
#include "Event/MCVeloHit.h"

// Associators
#include "Relations/AssociatorWeighted.h"
#include "Relations/RelationWeighted2D.h"

/** @class VeloCluster2MCHitAsct VeloCluster2MCHitAsct.h Associators/VeloCluster2MCHitAsct.h
 *  
 *
 *  @author Chris Parkes
 *  @date   21/05/2002
 */

class VeloCluster2MCHitAsct: public AssociatorWeighted<VeloCluster,MCVeloHit,double>
{
  friend ToolFactory<VeloCluster2MCHitAsct>;
  
public:
  /// Define data types
  typedef RelationWeighted2D<VeloCluster,MCVeloHit,double> Table;
  typedef OwnType                                   Asct;

protected:

  /// Standard constructor
  VeloCluster2MCHitAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent ): 
  Asct( type, name, parent) {
    setProperty( "Location", "MC/VeloClusters2MCHits");
    setProperty( "AlgorithmType", "VeloCluster2MCHitAlg" );
    setProperty( "AlgorithmName", "VeloCluster2MCHit" );
  }; 

  /// Destructor
  virtual ~VeloCluster2MCHitAsct() {}; ///< Destructor

private:

};
#endif // ASSOCIATORS_VeloCluster2MCHitASCT_H


