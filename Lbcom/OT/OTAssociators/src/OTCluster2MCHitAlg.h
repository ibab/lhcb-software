// $Id: OTCluster2MCHitAlg.h,v 1.2 2002-07-05 10:11:41 jvantilb Exp $
#ifndef OTCLUSTER2MCHITALG_H
#define OTCLUSTER2MCHITALG_H 1

#include <string>
#include <map>


#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"

class OTCluster;
class MCHit;

/** @class OTCluster2MCHitAlg OTCluster2MCHitAlg.h
 *  
 *
 *  @author Jeroen van Tilburg
 *  @date   14/05/2002
 */
class OTCluster2MCHitAlg : public Algorithm {
    friend AlgFactory<OTCluster2MCHitAlg>;
  
public:

  /// Standard constructor
  OTCluster2MCHitAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTCluster2MCHitAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTCluster* aCluster,
                                      MCHit*& aHit);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  std::string m_outputData;
  /// Flag to make relations with spillover hits as well
  bool m_spillOver;

  MCHits* m_mcHits;
};

inline std::string OTCluster2MCHitAlg::outputData() const {
  return  m_outputData;
}


#endif // OTCLUSTER2MCHITALG_H







