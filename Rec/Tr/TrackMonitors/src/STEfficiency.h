// $Id: STEfficiency.h,v 1.3 2009-08-20 08:18:46 mneedham Exp $
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
  std::string m_expectedHitsTool;
  IHitExpectation* m_expectedHits;
  
  std::vector< double > m_spacialCut;
  double m_xCut, m_stereoCut;
  unsigned int m_totalExpected;
  unsigned int m_totalFound;
  unsigned int m_whichCut[2];
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
