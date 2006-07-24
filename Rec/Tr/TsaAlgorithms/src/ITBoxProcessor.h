#ifndef _ITBoxProcessor_H
#define _ITBoxProcessor_H

/** @class ITBoxProcessor ITBoxProcessor.h
* Create spacepoints in an inner tracker box
*
* @author M. Needham
*/

#include "GaudiAlg/GaudiTool.h"

#include "TsaKernel/IITBoxProcessor.h"
#include "TsaKernel/STCluster.h"
#include "TsaKernel/SpacePoint.h"
#include "TsaKernel/ClusFun.h"


#include <vector>
#include <string>
#include <utility>

class IITDataSvc;
class ITDataCache;

class ITBoxProcessor: public GaudiTool,
                 virtual public IITBoxProcessor{

public:

  typedef Tsa::STClusters::const_iterator iter_type;

  /// constructer
  ITBoxProcessor(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  // destructer
  virtual ~ITBoxProcessor();

  /// init
  virtual StatusCode initialize();

  /// method
  virtual StatusCode execute(Tsa::SpacePointVector* pCont);

  /// type
  std::string boxType() const;

private:

  typedef std::vector<ITDataCache*> dataCont;
  
  void initCache();
  void initializeEvent();
  void xuvx(Tsa::SpacePointVector* pCont);
  void xux(Tsa::SpacePointVector* pCont);
  void xuv(Tsa::SpacePointVector* pCont);

  Tsa::SpacePoint* create4point(Tsa::STCluster* clusX1, Tsa::STCluster* clusX2,
                              Tsa::STCluster* clusU, Tsa::STCluster* clusV, 
                              double tX, double tY, Gaudi::XYZPoint aPoint); 

  Tsa::SpacePoint* create3point(Tsa::STCluster* clus1, Tsa::STCluster* clus2,
                                Tsa::STCluster* clus3, 
                                double tX,  Gaudi::XYZPoint aPoint); 


  bool used(const Tsa::STCluster* aCluster) const;

  void cloneFind(Tsa::SpacePointVector* pCont) const;

  typedef std::pair<bool,int> clonePair;
  clonePair isClone(const Tsa::SpacePoint* point1,
                    const Tsa::SpacePoint* point2) const;


  std::string m_dataSvcName;
  IITDataSvc* m_dataSvc;
  dataCont m_data;

  std::string m_sType;
  Tsa::SpacePoint::pointType m_eType;
  unsigned int m_station;


  bool m_missingX;
  unsigned int m_nEmpty;
 
  double  m_cutSlopeX1;
  double  m_cutSlopeX2;
  double m_vTol;
  double m_cutDeltaY;

  unsigned int m_pass;  

  bool m_cloneSearch;
  double m_uClone;

};


inline bool ITBoxProcessor::used(const Tsa::STCluster* aCluster) const{
  // used in previous pass
  return ((aCluster->pass() != 0) && (aCluster->pass() != m_pass));
}



#endif // ITBoxProcessor

