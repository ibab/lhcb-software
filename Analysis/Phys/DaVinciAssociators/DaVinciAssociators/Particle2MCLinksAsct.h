// $Id: Particle2MCLinksAsct.h,v 1.6 2004-07-27 17:42:16 phicharp Exp $
#ifndef Particle2MCLinksASCT_H 
#define Particle2MCLinksASCT_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// Forward declaration
class Particle;
class MCParticle;

// Associators
#include "Relations/AssociatorWeighted.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCLink.h"

static const std::string& 
Particle2MCLinksAsctLocation = "Phys/Relations/Particle2MCLinks";


/** @class Particle2MCLinksAsct Particle2MCLinksAsct.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   17/05/2002
 */

class Particle2MCLinksAsct : 
  public AssociatorWeighted<Particle,MCParticle,double>
{
  friend class ToolFactory<Particle2MCLinksAsct>;
public:
  // Define data types
  typedef RelationWeighted1D<Particle,MCParticle,double>       Table;
  typedef OwnType                               Asct;

  /// Standard constructor
  Particle2MCLinksAsct(const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : Asct( type, name, parent)
    , m_chargedLink(NULL)
    , m_neutralLink(NULL)
    , m_table(0)
  {

    setProperty( "Location", Particle2MCLinksAsctLocation );
    setProperty( "AlgorithmType", "Particle2MCLinks" );
    setProperty( "AlgorithmName", "Particle2MCLinks" );
  }; 

  virtual ~Particle2MCLinksAsct( ){}; ///< Destructor

  // This Associator has its own interface implementation in order to allow association
  // from Particles not in the TES
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual void handle(const Incident& incident);
  virtual bool tableExists() const;
  virtual ToRange rangeFrom( const From& part) const;
  virtual ToRange rangeWithLowCutFrom( const From& part, double cut) const;
  virtual ToRange rangeWithHighCutFrom( const From& part, double cut) const;
  virtual To associatedFrom( const From& part) const;
  virtual To associatedFrom( const From& part, double& weight) const;

  virtual FromRange rangeTo( const To& mcpart) const{
    if( m_hasTable ) return Asct::rangeTo( mcpart);
    return FromRange();
  };
  virtual FromRange rangeWithLowCutTo( const To& mcpart, double cut) const{
    if( m_hasTable ) return Asct::rangeWithLowCutTo( mcpart, cut);
    return FromRange();    
  };
  virtual FromRange rangeWithHighCutTo( const To& mcpart, double cut) const{
    if( m_hasTable ) return Asct::rangeWithHighCutTo( mcpart, cut);
    return FromRange();    
  };
  virtual From associatedTo( const To& mcpart) const{
    if( m_hasTable ) return Asct::associatedTo( mcpart);
    return From();
  };
  virtual From associatedTo( const To& mcpart, double& weight) const{
    if( m_hasTable ) return Asct::associatedTo( mcpart, weight);
    weight = 0;
    return From();    
  };

protected:

private:
  bool m_hasTable;  //<< Flag set if the associator doesn't use a table
  Object2MCLink*   m_chargedLink;
  Object2MCLink*   m_neutralLink;
  Table* m_table;     //<< Table to hold temporary relations

  // Private methods
  void insertRange( const From& part ) const;
};

typedef Particle2MCLinksAsct::FromRange      ParticlesToMCLinks;
typedef Particle2MCLinksAsct::FromIterator   ParticlesToMCLinksIterator;
typedef Particle2MCLinksAsct::ToRange        MCsFromParticleLinks;
typedef Particle2MCLinksAsct::ToIterator     MCsFromParticleLinksIterator;
typedef Particle2MCLinksAsct::Table          Particle2MCLinksTable;
  
#endif // Particle2MCLinksASCT_H
