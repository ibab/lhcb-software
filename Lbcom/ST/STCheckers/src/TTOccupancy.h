// $Id: TTOccupancy.h,v 1.3 2006-12-22 12:23:01 jvantilb Exp $
#ifndef TTOccupancy_H
#define TTOccupancy_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class STDigit;
};

class DeSTDetector;
class ISTSignalToNoiseTool;

/** @class TTOccupancy TTOccupancy.h
 *
 *  Class for plotting the occupancy of TTDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */


class TTOccupancy : public GaudiHistoAlg {

public:
 
  /// constructer
  TTOccupancy( const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~TTOccupancy();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  virtual StatusCode fillHistograms(const LHCb::STDigit* aDigit);

  double binValue(const unsigned int strip) const;

  std::string m_dataLocation;  
  
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;
  
  DeSTDetector* m_tracker;
  std::vector<double> m_threshold;   
  int m_binSize;
  int m_nBins;
  int m_hMax;  

};

#endif // TTOccupancy_H
