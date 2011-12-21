#ifndef ITOccupancy_H
#define ITOccupancy_H 1

#include "Kernel/STHistoAlgBase.h"

namespace AIDA {
  class IHistogram1D;
}

namespace LHCb{
 class STChannelID;
}

class DeSTDetector;



/** @class ITOccupancy ITOccupancy.h
 *
 *  Class for checking STDigits
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */


template <class PBASE>
class ITOccupancy : public ST::HistoAlgBase {

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

  const std::string& dataLocation() const;
 
  const std::string histoDirName() const;

  double defaultThreshold() const;

  void initHistograms();
  void fillHistograms(const PBASE* obj);
  
  unsigned int weight(const PBASE* obj) const; 

  int uniqueInt(const LHCb::STChannelID aChan) const;

  double SN(const PBASE* obj) const;

  std::map<int,int> m_mapping;
  std::vector<double> m_nStripsVector;
  std::vector<double> m_layerOccVector;
  std::vector<AIDA::IHistogram1D*> m_layerOccHistos;
  std::vector<AIDA::IHistogram1D*> m_stripOccVector;
  
 
 
  // job options
  std::vector<double> m_threshold;   ///< List of threshold values
 
  int m_binSize;                     ///< Number of channels in each bin
  std::string m_dataLocation;        ///< Location of the digits

  std::string m_histoLocation;
 
};

#endif // ITOccupancy_H
