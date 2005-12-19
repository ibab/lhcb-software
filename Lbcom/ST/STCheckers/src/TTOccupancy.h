// $Id: TTOccupancy.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _TTOccupancy_H
#define _TTOccupancy_H

#include <map>
#include <vector>
#include <string>

#include "GaudiAlg/GaudiHistoAlg.h"

class AIDA::IHistogram1D;

namespace LHCb{
 class STDigit;
};

class DeSTDetector;
class ISTSignalToNoiseTool;

/** @class TTOccupancy TTOccupancy.h
 *
 *  Class for checking TTDigits
 *
 *  @author M.Needham
 *  @date   21/4/20014
 */


class TTOccupancy : public GaudiHistoAlg {

public:
 
  /// constructer
  TTOccupancy(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~TTOccupancy();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  virtual StatusCode initHistograms();
  virtual StatusCode fillHistograms(const LHCb::STDigit* aDigit);

  int uniqueInt(unsigned int iStation, unsigned int iLayer, 
                unsigned int iCol) const;

    
  double binValue(const unsigned int strip) const;

  std::string m_dataLocation;
  
  std::map<int,int> m_Mapping;
  std::vector<AIDA::IHistogram1D*> m_stripOccVector;
  
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;
  
  DeSTDetector* m_tracker;
  std::vector<double> m_Threshold;   
  int m_binSize;
  
};

#endif // _TTOccupancy_H













