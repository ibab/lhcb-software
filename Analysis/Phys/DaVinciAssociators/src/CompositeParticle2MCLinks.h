// $Id: CompositeParticle2MCLinks.h,v 1.5 2003-05-26 11:38:38 phicharp Exp $
#ifndef CompositeParticle2MCLinks_H 
#define CompositeParticle2MCLinks_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


// local
#include "DaVinciAssociators/Particle2MCLinksAsct.h"
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "AsctAlgorithm.h"

/** @class CompositeParticle2MCLinks CompositeParticle2MCLinks.h
 *  
 *  @author Gerhard Raven
 *  @date   September 2002
 */
class CompositeParticle2MCLinks : public AsctAlgorithm {
  friend AlgFactory<CompositeParticle2MCLinks>;
  
public:
  /// Standard constructor
  CompositeParticle2MCLinks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompositeParticle2MCLinks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


private:
  Particle2MCLinksAsct::IAsct* m_pAsct;
  bool m_allowExtraMCPhotons;
  int  m_gamma;

  bool associate1(const Particle *p, const MCParticle *m, 
                  Particle2MCAsct::Table* table) const;
  // This can be removed when the Relations package is updated...
  MCParticle* myAssociatedFrom(const Particle* p) const;
  bool isAssociatedFrom( const Particle* p, const MCParticle* m) const;

  class associator : public std::unary_function<MCParticle*,bool> {
  public:
    associator(CompositeParticle2MCLinks* t,Particle* p,Particle2MCAsct::Table* table) 
      : m_t(t),m_p(p),m_table(table) {};
    bool operator()(const MCParticle* m) const {
      return m_t->associate1(m_p,m,m_table);
    }
           
  private: 
    CompositeParticle2MCLinks *m_t;
    Particle *m_p;
    Particle2MCAsct::Table *m_table;
  };
  friend class associator;
  
};
#endif // Particle2MCLinks_H
