// $Id: Calo2MCTool.h,v 1.1 2009-09-16 16:07:07 odescham Exp $
#ifndef CALO2MCTOOL_H 
#define CALO2MCTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICalo2MCTool.h"            // Interface
// from LHCb : the calo->particle stream
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// from LHCb : some  typedef and utilities
#include "CaloUtils/Calo2MC.h"
#include "CaloUtils/CaloMCTools.h"



/** @class Calo2MCTool Calo2MCTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-07-27
 */
class Calo2MCTool : public GaudiTool, virtual public ICalo2MCTool {
public: 
  /// Standard constructor
  Calo2MCTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual StatusCode initialize( ); 
  virtual ~Calo2MCTool( ); ///< Destructor

  //
  ICalo2MCTool* from(const LHCb::CaloDigit*     digit    ){  return this;}  
  ICalo2MCTool* from(const LHCb::CaloCluster*   cluster  );
  ICalo2MCTool* from(const LHCb::CaloHypo*      hypo     ){  return this;}  
  ICalo2MCTool* from(const LHCb::ProtoParticle* proto    ){  return this;}  
  ICalo2MCTool* from(const LHCb::Particle*      particle ){  return this;}  


protected:

private:
  StatusCode process();
  void clear();
  void mcDigest();
  void mcTree(const LHCb::MCParticle* part, std::vector<const LHCb::MCParticle*>& tree , std::string& sTree);
  double weight(const LHCb::MCParticle*);
  double quality(const LHCb::MCParticle*);
  const LHCb::MCParticle* findMC(std::string name, double threshold = 0 );
  std::string descriptor();
  const LHCb::MCParticle* bestMC();
  const LHCb::MCParticle* maxMC();
  CaloMCTools::CaloMCMap  mcMap();
  
  

  //
  std::vector<const LHCb::CaloDigit*>     m_digits;
  std::vector<const LHCb::CaloCluster*>   m_clusters;
  std::vector<const LHCb::CaloHypo*>      m_hypos;
  std::vector<const LHCb::ProtoParticle*> m_protos;
  std::vector<const LHCb::Particle*>      m_parts;
  CaloMCTools::CaloMCMap m_mcMap;
  //
  std::string m_cluster2MCLoc;
  std::string m_digit2MCLoc;
  LHCb::Calo2MC::IClusterTable* m_cluster2MC ;
  LHCb::Calo2MC::IDigitTable* m_digit2MC ;
  double m_sum;
  const LHCb::MCParticle* m_maxMC;
  const LHCb::MCParticle* m_bestMC;
  std::map<std::string,std::vector<const LHCb::MCParticle*> > m_treeMap;
  LHCb::IParticlePropertySvc* m_ppsvc;
};
#endif // CALO2MCTOOL_H
