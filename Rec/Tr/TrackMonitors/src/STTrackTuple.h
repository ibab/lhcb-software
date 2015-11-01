#ifndef STTRACKTUPLE_H 
#define STTRACKTUPLE_H 1

// Include files
#include "TrackMonitorTupleBase.h"
#include "TrackInterfaces/ISTClusterCollector.h"
#include "Event/STCluster.h"
#include "Event/STMeasurement.h"
#include "STDet/DeSTLayer.h"

class ISTClusterCollector;
class IHitExpectation;
class DeSTDetector;
class DeSTSector;
class DeSTSensor;

#include <map>
#include <vector>

/** @class STTrackTuple STTrackTuple.h
 *  
 *
 *  @author Johan Luisier, Frederic Dupertuis, Ilya Komarov and Elena Graverini
 *  @date   2009-06-16, 2010-07-27, 2015-04-15
 */
class STTrackTuple : public TrackMonitorTupleBase {
public: 
  /// Standard constructor
  STTrackTuple( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~STTrackTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  bool foundHitInSector( const ISTClusterCollector::Hits& hits,
                         LHCb::Track* const& track,
                         const unsigned int testsector,
                         const double resCut,
                         const bool toplot ) const;

  bool foundHitInLayer( const ISTClusterCollector::Hits& hits,
                         LHCb::Track* const& track,
                         const unsigned int testlayer,
                         const double resCut) const;

  double foundResInSector( const ISTClusterCollector::Hits& hits,
                         LHCb::Track* const& track,
                         const unsigned int testsector,
                         const double resCut) const;

  std::string formatNumber( const double& nbr, const unsigned int& digits = 2u ) const;

  bool hasMinStationPassed(const LHCb::Track* ) const;

  bool crossedLayer(const LHCb::Track* aTrack,
                                DeSTLayer * &aLayer) const;
  unsigned int hitsOnLayer(const ISTClusterCollector::Hits& hits,
                                DeSTLayer * aLayer) const;

  unsigned int hitsOnLayer(const std::vector<const LHCb::STMeasurement*>& measVector,
                                     DeSTLayer * aLayer) const;

  DeSTSensor* findSensor(const DeSTSector* sector,
                                    LHCb::STChannelID id);

  LHCb::LHCbID findHitId(const LHCb::Track* aTrack,
                          const LHCb::STMeasurement* aHit) const;
  
  double retrieveErrResidual2(const LHCb::Track* aTrack,
                                        LHCb::LHCbID aID) const;


  std::string m_clustercollectorName;

  ISTClusterCollector* m_clustercollector;
  
  /**
   * Name of the expected hits tool
   */
  std::string m_expectedHitsToolName;
  /**
   * Collects all the expeted hits around a track
   */
  IHitExpectation* m_expectedHits;
    /**
   * Cuts applied to get the scan efficiency vs window
   */
  std::vector< double > m_spacialCuts;
  double m_xCut, /**< Applied cut to compute X layer efficiencies */
    m_stereoCut; /**< Applied cut to compute stereo layer
		    efficiencies */
  /**
   * Default cut to check if hit exists in layer
   */
  double m_defaultCut;
  /**
   * Total number of expected hits
   */
  unsigned int m_totalExpected;
  /**
   * Total number of found hits
   */
  unsigned int m_totalFound;
  /**
   * Indicates what is the applied cut, depending on the layer.
   */
  unsigned int m_whichCut[2];
  /**
   * Size of the m_spacialCut container
   */
  unsigned int m_NbrOfCuts;
  /**
   * Min number of expected hits for the track to be accepted.
   */
  unsigned int m_minHits;
  /**
   * Detector type, can be either IT or TT.
   */
  std::string m_detType;
  /**
   * Detector element, automatically retrieved.
   */
  DeSTDetector* m_tracker;

  typedef std::map<unsigned int, unsigned > CounterMap;
  std::vector<CounterMap> m_foundSector, m_foundLayer;
  CounterMap m_expectedSector, m_expectedLayer;
  std::map<unsigned int , DeSTSector*> m_nameMapSector, m_nameMapLayer;
  
  /**
   * Cut applied on the cluster charge, in terms of signal to noise.
   */
  double m_minCharge;
  /**
   * Prefix added to the collector names used to get the hits. Default
   * value is "". SHould be used to study spill over.
   */
  std::string m_collectorPrefix;
  /**
   * Size of a bin, derived from the spacial cuts given in option
   */
  double m_binSize;
  /**
   * Number of bins, derived from the spacial cuts given in option
   */
  unsigned int m_binNumber;
  /**
   * Minimum number of expected hits in order to compute an efficiency
   * Default value is 100.
   */
  unsigned int m_minExpSectors;
  /**
   * 
   */
  int m_minNbResSectors;
  /**
   * 
   */
  int m_maxNbResSectors;
  /**
   * 
   */
  bool m_singlehitpersector;
  /**
   * Exclude dead regions of the sensors
   */
  bool m_skipEdges;
  /**
   * List of wanted track types that will be used to compute efficiency.
   * Default is only Long (type 3)
   */
  std::vector< unsigned int > m_wantedTypes;
  /**
   * Toggles off / on picking only hits from the track.\n
   * \e true => every found hit is taken\n
   * \e false => only hits picked up by the pattern recognition are
   * taken into account.
   */
  bool m_everyHit;
  /**
   * Toggles off / on picking only hits from the track.\n
   * \e false => every found hit is taken\n
   * \e true => only hits picked up by the pattern recognition are
   * taken into account.
   */
  bool m_hitsOnTrack;
  /**
   * Cut on the active region X
   */
  double m_minDistToEdgeX;
  /**
   * Cut on the active region Y
   */
  double m_minDistToEdgeY;
  /**                                                                                                           
   * Cut on minimum stations that the track must cross
   */
  unsigned int m_minStationPassed;
  /**
   * Plot efficiency plot
   */
  bool m_effPlot;
  /**
   * Plot residuals plot
   */
  bool m_resPlot;
  /**
   * Ntuple branched by sector names
   */
  bool m_branchBySector;
  /**
   * Ntuple branched by track
   */
  bool m_branchByTrack;
  /**
   * Save sector positions in ntuple
   */
  bool m_savePositions;

};
#endif // STTRACKTUPLE_H

