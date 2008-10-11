// $Id: ITGenericTracking.h,v 1.3 2008-10-11 10:48:15 mneedham Exp $
#ifndef ITGenericTracking_H
#define ITGenericTracking_H 1

#include "GaudiAlg/GaudiHistoAlg.h"
#include "TfKernel/STHit.h"
#include "TsaKernel/Line.h"


namespace LHCb{
  class STCluster;
};
class ISTSignalToNoiseTool;
class DeSTDetector;
class IHitExpectation;

/** @class ITGenericTracking ITGenericTracking.h
 *
 *  Class for monitoring STClusters
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class ITGenericTracking : public GaudiHistoAlg {

public:
 
  /// constructer
  ITGenericTracking( const std::string& name, 
                    ISvcLocator *svcloc );

  /// destructer
  virtual ~ITGenericTracking();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:


  typedef std::pair<Tf::STHit*,Gaudi::XYZPoint> yInfo;

  bool collectXHits13(const Tf::Tsa::Line& line,
		      Tf::STHit* hit1, Tf::STHit* hit2,
   		      const std::vector<Tf::STHit*>& xhits,  
                      std::vector<Tf::STHit*>& selected) const;

  bool collectXHits2(const Tf::Tsa::Line& line,
                     Tf::STHit* hit1,
		     const std::vector<Tf::STHit*>& xhits,  
                     std::vector<Tf::STHit*>& selected) const;



  void collectYHits(const Tf::Tsa::Line& line,
		    const std::vector<Tf::STHit*>& yhits, const Tf::STHit* hit,  std::vector<yInfo>& selected) const;

  void selectY(const std::vector<yInfo>& hits, std::vector<Tf::STHit*>& selected, Tf::Tsa::Line& yLine ) const; 

  void collectIDs( std::vector<Tf::STHit*> hits , std::vector<LHCb::LHCbID>& ids)  const;

  void selectX(const std::vector<Tf::STHit*>& hits, 
               const Tf::Tsa::Line& yLine, 
               std::vector<Tf::STHit*>& selected2 );

  bool allowedBox(const Tf::STHit* hit) const;
  bool allowedFirstStation(const Tf::STHit* hit) const;
  bool allowedLastStation(const Tf::STHit* hit) const;
  bool sameBox(const Tf::STHit* hit1, const Tf::STHit* hit2) const;

  unsigned int countHigh(const std::vector<Tf::STHit*>& xhits, const std::vector<Tf::STHit*>& yhits, 
                         const LHCb::STClusters* clusterCont) const;

  IHitExpectation* m_hitExpectation;
  IHitExpectation* m_otHitExpectation;
  DeSTDetector* m_tracker;
  
  std::vector<unsigned int> m_allowedBoxes;
  std::vector<unsigned int> m_firstStation;
  std::vector<unsigned int> m_lastStation;
  

  std::string m_clusterLocation;

  bool m_requireFirstAndLast;
 
  double m_xWindow1;
  double m_xWindow2;
  double m_yWindow;
  double m_yTol;
  unsigned int m_maxHits;
  int m_maxFaults;

  double m_zRef;
  double m_maxTx;
  double m_maxTy;
  double m_maxRefX;
  double m_maxRefY;

  unsigned int m_minYHits;
  unsigned int m_minXHits;
  unsigned int m_minHits;
  unsigned int m_minXHitsToConfirm;
  bool m_requireSameBox; 
  unsigned int m_maxClusterSize;
  double m_minCharge;

};


inline bool ITGenericTracking::allowedBox(const Tf::STHit* hit) const {
  const unsigned int testBox = hit->cluster().detRegion();
  return (std::find(m_allowedBoxes.begin(), m_allowedBoxes.end(), testBox) == m_allowedBoxes.end() ? false : true ); 
}


inline bool ITGenericTracking::allowedFirstStation(const Tf::STHit* hit) const {
  const unsigned int station = hit->cluster().station();
  return (std::find(m_firstStation.begin(), m_firstStation.end(), station) == m_firstStation.end() ? false : true ); 
}

inline bool ITGenericTracking::allowedLastStation(const Tf::STHit* hit) const {
  const unsigned int station = hit->cluster().station();
  return (std::find(m_lastStation.begin(), m_lastStation.end(), station) == m_lastStation.end() ? false : true ); 
}


inline bool ITGenericTracking::sameBox(const Tf::STHit* hit1, const Tf::STHit* hit2) const{
  return(m_requireSameBox == true ? hit1->cluster().detRegion() == hit2->cluster().detRegion() : true);
}

#endif // MCITGenericTracking_H
