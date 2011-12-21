#ifndef STEffChecker_H
#define STEffChecker_H 1

// GaudiAlg
#include "Kernel/STHistoAlgBase.h"

// linkers
#include "Linker/LinkerTool.h"

namespace LHCb {
  class MCHit;
  class MCParticle;
  class STCluster;
  class STChannelID;
}

class DeSTLayer;
class IMCParticleSelector;

namespace AIDA {
  class IHistogram1D;
  class IHistogram2D;
}

/** @class STEffChecker STEffChecker.h
 *
 *  Class for checking ST efficiencies. It produces the following plots:
 *  - x and y distributions of all MCHits in a IT or TT layer.
 *  - x vs y distribution of all MCHits in a IT or TT layer.
 *  - x and y distributions of MCHits which make an STCluster.
 *  - x vs y distribution of MCHits which make an STCluster.
 *  By dividing these histograms one gets the efficiency per layer.
 *
 *  In the finalize method, a summary of the efficiency per layer is printed. 
 * 
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class STEffChecker : public ST::HistoAlgBase {

public:
 
  /// constructer
  STEffChecker(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~STEffChecker();

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

  virtual void initHistograms();
  
  virtual void layerEff(const LHCb::MCParticle* aParticle);

  bool isInside(const DeSTLayer* aLayer, const LHCb::MCHit* aHit) const;

  int findHistoId(unsigned int aLayerId);
  int uniqueHistoID(const LHCb::STChannelID aChannel) const;
 
  void unBookHistos();
  void eraseHistos(); 

  Table* m_table;
  

  std::string m_asctLocation;
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
  std::map<unsigned int,int> m_mapping;

  std::vector<AIDA::IHistogram1D*> m_xLayerHistos;
  std::vector<AIDA::IHistogram1D*> m_yLayerHistos;
  std::vector<AIDA::IHistogram2D*> m_xyLayerHistos;
  std::vector<AIDA::IHistogram1D*> m_effXLayerHistos;
  std::vector<AIDA::IHistogram1D*> m_effYLayerHistos;
  std::vector<AIDA::IHistogram2D*> m_effXYLayerHistos;

};

#endif // STEffChecker_H



