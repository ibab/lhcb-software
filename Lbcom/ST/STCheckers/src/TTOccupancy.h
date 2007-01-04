// $Id: TTOccupancy.h,v 1.4 2007-01-04 10:37:37 jvantilb Exp $
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
  
  ISTSignalToNoiseTool* m_sigNoiseTool;
  DeSTDetector* m_tracker;
  int m_nBins; 
  int m_hMax;  

  // job options
  std::vector<double> m_threshold;   ///< List of threshold values
  std::string m_sigNoiseToolName;    ///< Name of the S/N tool 
  int m_binSize;                     ///< Number of channels in each bin
  std::string m_dataLocation;        ///< Location of the digits

};

#endif // TTOccupancy_H
