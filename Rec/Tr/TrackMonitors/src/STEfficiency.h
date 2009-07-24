// $Id: STEfficiency.h,v 1.1 2009-07-24 14:03:15 jluisier Exp $
#ifndef STEFFICIENCY2_H 
#define STEFFICIENCY2_H 1

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
class STEfficiency2 : public TrackMonitorBase {
public: 
  /// Standard constructor
  STEfficiency2( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STEfficiency2( ); ///< Destructor

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
  std::string m_expectedHitsTool;
  IHitExpectation* m_expectedHits;
  
  std::vector< double > m_spacialCut;
  double m_xCut, m_stereoCut;
  unsigned int m_NbrOfCuts;
  unsigned int m_minHits;

  std::string m_detType;
  DeSTDetector* m_tracker;

  typedef std::map<unsigned int, unsigned > CounterMap;
  std::vector<CounterMap> m_foundSector, m_foundLayer;
  CounterMap m_expectedSector, m_expectedLayer;
  std::map<unsigned int , DeSTSector*> m_nameMapSector, m_nameMapLayer;
  
  Category ITCategory(const std::vector<LHCb::LHCbID>& ids) const;

  void filterNameList(std::vector< unsigned int>& vec);

  double m_chargeCut;

};
#endif // STEFFICIENCY_H
