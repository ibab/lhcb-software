// $Id: OTCluster2MCHitAsct.h,v 1.3 2002-08-07 15:55:06 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCHITASCT_H 
#define OTASSOCIATORS_OTCLUSTER2MCHITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTCluster.h"
#include "Event/MCHit.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTCluster2MCHitLocation =
                       "Rec/Relations/OTClusters2MCHits";

/** @class OTCluster2MCHitAsct OTCluster2MCHitAsct.h \
 *         "OTAssociators/OTCluster2MCHitAsct.h"
 *  
 *  Associator for the OTClusters with the corresponding MCHits. 
 *  By default OTClusters coming from spillover are not associated.
 * 
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */

class OTCluster2MCHitAsct: public Associator<OTCluster,MCHit>
{
  friend ToolFactory<OTCluster2MCHitAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTCluster,MCHit>               Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Clusters;
  typedef FromIterator                              ClustersIterator;
  typedef ToRange                                   MCHits;
  typedef ToIterator                                MCHitsIterator;
    
private:

  /// Standard constructor
  OTCluster2MCHitAsct(const std::string& type,
                      const std::string& name,
                      const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTCluster2MCHitLocation);
    setProperty( "AlgorithmType", "OTCluster2MCHitAlg" );
    setProperty( "AlgorithmName", "OTCluster2MCHit" );
  }; 

  /// Destructor
  virtual ~OTCluster2MCHitAsct() {}; ///< Destructor

};
#endif // OTASSOCIATORS_OTCLUSTER2MCHITASCT_H

