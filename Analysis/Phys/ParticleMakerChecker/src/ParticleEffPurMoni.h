//-----------------------------------------------------------------------------
/** @file ParticleEffPurMoni.h
 *
 *  Header file for class : ParticleEffPurMoni
 *
 *  CVS Log :-
 *  $Id: ParticleEffPurMoni.h,v 1.13 2008-07-09 22:43:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2007-002-21
 */
//-----------------------------------------------------------------------------

#ifndef PARTICLEMAKERCHECKER_ParticleEffPurMoni_H
#define PARTICLEMAKERCHECKER_ParticleEffPurMoni_H 1

// STD
#include <sstream>
#include <memory>
#include <map>
#include <set>

// Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiUtils/Aida2ROOT.h"

// base class
#include "Kernel/DVAlgorithm.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "MCInterfaces/IMCParticleSelector.h"

// Kernel
#include "RichKernel/RichPoissonEffFunctor.h"

// MC association
#include "Kernel/Particle2MCLinker.h"

// Event
#include "Event/MCParticle.h"

// ROOT
#include "TProfile.h"

//-----------------------------------------------------------------------------
/** @class ParticleEffPurMoni ParticleEffPurMoni.h
 *
 *  Algorithm to provide basic (MC based) PID efficiency and
 *  purity information for the input Particles.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2007-02-21
 */
//-----------------------------------------------------------------------------

class ParticleEffPurMoni : public DVAlgorithm
{

public:

  /// Standard constructor
  ParticleEffPurMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ParticleEffPurMoni( );  ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute   (); ///< Algorithm execution
  virtual StatusCode finalize  (); ///< Algorithm finalization

private: // definitions

  /** @class EffVersusMomentum ParticleEffPurMoni.h
   *  Utility class used to create efficneicy versus momentum plots
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   09/07/2008
   */
  class EffVersusMomentum
  {
  public:
    /// Defintion of internal data storage
    typedef std::vector<unsigned int> Data;
    /// Constructor
    EffVersusMomentum( const double _minP,
                       const double _maxP,
                       const int    _nBins )
      : m_minP(_minP),
        m_maxP(_maxP),
        m_nBins(_nBins),
        m_data(_nBins,0) { }
  private:
    inline unsigned int bin( const double p )
    {
      return static_cast<unsigned int>( m_nBins * (p-m_minP) / (m_maxP-m_minP) );
    }
  public:
    inline const Data & data() const { return m_data; }
    inline double minP() const { return m_minP; }
    inline double maxP() const { return m_maxP; }
    inline unsigned int nBins() const { return m_nBins; }
    inline double p(const unsigned int bin) const
    {
      return m_minP + (0.5+bin)*(m_maxP-m_minP)/m_nBins;
    }
  public:
    /// Fill the histogram for the given momentum value
    inline void fill( const double p )
    {
      if ( p<=m_maxP && p>=m_minP )
      {
        const unsigned int b = bin(p);
        if ( b<m_nBins) ++(m_data[b]);
      }
    }
  private:
    double m_minP;        ///< Minimum momentum
    double m_maxP;        ///< Maximum momentum
    unsigned int m_nBins; ///< Number of bins
    Data m_data;          ///< The data
  };

