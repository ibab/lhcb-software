// $Id: TsaFollow.h,v 1.2 2007-03-21 14:35:01 cattanem Exp $
#ifndef _TsaFollow_H
#define _TsaFollow_H

/** @class TsaFollow TsaFollow.h
* Follow track and pick up hits
* @author M. Needham
**/

#include "ITsaFollow.h"

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/Line3D.h"

#include <map>

class DeITStation;
class DeSTLayer;
class IITDataSvc;

namespace Tsa{
  class Cluster;
  class Line;
  class Parabola;
};

class TsaFollow:  public GaudiTool,
                  virtual public ITsaFollow{

public:

  /// constructer
  TsaFollow(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~TsaFollow();

  /// init
  virtual StatusCode initialize();

  // execute method
  virtual StatusCode execute(const unsigned int stationNum,
                             const Tsa::Parabola& parab, 
                             const Tsa::Line& line, 
                             std::vector<Tsa::Cluster*>& clusVector);
private:


  void searchLayer(const DeSTLayer* layer,
                   const Tsa::Parabola& parab, 
                   const Tsa::Line& line, 
                   std::vector<Tsa::Cluster*>& clusVector);


  bool insideLayer(const DeSTLayer* layer, 
                   const Tsa::Line3D& line) const;

  std::map<unsigned int,const DeITStation*> m_stations;
  std::string m_dataSvcName;
  IITDataSvc* m_dataSvc;
  double m_deltaU;
  double m_vTol;

  
  
};

#endif  // _TsaFollow_H

  

