// $Id: STSignalToNoiseTool.h,v 1.1.1.1 2005-12-19 15:58:58 mneedham Exp $
#ifndef _STSIGNALTONOISETOOL_H
#define _STSIGNALTONOISETOOL_H

#include "GaudiAlg/GaudiTool.h"
#include "STTools/ISTSignalToNoiseTool.h"

#include <string>

/** @class STSignalToNoiseTool STSignalToNoiseTool.h
 *
 *  ST Signal to noise tool
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class DeSTDetector;
class DeSTSector;

class STSignalToNoiseTool : public GaudiTool, virtual public ISTSignalToNoiseTool {

public: 

  /// constructer
  STSignalToNoiseTool(const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  /// destructer
  virtual ~STSignalToNoiseTool();

  /// init
  virtual StatusCode initialize();

  
  // S/N   
  virtual double signalToNoise(const LHCb::STCluster* aCluster) ;
  virtual double signalToNoise(const SmartRefVector<LHCb::STDigit>& digitCont);


  virtual double signalToNoise(const LHCb::STDigit* aDigit);

  virtual double noiseInADC(const LHCb::STChannelID& aChan);
  virtual double noiseInElectrons(const LHCb::STChannelID& aChan);
  virtual double noiseInADC(const DeSTSector* aSector);
  virtual double noiseInElectrons(const DeSTSector* aSector);

  virtual double convertToADC(const double aCharge);

private:

 double capacitance(const LHCb::STChannelID aChan) const;

 std::vector<double> m_paramsInADC;
 std::vector<double> m_paramsInElectron;

 double m_conversionToADC;
 

 std::string m_detType;
 DeSTDetector* m_tracker;

};


#endif // _STSIGNALTONOISETOOL_H




