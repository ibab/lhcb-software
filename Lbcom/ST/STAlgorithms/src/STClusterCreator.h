// $Id: STClusterCreator.h,v 1.3 2007-01-09 15:34:34 jvantilb Exp $
#ifndef STCLUSTERCREATOR_H
#define STCLUSTERCREATOR_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/STDigit.h"
#include "Event/STCluster.h"

class DeSTSDetector;
class DeSTSector;
class ISTSignalToNoiseTool;
class ISTClusterPosition;

/** @class STClusterCreator STClusterCreator.h
 *
 *  Class for clustering in the ST tracker
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

class STClusterCreator :public GaudiAlgorithm {

public:
  
  // Constructors and destructor
  STClusterCreator( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STClusterCreator();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  
  StatusCode createClusters( const LHCb::STDigits* digitsCont, 
                             LHCb::STClusters* clustersCont) const ;

  bool keepClustering( const LHCb::STDigit* firstDigit, 
                       const LHCb::STDigit* secondDigit, 
                       const DeSTSector* aSector ) const;
  
  bool sameBeetle( const LHCb::STChannelID firstChan, 
                   const LHCb::STChannelID secondChan ) const;

  bool aboveDigitSignalToNoise( const LHCb::STDigit* aDigit,
                                const DeSTSector* aSector ) const;
  bool aboveClusterSignalToNoise( const double charge,
                                  const DeSTSector* sector ) const;
  bool hasHighThreshold(const double charge, const DeSTSector* aSector) const; 

  double neighbourSum( LHCb::STDigits::const_iterator start, 
                       LHCb::STDigits::const_iterator stop, 
                       const LHCb::STDigits* digits ) const;

  LHCb::STCluster::ADCVector strips(const SmartRefVector<LHCb::STDigit>& 
                                    clusteredDigits,
                                    const LHCb::STChannelID closestChan) const;
 
  double m_digitSig2NoiseThreshold;
  double m_clusterSig2NoiseThreshold;
  double m_highThreshold;

  typedef std::map<const DeSTSector*,double> CutMap;
 
  mutable CutMap m_digitSig2NoiseCut;
  mutable CutMap m_clusterSig2NoiseCut;
  mutable CutMap m_highSig2NoiseCut;

  int m_outputVersion;
  int m_maxSize;
  bool m_byBeetle;

  std::string m_sigNoiseToolName;
  std::string m_positionToolName;

  std::string m_inputLocation;
  std::string m_outputLocation;
  
  std::string m_detType;
  DeSTDetector* m_tracker;
  ISTSignalToNoiseTool* m_sigNoiseTool;
  ISTClusterPosition* m_positionTool;
 
};

#endif // STCLUSTERSCREATOR_H
