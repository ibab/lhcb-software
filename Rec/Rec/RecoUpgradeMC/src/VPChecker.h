// $Id: VPChecker.h,v 1.1.1.1 2007/10/09 18:41:19 smenzeme Exp $
#ifndef VPCHECKER_H 
#define VPCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "VPDet/DeVP.h"
#include "TrackInterfaces/IVPClusterPosition.h"
#include "VPDet/DeVP.h"
#include "Event/VPLiteCluster.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"


/** @class VPChecker VPChecker.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */


  class VPChecker : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VPChecker( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VPChecker( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    ILHCbIDsToMCHits* linkTool() const;
    
  protected:
    
  private:
    std::string m_tupleName;
    std::string m_container;
    DeVP* m_vP;
    IVPClusterPosition* m_positiontool ;
    ILHCbIDsToMCHits* m_linkTool;
    LHCb::VPLiteCluster::VPLiteClusters* m_clusters;
  };


inline ILHCbIDsToMCHits* VPChecker::linkTool() const{
  return m_linkTool;
}

#endif // VPCHECKER_H
