// $Id: ITOccupancy.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _ITOccupancy_H
#define _ITOccupancy_H

#include <map>
#include <vector>
#include <string>

#include "GaudiAlg/GaudiHistoAlg.h"

class AIDA::IHistogram1D;

namespace LHCb{
 class STDigit;
 class STChannelID;
};

class DeSTDetector;

class ISTSignalToNoiseTool;

/** @class ITOccupancy ITOccupancy.h
 *
 *  Class for checking STDigits
 *
 *  @author M.Needham
 *  @date   21/4/20014
 */


class ITOccupancy : public GaudiHistoAlg {

public:
 
  /// constructer
  ITOccupancy(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~ITOccupancy();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  virtual StatusCode initHistograms();
  virtual StatusCode fillHistograms(const LHCb::STDigit* aDigit);

  int uniqueInt(const LHCb::STChannelID aChan) const;
    
  double binValue(const unsigned int strip) const;

  std::map<int,int> m_Mapping;
  std::vector<double> m_nStripsVector;
  std::vector<double> m_layerOccVector;
  std::vector<AIDA::IHistogram1D*> m_layerOccHistos;
  std::vector<AIDA::IHistogram1D*> m_stripOccVector;

  std::string m_dataLocation;
  
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;
  
  std::string m_detType;
  DeSTDetector* m_tracker;


  std::vector<double> m_Threshold;   
  int m_binSize;
  
};



#endif // _ITOccupancy_H













