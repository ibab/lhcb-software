// $Id: OTCluster2MCDepositAsct.h,v 1.3 2002-09-27 09:41:04 jvantilb Exp $
#ifndef OTASSOCIATORS_OTCLUSTER2MCDEPOSITASCT_H 
#define OTASSOCIATORS_OTCLUSTER2MCDEPOSITASCT_H 1

// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/OTCluster.h"
#include "Event/MCOTDeposit.h"

// Associators
#include "Relations/Associator.h"
#include "Relations/Relation1D.h"

static const std::string& OTCluster2MCDepositLocation =
                       "Rec/Relations/OTClusters2MCDeposits";

/** @class OTCluster2MCDepositAsct OTCluster2MCDepositAsct.h \
 *         "OTAssociators/OTCluster2MCDepositAsct.h"
 *  
 *  Associator for the OTClusters with the corresponding MCOTDeposits. 
 *  By default OTClusters coming from spillover are also associated.

 *  @author J. van Tilburg
 *  @date   14/05/2002
 */

class OTCluster2MCDepositAsct: public Associator<OTCluster,MCOTDeposit>
{
  friend ToolFactory<OTCluster2MCDepositAsct>;
  
public:
  // Define data types
  typedef Relation1D<OTCluster,MCOTDeposit>         Table;
  typedef OwnType                                   Asct;
  typedef FromRange                                 Clusters;
  typedef FromIterator                              ClustersIterator;
  typedef ToRange                                   MCDeposits;
  typedef ToIterator                                MCDepositsIterator;
    
private:

  /// Standard constructor
  OTCluster2MCDepositAsct(const std::string& type,
                          const std::string& name,
                          const IInterface* parent ) 
  :Asct( type, name, parent) {
    setProperty( "Location", OTCluster2MCDepositLocation);
    setProperty( "AlgorithmType", "OTCluster2MCDepositAlg" );
    setProperty( "AlgorithmName", "OTCluster2MCDeposit" );
  }; 

  /// Destructor
  virtual ~OTCluster2MCDepositAsct() {}; ///< Destructor


};
#endif // OTASSOCIATORS_OTCLUSTER2MCDEPOSITASCT_H