  /** @class ParticleHistory ParticleEffPurMoni.h
   *  Utility class containing basic Particle history information for the ParticleEffPurMoni algorithm
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
  class ParticleHistory
  {
  public:
    ParticleHistory( const LHCb::Particle * _part,
                     const LHCb::Particle * _prim_part,
                     const std::string & _history,
                     const ParticleProperty * _prop,
                     const bool _orig )
      : particle       (_part      ),
        firstParticle  (_prim_part ),
        history        (_history   ),
        properties     (_prop      ),
        topLevel       (_orig      ) { }
  public:
    const LHCb::Particle * particle;      ///< Pointer to the Particle
    const LHCb::Particle * firstParticle; ///< Pointer to the first Particle
    std::string history;                  ///< Particle history
    const ParticleProperty * properties;  ///< Properties
    bool topLevel;                        ///< Was the Particle in one of the original input locations
  public:
    typedef std::vector< ParticleHistory > Vector;
  public:
    /// Overloaded printout to ostream
    friend inline std::ostream& operator << ( std::ostream& os,
                                              const ParticleHistory & hist )
    {
      return os << "Particle=" << hist.particle << " history=" << hist.history
                << " topLevel=" << hist.topLevel;
    }
  };

  /// Type for mapping between ProtoParticle and list of Particles produced from it
  typedef std::map<const LHCb::ProtoParticle *, ParticleHistory::Vector > ParticleProtoMap;

  /// Simple tally map for each particle type
  class MCTally
  {
  public:
    MCTally() : all(0), clones(0), m_effVp(NULL), m_effVpt(NULL) { }
    ~MCTally() { delete m_effVp; delete m_effVpt; }
  public:
    unsigned long int all;         ///< Total number
    unsigned long int clones;      ///< Number of clones
    typedef std::map<std::string,unsigned long int> Contributions;
    Contributions all_detailed;    ///< Detailed breakdown for each full MC tree decay (all)
    Contributions clones_detailed; ///< Detailed breakdown for each full MC tree decay (clones)
  private:
    mutable EffVersusMomentum * m_effVp;       ///< Eff versus momentum (p) data
    mutable EffVersusMomentum * m_effVpt;      ///< Eff versus momentum (pt) data
  public:
    inline EffVersusMomentum & effVp() const
    {
      if (!m_effVp)
      {
        m_effVp  = new EffVersusMomentum(0*Gaudi::Units::GeV,100*Gaudi::Units::GeV,50);
      }
      return *m_effVp;
    }
    inline EffVersusMomentum & effVpt() const
    { 
      if (!m_effVpt) 
      { 
        m_effVpt = new EffVersusMomentum(0*Gaudi::Units::GeV,10*Gaudi::Units::GeV,50);
      } 
      return *m_effVpt; 
    }
  };
  typedef std::map<std::string, MCTally> TypeTally;
  typedef std::map< IMCReconstructible::RecCategory, TypeTally > MCRecTypeMap;

  /** @class MCSummary ParticleEffPurMoni.h
   *  Utility class containing summary MC tallies for the ParticleEffPurMoni algorithm
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
  class MCSummary
  {
  public:
    /// Default Constructor
    MCSummary() : nReco(0) { }
  public:
    MCRecTypeMap trueMCType;  ///< Total number of each MC type
    unsigned long int nReco;  ///< Total number reconstructed
    std::string decayHistory; ///< The particle decay history
  };

  /** @class FullPartName ParticleEffPurMoni.h
   *  Utility class containing unique name information for the Particle
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
  class FullPartName
  {
  public:
    /// Default Constructor
    FullPartName( const std::string & name = "",
                  const std::string & tree = "",
                  const std::string & type = "",
                  const std::string & protoTes = "" )
      :  particleName(name),
         decayTree(tree),
         protoType(type),
         protoTESLoc(protoTes) { }
    /// Operator <
    inline bool operator< ( const FullPartName& name ) const
    { return ( this->protoTESLoc + this->protoType + this->particleName + this->decayTree <
               name.protoTESLoc  + name.protoType  + name.particleName  + name.decayTree ); }
    /// Equality operator
    inline bool operator== ( const FullPartName& name ) const
    { return ( this->particleName == name.particleName &&
               this->decayTree    == name.decayTree    &&
               this->protoType    == name.protoType    &&
               this->protoTESLoc  == name.protoTESLoc   ); }
    /// Non-Equality operator
    inline bool operator!= ( const FullPartName& name ) const
    { return ! this->operator==(name); }
    /// Overloaded operator to ostream
    friend inline std::ostream& operator << ( std::ostream& os, const FullPartName & name )
    { return os << "[ " << name.particleName << " "
                << name.decayTree << " " << name.protoType << " ]"; }
  public:
    std::string particleName; ///< Particle name (K+, pi- etc.)
    std::string decayTree;    ///< The decay tree for the particle
    std::string protoType;    ///< The ProtoParticle type
    std::string protoTESLoc;  ///< ProtoParticle container location in TES
  };

  typedef std::map< FullPartName, MCSummary >     MCSummaryMap;
  typedef std::map< std::string, std::map< std::string, MCSummary > > MCSummaryMapAllProtos;
  typedef std::map< std::string, MCSummaryMap >   LocationMap;

  /// ProtoParticle TES statistics summary class
  class ProtoTESStats
  {
  public:
    ProtoTESStats() : nTotal(0), nWithMC(0) { }
  public:
    unsigned long nTotal;  ///< Tally of the total number of selected ProtoParticles
    unsigned long nWithMC; ///< Number of selected ProtoParticles with MC information
  };

  /// map between TES location and Stats object
  typedef std::map< std::string, ProtoTESStats > ProtoTESStatsMap;

  /// Map between Particles and MCParticles
  typedef std::map<const LHCb::ProtoParticle*,const LHCb::MCParticle*> P2MCP;

private: // methods

  /** Returns the full location of the given object in the Data Store
   *
   *  @param pObj Data object
   *
   *  @return Location of given data object
   */
  inline std::string objectLocation( const DataObject * pObj ) const
  {
    return ( !pObj ? "Null DataObject !" :
             (pObj->registry() ? pObj->registry()->identifier() : "UnRegistered") );
  }

