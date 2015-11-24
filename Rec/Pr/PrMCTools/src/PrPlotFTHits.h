// $Id: $
#ifndef PRPLOTFTHITS_H
#define PRPLOTFTHITS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


// local
#include "PrKernel/PrHitManager.h"
#include "PrKernel/PrHit.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Event/Track.h"
#include "Event/MCTrackInfo.h"
#include "MCInterfaces/IIdealStateCreator.h"
#include "MCInterfaces/ILHCbIDsToMCParticles.h"
#include "MCInterfaces/ILHCbIDsToMCHits.h"
#include "Event/FTCluster.h"
#include "Event/FTLiteCluster.h"
#include "Event/MCHit.h"
#include "GaudiAlg/GaudiHistoTool.h"

/** @class PrPlotFTHits PrPlotFTHits.h
 *  Make plots of hitmaps / efficiencies / MCHits in the FT
 *
 *  - ExcludeElectrons: Exclude electrons for plots
 *  - PlotHitEfficiency: Switch to plot the hit efficiencies
 *  - PlotAllFTHits: Plot all FT hits in the detector
 *  - PlotFTHitsOnTrack: Plot all FT hits which belong to a track.
 *  - PlotTrackingEfficiency: Plot the tracking efficiency in 2D.
 *  - PlotMCHits: Plot the MC hits belonging to FT hits
 *  - OnlyLongAndDownForMCHits: Only plot MCHits that belong to long and downstream reconstructible tracks?
 *  - PlottaOccupancy: Plot the occupancy in the different layers disentangling noise/spill(next and prev)/from MCParticle
 *  - PlotStates: Plot the Delta state x,y,tx,ty for all the Track state wrt to the IdealStateCreator of the associated MCParticle from forward and seeding output containers
 *  Note that this tool needs MCHits and may therfore not run on all input files.
 *  Furthermore note that the location of MCHits has to be mapped correctly, like:
 *  
 *  @code 
 *  from Configurables import MCParticle2MCHitAlg
 *  FTAssoc = MCParticle2MCHitAlg( "MCP2FTMCHitAlg", MCHitPath = "MC/FT/Hits", OutputData = "/Event/MC/Particles2MCFTHits" )
 *  # tell the Data On Demand Service about them
 *  DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCFTHits" ]    = FTAssoc
 *  DataOnDemandSvc().NodeMap[ "/Event/Link" ]    = "DataObject"
 *  DataOnDemandSvc().NodeMap[ "/Event/Link/MC" ] = "DataObject"
 *  @endcode
 * 
 *  Also note that the IdealStateCreator needs to be given the correct detectors to find the MCHits it needs internally
 *  
 *  @code
 *  from Configurables import IdealStateCreator
 *  plotFTHits.addTool( IdealStateCreator("IdealStateCreator"))
 *  plotFTHits.IdealStateCreator.Detectors = [ "FT" ]
 *  @endcode
 *
 *  
 *  @author Michel De Cian
 *  @date   2014-05-08
 */

class IHistoTool ;

class PrPlotFTHits : public GaudiHistoAlg {
  
public:
  /// Standard constructor
  PrPlotFTHits( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrPlotFTHits( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  
  

protected:


private:
  /** @brief Plot the occupancy in the detector layer by layer 
   */
  void plotOccupancy();
  
  
  /** @brief Plot the Track state distance in x,y,tx,ty
   */
  void plotState();
      
  /** @brief Plot the hit efficiency in the FT.
   */
  void plotHitEfficiency();

  /** @brief Plot all FT hits.
   */
  void plotAllFTHits();

  /** @brief Plot all FT hits which belong to a track.
   */
  void plotFTHitsOnTrack();

  /** @brief Plot tracking efficiency in 2D.
   */
  void plotTrackingEfficiency();

  /** @brief Plot MC hits which belong to a particle
   */
  void plotMCHits();
  
  /** @brief Get the PrHit (and the zone) corresponding to an LHCbID 
   *  @param id LHCbID of the cluster
   *  @param zone Zone the PrHit is in.
   *  @return PrHit The PrHit
   */
  const PrHit* getPrHit(const LHCb::LHCbID id, unsigned int& zone);
  
  /** @brief Get the track(s) where the LHCbID is on.
   *  @param id LHCbID of the cluster
   *  @param location Location of the tracks.
   *  @return std::vector vector of tracks
   */
  std::vector<const LHCb::Track*> getTrack(const LHCb::LHCbID id, const std::string location);

  /** @brief Get the cluster corresponding to an LHCbID  
   *  @param id LHCbID of the hit
   *  @return LHCb::FTCluster The FT cluster
   */
  LHCb::FTCluster* getCluster(const LHCb::LHCbID id);
  LHCb::FTLiteCluster getLiteCluster(const LHCb::LHCbID id);

  const IHistoTool* m_histoTool;
  GaudiHistoTool* m_ghtool;

  /// The FT hit manager
  PrHitManager* m_ftHitManager;
  
  /// The track extrapolator
  ITrackExtrapolator* m_extrapolator;

  /// The ideal state creator, which makes a state out of an MCParticle
  IIdealStateCreator* m_idealStateCreator;
  
  unsigned int m_zone;
  
  bool m_plotOccupancy;
  bool m_excludeElectrons;
  bool m_plotHitEfficiency;
  bool m_plotAllFTHits;
  bool m_plotFTHitsOnTrack;
  bool m_plotTrackingEfficiency;
  bool m_plotMCHits;
  bool m_onlyLongDownForMCHits;
  bool m_plotState;
  /// Type for container of Hits
  typedef std::vector< PrHit* > Hits;
  /// range of object for Hits
  typedef Gaudi::Range_< Hits > HitRange;
  // Clusters container
  typedef FastClusterContainer<LHCb::FTLiteCluster,int> FTLiteClusters;
     
};

#endif // PRPLOTFTHITS_H
