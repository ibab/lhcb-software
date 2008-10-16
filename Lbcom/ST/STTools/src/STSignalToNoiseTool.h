// $Id: STSignalToNoiseTool.h,v 1.4 2008-10-16 13:05:27 mneedham Exp $
#ifndef STSIGNALTONOISETOOL_H
#define STSIGNALTONOISETOOL_H 1

// Gaudi
#include "Kernel/STToolBase.h"

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

class STSignalToNoiseTool : public ST::ToolBase, 
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


  std::vector<double> m_paramsInADC;

  // job options
  std::vector<double> m_paramsInElectron;
  double m_conversionToADC;


};


#endif // STSIGNALTONOISETOOL_H




