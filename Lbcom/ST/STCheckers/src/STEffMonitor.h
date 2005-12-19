// $Id: STEffMonitor.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _STEffMonitor_H
#define _STEffMonitor_H

#include <vector>
#include <string>
#include <map>

#include "GaudiAlg/GaudiHistoAlg.h"

// from Associators
#include "Relations/IAssociator.h" 



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

  typedef IAssociator<LHCb::MCParticle, LHCb::MCHit> MCHitAsct;
 
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

  int findHistoId(unsigned int aLayerId);
  int uniqueHistoID(const LHCb::STChannelID aChannel) const;

  void unBookHistos();
  void eraseHistos(); 

  Table* m_table;
  

  std::string m_detType;
  DeSTDetector* m_tracker;
  std::string m_clusterLocation;
 

  // pointer to p to hit associator
  std::string m_p2STHitAsctName;
  MCHitAsct* m_p2STHitAsct;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

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













