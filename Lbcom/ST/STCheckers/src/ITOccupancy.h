// $Id: ITOccupancy.h,v 1.3 2007-01-04 10:37:36 jvantilb Exp $
#ifndef ITOccupancy_H
#define ITOccupancy_H 1

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
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */


class ITOccupancy : public GaudiHistoAlg {

public:
 
  /// constructer
  ITOccupancy( const std::string& name, ISvcLocator *svcloc );

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

  std::map<int,int> m_mapping;
  std::vector<double> m_nStripsVector;
  std::vector<double> m_layerOccVector;
  std::vector<AIDA::IHistogram1D*> m_layerOccHistos;
  std::vector<AIDA::IHistogram1D*> m_stripOccVector;
  
  ISTSignalToNoiseTool* m_sigNoiseTool;  
  DeSTDetector* m_tracker;

  // job options
  std::vector<double> m_threshold;   ///< List of threshold values
  std::string m_sigNoiseToolName;    ///< Name of the S/N tool 
  int m_binSize;                     ///< Number of channels in each bin
  std::string m_dataLocation;        ///< Location of the digits
  
};

#endif // ITOccupancy_H
