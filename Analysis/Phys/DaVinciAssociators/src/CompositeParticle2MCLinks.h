// $Id: CompositeParticle2MCLinks.h,v 1.8 2004-06-11 15:26:16 phicharp Exp $
#ifndef CompositeParticle2MCLinks_H 
#define CompositeParticle2MCLinks_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCLink.h"
#include "AsctAlgorithm.h"

/** @class CompositeParticle2MCLinks CompositeParticle2MCLinks.h
 *  
 *  @author Gerhard Raven
 *  @date   September 2002
 */
class CompositeParticle2MCLinks : public AsctAlgorithm {
  friend class AlgFactory<CompositeParticle2MCLinks>;
  
public:
  /// Standard constructor
  CompositeParticle2MCLinks(const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompositeParticle2MCLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  

private:
  Object2MCLink*      m_p2MCLink;
  IParticlePropertySvc* m_ppSvc;
  bool m_allowExtraMCPhotons;
  bool m_inclusiveMode;
  bool m_skipResonances;
  double m_maxResonanceLifeTime;
  int  m_gamma;
  Particle2MCAsct::Table*   m_table;
  Particle2MCLink::Linker*  m_linkerTable;
  int m_nrel;

  bool associate1(const Particle *p, const MCParticle* m) ;
  // This can be removed when the Relations package is updated...
  MCParticle* myAssociatedFrom(const Particle* p) const;
  bool isAssociatedFrom( const Particle* p, const MCParticle* m) const;
  bool addDaughters( const Particle* m,
                     std::vector<const Particle*>& daughters) const;
  bool addMCDaughters( const MCParticle* m,
                       std::vector<const MCParticle*>& daughters) const;
  

  class associator : public std::unary_function<MCParticle*,bool> {
  public:
    associator(CompositeParticle2MCLinks* t,Particle* p,
               Particle2MCAsct::Table* table) 
      : m_t(t), m_p(p), m_table(table) {};
    bool operator()(const MCParticle* m) const {
      return m_t->associate1( m_p, m);
    }
           
  private: 
    CompositeParticle2MCLinks *m_t;
    Particle *m_p;
    Particle2MCAsct::Table *m_table;
  };
  friend class associator;
  
};
#endif // Particle2MCLinks_H
