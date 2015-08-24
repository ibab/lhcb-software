// $Id: VeloIPResolutionMonitorNT.h,v 1.9 2010/04/02 16:09:45 malexand Exp $
#ifndef VELORECMONITORS_VELOIPRESOLUTIONMONITORNT_H 
#define VELORECMONITORS_VELOIPRESOLUTIONMONITORNT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/IPVOfflineTool.h"
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

#include "Event/RecVertex.h"

#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Kernel/Particle2MCLinker.h"

/** @class VeloIPResolutionMonitorNT VeloIPResolutionMonitorNT.h
 *  
 *  An algorithm to monitor IP resolutions as a function of 1/PT.
 *  The IP of tracks used to make a primary vertex is taken on that primary vertex.
 *  Assuming these tracks are from prompt particles, the true IP should be 0, so the
 *  calculated IP is in fact the resolution on the IP. Histograms produced are the 
 *  mean of the absolute unsigned 3D IP resolution against 1/PT, 
 *  and the width of a Gaussian fitted to each 1D IP resolution against 1/PT.
 *
 *  @author Michael Thomas Alexander
 *  @date   2009-05-15
 */

namespace Velo
{
  
  class VeloIPResolutionMonitorNT : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VeloIPResolutionMonitorNT( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloIPResolutionMonitorNT( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:

  private:

    std::string m_vertexLocation ;
    std::string m_trackLocation ;

    const LHCb::RecVertex* m_pv;
    const LHCb::Track* m_track;

    bool m_withMC;

    bool m_refitPVs;
    bool m_checkIDs ;
    float m_checkFrac ;
    std::string m_trackExtrapolatorType ;
    std::string m_materialLocatorType ;

    ITrackExtrapolator* m_trackExtrapolator;
    IPVOfflineTool* m_pvtool;
    IMaterialLocator* m_materialLocator ;
    IHitExpectation* m_TTExpectTool;
    IVeloExpectation* m_VeloExpectTool;
  
    Gaudi::XYZPoint extrapolateToZ(const LHCb::Track*, double );
    Gaudi::XYZPoint extrapolateToPOCA(const LHCb::Track*, Gaudi::XYZPoint );    

    StatusCode calculateIPs( const LHCb::RecVertex*, const LHCb::Track*, 
                             double&, double&, double&, double&, double&, double&, LHCb::State&, LHCb::State& );
    void distance( const LHCb::RecVertex*, LHCb::State, double&, double&, int );
    
    StatusCode checkMCAssoc( const LHCb::Track*, const LHCb::RecVertex*, const LHCb::MCVertex*&, Gaudi::LorentzVector&, unsigned int& );

    const LHCb::Track* matchTrack(const LHCb::Track&, const LHCb::RecVertex&) const ;
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
