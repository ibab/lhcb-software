// $Id: TsaFollow.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _TsaFollow_H
#define _TsaFollow_H

/** @class TsaFollow TsaFollow Tsa/TsaFollow
* Follow track and pick up hits
* @author M. Needham
**/

#include "ITsaFollow.h"

#include "GaudiAlg/GaudiTool.h"

class DeSTDetector;
class IITDataSvc;
class TsaCluster;

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


  DeSTDetector* m_tracker;
  std::string m_dataSvcName;
  IITDataSvc* m_dataSvc;
  double m_deltaU;
  double m_vTol;
  
};

#endif  // _TsaFollow_H

  

