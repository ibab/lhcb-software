#ifndef STEFFICIENCY_H 
#define STEFFICIENCY_H 1

// Include files
#include "TrackMonitorBase.h"

#include "TrackInterfaces/ISTClusterCollector.h"

#include "Event/STCluster.h"

class ISTClusterCollector;
class IHitExpectation;
class DeSTDetector;
class DeSTSector;

#include <map>
#include <vector>

/** @class STEfficiency STEfficiency.h
 *  
 *
 *  @author Johan Luisier, Frederic Dupertuis
 *  @date   2009-06-16, 2010-07-27
 */
class STEfficiency : public TrackMonitorBase {
public: 
  /// Standard constructor
  STEfficiency( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~STEfficiency( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  unsigned int TThistoBin(const LHCb::STChannelID& chan) const;
  unsigned int IThistoBin(const LHCb::STChannelID& chan) const;

  bool foundHitInSector( const ISTClusterCollector::Hits& hits,
                         LHCb::Track* const& track,
                         const unsigned int testsector,
                         const double resCut,
                         const bool toplot ) const;

  std::string formatNumber( const double& nbr, const unsigned int& digits = 2u ) const;

  bool hasMinStationPassed(LHCb::Track* const&) const;

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
  
  void filterNameList(std::vector< unsigned int>& vec);

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
   * Cut on the active region X
   */
  double m_minDistToEdgeX;

  /**
   * Cut on the active region Y
   */
  double m_minDistToEdgeY;

  /**                                                                                                           
   * Cut on minimum station that the track must passed through
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

  bool m_2DSummaryHist;  
  bool m_ProfileSummaryHist;  

    
};
#endif // STEFFICIENCY_H
