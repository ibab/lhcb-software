// $Id: STSignalToNoiseTool.h,v 1.3 2007-01-09 15:02:25 jvantilb Exp $
#ifndef STSIGNALTONOISETOOL_H
#define STSIGNALTONOISETOOL_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// LHCbKernel
#include "Kernel/ISTSignalToNoiseTool.h"

class DeSTDetector;
class DeSTSector;

/** @class STSignalToNoiseTool STSignalToNoiseTool.h
 *
 *  ST Signal to noise tool
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STSignalToNoiseTool : public GaudiTool, 
                            virtual public ISTSignalToNoiseTool {

public: 

  /// constructer
  STSignalToNoiseTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent );

  /// destructer
  virtual ~STSignalToNoiseTool();

  /// initialize
  virtual StatusCode initialize();
  
  // S/N   
  virtual double signalToNoise(const LHCb::STDigit* aDigit);
  virtual double signalToNoise(const LHCb::STCluster* aCluster) ;
  virtual double signalToNoise(const SmartRefVector<LHCb::STDigit>& digitCont);

  virtual double noiseInADC(const LHCb::STChannelID& aChan);
  virtual double noiseInElectrons(const LHCb::STChannelID& aChan);
  virtual double noiseInADC(const DeSTSector* aSector);
  virtual double noiseInElectrons(const DeSTSector* aSector);

  virtual double convertToADC(const double aCharge);

private:

  DeSTDetector* m_tracker;
  std::vector<double> m_paramsInADC;

  // job options
  std::vector<double> m_paramsInElectron;
  double m_conversionToADC;
  std::string m_detType;

};


#endif // STSIGNALTONOISETOOL_H