  /// Create a unique list of Particles to study
  void addParticle( const LHCb::Particle * particle,
                    const LHCb::Particle * firstParticle,
                    const std::string & history = "",
                    const bool toplevel         = true ) const;

  /// Printout statistics
  void printStats() const;

  /// Returns the momentum value for a given ProtoParticle
  const double momentum( const LHCb::ProtoParticle * proto ) const
  {
    // CRJ : Need to decide what to do for neutrals
    return ( NULL != proto->track() ? proto->track()->p() : 0 );
  }

  /// Returns the transverse momentum value for a given ProtoParticle
  const double transverseMomentum( const LHCb::ProtoParticle * proto ) const
  {
    // CRJ : Need to decide what to do for neutrals
    return ( NULL != proto->track() ? proto->track()->pt() : 0 );
  }

  /// Returns the ParticleProperty object for a given ParticleID
  const ParticleProperty * partProp( const LHCb::ParticleID id ) const;

  /// Access the Particle Linker appropriate for the given Particle
  Particle2MCLinker * particleLinker( const LHCb::Particle * /* part */ ) const;

  /// Access the charged ProtoParticle Linker
  ProtoParticle2MCLinker * chargedProtoLinker(const LHCb::ProtoParticle * proto) const;

  /// Access the neutral ProtoParticle Linker
  ProtoParticle2MCLinker * neutralProtoLinker(const LHCb::ProtoParticle * proto) const;

  /// Access the ProtoParticle Linker appropriate for the given ProtoParticle
  inline ProtoParticle2MCLinker * protoLinker( const LHCb::ProtoParticle * proto ) const
  {
    return ( 0 != proto->charge() ? chargedProtoLinker(proto) : neutralProtoLinker(proto) );
  }

  /// Returns the MCParticle for a given Particle
  const LHCb::MCParticle * mcParticle( const LHCb::Particle * part ) const;

  /// Returns the MCParticle for a given ProtoParticle
  inline const LHCb::MCParticle * mcParticle( const LHCb::ProtoParticle * proto ) const
  {
    return ( ( 0 != proto->charge() && m_useRMCPcharged ) ||
             ( 0 == proto->charge() && m_useRMCPneutral ) ?
             mcParticle_reverse(proto) : mcParticle_forward(proto) );
  }

