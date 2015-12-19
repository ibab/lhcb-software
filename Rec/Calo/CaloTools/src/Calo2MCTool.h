// $Id: Calo2MCTool.h,v 1.4 2009-11-23 14:11:55 odescham Exp $
#ifndef CALO2MCTOOL_H 
#define CALO2MCTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICalo2MCTool.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
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
class Calo2MCTool : public GaudiTool
                  , virtual public ICalo2MCTool
                  , virtual public IIncidentListener{

public: 
  // category enum
  
  enum MCCategory  {
    /* Single cluster/hypo categories */
    UnMatched           = 0,
    Photon              = 1,
    BremStrahlung       = 2,
    Electron            = 3,
    ConvertedPhoton     = 4,
    MergedPi0           = 5, 
    ChargedHadron       = 6,
    NeutralHadron       = 7,
    Spillover           = 8
  };
  

  /// Standard constructor
  Calo2MCTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual StatusCode initialize( ); 
  virtual ~Calo2MCTool( ); ///< Destructor

  //
  virtual ICalo2MCTool* from(const LHCb::CaloDigit*     digit    );
  virtual ICalo2MCTool* from(const LHCb::CaloCluster*   cluster  );
  virtual ICalo2MCTool* from(const LHCb::CaloHypo*      hypo     );
  virtual ICalo2MCTool* from(const LHCb::ProtoParticle* proto    );
  virtual ICalo2MCTool* from(const LHCb::Particle*      particle );
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v){return  setProperty(p,v);};
  virtual const LHCb::MCParticle* bestMC();
  virtual const LHCb::MCParticle* maxMC();
  virtual double weight(const LHCb::MCParticle*);
  virtual const LHCb::MCParticle* findMC(LHCb::ParticleID id, double threshold = 0 );
  virtual const LHCb::MCParticle* findMCOrBest(LHCb::ParticleID id, double threshold = 0 );
  virtual const LHCb::MCParticle* findMC(std::string name, double threshold = 0 );
  virtual const LHCb::MCParticle* findMCOrBest(std::string name, double threshold = 0 );
  virtual double quality(const LHCb::MCParticle*);
  virtual std::string descriptor();
  virtual bool isCalo(const LHCb::Particle* particle){
    if( NULL == particle )return false;
    LHCb::CaloParticle cPart = LHCb::CaloParticle( (LHCb::Particle*) particle );
    return  cPart.isCalo();
  }
  virtual bool isPureNeutralCalo(const LHCb::Particle* particle){  // SHOULD BE IN CALOPARTICLE
    if( NULL == particle )return false;
    LHCb::CaloParticle cPart = LHCb::CaloParticle( (LHCb::Particle*) particle );
    return cPart.isPureNeutralCalo();// non pure calorimetric object
  }

    

  /// Inform that a new incident has occurred
  virtual void handle(const Incident& /* inc */ ) { clear();} 


  // TO BE INTERFACED :
  int MCCategory(){return m_category;};
  ICalo2MCTool* fragment(unsigned int i);
  unsigned int numberOfFragments(){return m_nFrag;}    
  //ostream << category
  // clusters()
  // hypos()
  // protos()
protected:

private:
  StatusCode process();
  void addDigit   (const LHCb::CaloDigit* digit);
  void addCluster (const LHCb::CaloCluster* cluster);
  void addHypo    (const LHCb::CaloHypo* hypo);
  void addProto   (const LHCb::ProtoParticle* proto, const LHCb::Particle* parent = NULL);
  void clear();
  void mcDigest();
  void mcTree(const LHCb::MCParticle* part, std::vector<const LHCb::MCParticle*>& tree , std::string& sTree);
  CaloMCTools::CaloMCMap  mcMap();
  
  

  //
  std::vector<const LHCb::CaloDigit*>     m_digits;
  std::vector<const LHCb::CaloCluster*>   m_clusters;
  std::vector<const LHCb::CaloHypo*>      m_hypos;
  std::vector<const LHCb::ProtoParticle*> m_protos;
  std::vector<const LHCb::Particle*>      m_parts;
  LHCb::CaloDigit* m_digit;
  LHCb::CaloCluster* m_cluster;
  LHCb::CaloHypo* m_hypo;
  LHCb::ProtoParticle* m_proto;
  LHCb::Particle* m_part;
  CaloMCTools::CaloMCMap m_mcMap;
  //
  std::string m_cluster2MCLoc;
  std::string m_digit2MCLoc;
  LHCb::Calo2MC::IClusterTable* m_cluster2MC ;
  //LHCb::Calo2MC::IHypoTable*    m_hypo2MC ;
  LHCb::Calo2MC::IDigitTable*   m_digit2MC ;
  double m_sum;
  const LHCb::MCParticle* m_maxMC;
  const LHCb::MCParticle* m_bestMC;
  std::map<std::string,std::vector<const LHCb::MCParticle*> > m_treeMap;
  LHCb::IParticlePropertySvc* m_ppsvc;
  
  // 
  bool m_hypo2Cluster;
  bool m_cluster2Digit;
  bool m_merged2Split;
  int m_sFilter;
  int m_category;
  int m_depth;
  unsigned int m_nFrag;
  ICalo2MCTool* m_tool;
};
#endif // CALO2MCTOOL_H
