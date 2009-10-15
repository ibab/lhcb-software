// $Id: STEfficiency.h,v 1.4 2009-10-15 08:57:49 jluisier Exp $
#ifndef STEFFICIENCY_H 
#define STEFFICIENCY_H 1

// Include files
#include "TrackMonitorBase.h"

#include "TrackInterfaces/ISTClusterCollector.h"

class ISTClusterCollector;
class IHitExpectation;
class ITrackSelector;
class DeSTDetector;
class DeSTSector;

#include <map>
#include <vector>

/** @class STEfficiency STEfficiency.h jluisier/STEfficiency.h
 *  
 *
 *  @author Johan Luisier
 *  @date   2009-06-16
 */
class STEfficiency : public TrackMonitorBase {
public: 
  /// Standard constructor
  STEfficiency( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STEfficiency( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum Category  {CSide = 1 , ASide =2, Bottom = 3, Top =4 , Mixed =5 };

protected:

private:

  bool foundHitInSector( const ISTClusterCollector::Hits& hits,
			 const unsigned int testsector,
			 const double resCut  ) const;

  bool foundHitInLayer( const ISTClusterCollector::Hits& hits,
			 const unsigned int testlayer,
			 const double resCut  ) const;

  std::string m_collectorName, m_trackSelectorName;
   /**
    * Collects the clusters in the wide window, used to get the efficiency
    */
  std::vector<ISTClusterCollector* > m_collectors;

  /**
   * Collects clean tracks
   */
  ITrackSelector  *m_trackSelector;
  /**
   * Name of the expected hits tool
   */
  std::string m_expectedHitsTool;
  /**
   * Collects all the expeted hits around a track
   */
  IHitExpectation* m_expectedHits;
  
  /**
   * Cuts applied to get the scan efficiency vs window
   */
  std::vector< double > m_spacialCut;
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
  
  Category ITCategory(const std::vector<LHCb::LHCbID>& ids) const;

  void filterNameList(std::vector< unsigned int>& vec);

  /**
   * Cut applied on the cluster charge, in terms of signal to noise.
   */
  double m_chargeCut;

  /**
   * Prefix added to the collector names used to get the hits. Default
   * value is "". SHould be used to study spill over.
   */
  std::string m_collectorPrefix;

};
#endif // STEFFICIENCY_H