  /// Returns the MCParticle for a given ProtoParticle (using best forward P -> MCP association)
  const LHCb::MCParticle * mcParticle_forward( const LHCb::ProtoParticle * proto ) const;

  /// Returns the MCParticle for a given ProtoParticle (using best reverse MCP -> P association)
  const LHCb::MCParticle * mcParticle_reverse( const LHCb::ProtoParticle * proto ) const;

  /// ProtoParticle selection
  bool selectProto( const LHCb::ProtoParticle * proto ) const;

  /// Returns the min MC association weight for the given ProtoParticle
  inline double minMCWeight( const LHCb::Particle * part ) const
  {
    return ( part->isBasicParticle() ? m_minAssWeightComp : minMCWeight(part->proto()) );
  }

  /// Returns the min MC association weight for the given ProtoParticle
  inline double minMCWeight( const LHCb::ProtoParticle * proto ) const
  {
    return ( proto == NULL ? 1.0 :
             (0 != proto->charge() ? m_minAssWeightCh : m_minAssWeightNu) );
  }

  /// Returns the ProtoParticle 'type'
  std::string protoParticleType( const LHCb::ProtoParticle * proto ) const;

  /// Returns the MCParticle name
  std::string mcParticleName( const LHCb::MCParticle * mcPart );

  /// Returns the MCParticle name including the decay tree
  std::string mcParticleNameTree( const LHCb::MCParticle * mcPart );

  /// Make a histogram
  void makeEffHisto( const std::string title,
                     const EffVersusMomentum & top,
                     const EffVersusMomentum & bot ) const;

private: // data

  /// Particle/Proto map
  mutable ParticleProtoMap m_partProtoMap;

  /// Recursion count
  mutable unsigned int m_recurCount;

  /// Track selector tool
  ITrackSelector * m_trSel;

  /// MC Reconstructible tool
  IMCReconstructible * m_mcRec;

  /// MCParticle selector
  IMCParticleSelector * m_mcSel;

  /// MC Associations for Particles
  mutable Particle2MCLinker * m_particleLinker;

  /// TES location -> linker map type
  typedef std::map<std::string,ProtoParticle2MCLinker *> ProtoLinkerTESMap;

  /// MC Associations for charged ProtoParticles
  mutable ProtoLinkerTESMap m_chargedProtoLinker;

  /// MC Associations for neutral ProtoParticles
  mutable ProtoLinkerTESMap m_neutralProtoLinker;

  /// The overall tally
  mutable LocationMap m_locMap;

  /// Total true number of particles of each type at each TES location
  mutable MCSummaryMapAllProtos m_mcProtoCount;

  /// ProtoParticle stats for each TES location
  mutable ProtoTESStatsMap m_protoTesStats;

  /// Raw MC informtion
  mutable MCRecTypeMap m_rawMCMap;

  /// Min percentage contribution to include in summary tables
  double m_minContrib;

  /// Number of events called
  unsigned long m_nEvts;

  /// Minimum MC association weight for charged ProtoParticles
  double m_minAssWeightCh;

  /// Minimum MC association weight for neutral ProtoParticles
  double m_minAssWeightNu;

  /// Minimum MC association weight for composite Particles
  double m_minAssWeightComp;

  /// Map for storing results of reverse charged Particle to MCParticle mapping
  mutable P2MCP m_cp2mcp;

  /// Map for storing results of reverse neutral Particle to MCParticle mapping
  mutable P2MCP m_np2mcp;

  /// Use reverse MC associations for charged particles
  bool m_useRMCPcharged;

  /// Use reverse MC associations for neutral particles
  bool m_useRMCPneutral;

  /// Use the FULL MC Decay tree for particle classification
  bool m_fullMCTree;

  /// Length of longest name string
  unsigned int m_maxNameLength;

  /// Max tree size
  unsigned int m_maxMCTreeSize;

  /// Minimum momentum for plots
  double m_minP;

};

#endif // PARTICLEMAKERCHECKER_ParticleEffPurMoni_H
