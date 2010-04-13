// $Id: VeloPixChecker.h,v 1.1.1.1 2007/10/09 18:41:19 smenzeme Exp $
#ifndef VELOPIXCHECKER_H 
#define VELOPIXCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "VeloPixDet/DeVeloPix.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixLiteCluster.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"


/** @class VeloPixChecker VeloPixChecker.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */


  class VeloPixChecker : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VeloPixChecker( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloPixChecker( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    ILHCbIDsToMCHits* linkTool() const;
    
  protected:
    
  private:
    std::string m_tupleName;
    std::string m_container;
    DeVeloPix* m_veloPix;
    IVeloPixClusterPosition* m_positiontool ;
    ILHCbIDsToMCHits* m_linkTool;
    LHCb::VeloPixLiteCluster::VeloPixLiteClusters* m_clusters;
  };


inline ILHCbIDsToMCHits* VeloPixChecker::linkTool() const{
  return m_linkTool;
}

#endif // VELOPIXCHECKER_H
