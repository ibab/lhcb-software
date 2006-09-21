// $Id: STEffMonitor.h,v 1.5 2006-09-21 15:47:48 ocallot Exp $
#ifndef _STEffMonitor_H
#define _STEffMonitor_H

#include <vector>
#include <string>
#include <map>

#include "GaudiAlg/GaudiHistoAlg.h"


// linkers
#include "Linker/LinkerTool.h"

namespace LHCb {
  class MCHit;
  class MCParticle;
  class STCluster;
  class STChannelID;
};

class DeSTDetector;
class DeSTLayer;
class IMCParticleSelector;

class AIDA::IHistogram1D;
class AIDA::IHistogram2D;

/** @class STEffMonitor STEffMonitor.h
 *
 *  Class for checking ITEffs
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class STEffMonitor : public GaudiHistoAlg {

public:
 
  /// constructer
  STEffMonitor(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STEffMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::InverseType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  virtual StatusCode initHistograms();
  
  virtual StatusCode layerEff(const LHCb::MCParticle* aParticle);

  bool isInside(const DeSTLayer* aLayer, const LHCb::MCHit* aHit) const;

  int findHistoId(unsigned int aLayerId);
  int uniqueHistoID(const LHCb::STChannelID aChannel) const;

  void unBookHistos();
  void eraseHistos(); 

  Table* m_table;
  

  std::string m_detType;
  std::string m_asctLocation;
  DeSTDetector* m_tracker;
  std::string m_clusterLocation;
  bool m_includeGuardRings; 
  bool m_pEff;

  // pointer to p to hit associaton
  typedef LinkerTool< LHCb::MCHit, LHCb::MCParticle > HitTable;
  const HitTable::InverseType* m_hitTable;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  std::string m_hitTableLocation;
 
  // mapping
  std::map<unsigned int,int> m_Mapping;

  std::vector<AIDA::IHistogram1D*> m_xLayerHistos;
  std::vector<AIDA::IHistogram1D*> m_yLayerHistos;
  std::vector<AIDA::IHistogram2D*> m_xyLayerHistos;
  std::vector<AIDA::IHistogram1D*> m_effXLayerHistos;
  std::vector<AIDA::IHistogram1D*> m_effYLayerHistos;
  std::vector<AIDA::IHistogram2D*> m_effXYLayerHistos;

};

#endif // _STEffMonitor_H













