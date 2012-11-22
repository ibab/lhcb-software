//-----------------------------------------------------------------------------
/** @file ParticleEffPurMoni.h
 *
 *  Header file for class : ParticleEffPurMoni
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
#include "Kernel/DaVinciHistoAlgorithm.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "MCInterfaces/IMCParticleSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Kernel
#include "RichKernel/RichPoissonEffFunctor.h"

// MC association
#include "Kernel/Particle2MCLinker.h"

// Event
#include "Event/MCParticle.h"

// ROOT
#include "TProfile.h"
#include "TProfile2D.h"

// Boost
#include "boost/lexical_cast.hpp"

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

class ParticleEffPurMoni : public DaVinciHistoAlgorithm
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
   *  Utility class used to create efficiency versus momentum (p or pt) plots
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   09/07/2008
   */
  class EffVersusMomentum
  {
  public:
    /// Defintion of internal data storage
    typedef std::vector<unsigned int> Data;
    /// Constructor
    EffVersusMomentum( const double _minX,
                       const double _maxX,
                       const int    _nBins )
      : m_minX(_minX),
        m_maxX(_maxX),
        m_nBins(_nBins),
        m_data(_nBins,0) { }
  private:
    inline unsigned int bin( const double p )
    {
      return static_cast<unsigned int>( m_nBins * (p-m_minX) / (m_maxX-m_minX) );
    }
  public:
    inline const Data & data() const { return m_data; }
    inline double minX() const { return m_minX; }
    inline double maxX() const { return m_maxX; }
    inline unsigned int nBins() const { return m_nBins; }
    inline double p(const unsigned int bin) const
    {
      return m_minX + (0.5+bin)*(m_maxX-m_minX)/m_nBins;
    }
  public:
    /// Fill the histogram for the given momentum value
    inline void fill( const double p )
    {
      if ( p<=m_maxX && p>=m_minX )
      {
        const unsigned int b = bin(p);
        if ( b<m_nBins) ++(m_data[b]);
      }
    }
  private:
    double m_minX;        ///< Minimum momentum
    double m_maxX;        ///< Maximum momentum
    unsigned int m_nBins; ///< Number of bins
    Data m_data;          ///< The data
  };

  /** @class EffVersusMomentum2D ParticleEffPurMoni.h
   *  Utility class used to create efficiency versus momentum (p,pt) plots
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   09/07/2008
   */
  class EffVersusMomentum2D
  {
  public:
    /// Defintion of internal data storage
    typedef std::vector< std::vector<unsigned int> > Data;
    /// Constructor
    EffVersusMomentum2D( const double _minX,
                         const double _maxX,
                         const double _minY,
                         const double _maxY,
                         const int    _nBinsX,
                         const int    _nBinsY )
      : m_minX(_minX),
        m_maxX(_maxX),
        m_minY(_minY),
        m_maxY(_maxY),
        m_nBinsX(_nBinsX),
        m_nBinsY(_nBinsY),
        m_data( _nBinsX , std::vector<unsigned int>(_nBinsY,0) ) { }
  private:
    inline unsigned int binX( const double x )
    {
      return static_cast<unsigned int>( m_nBinsX * (x-m_minX) / (m_maxX-m_minX) );
    }
    inline unsigned int binY( const double y )
    {
      return static_cast<unsigned int>( m_nBinsY * (y-m_minY) / (m_maxY-m_minY) );
    }
  public:
    inline const Data & data() const { return m_data; }
    inline double minX() const { return m_minX; }
    inline double maxX() const { return m_maxX; }
    inline double minY() const { return m_minY; }
    inline double maxY() const { return m_maxY; }
    inline unsigned int nBinsX() const { return m_nBinsX; }
    inline unsigned int nBinsY() const { return m_nBinsY; }
    inline double x(const unsigned int binx) const
    {
      return m_minX + (0.5+binx)*(m_maxX-m_minX)/m_nBinsX;
    }
    inline double y(const unsigned int biny) const
    {
      return m_minY + (0.5+biny)*(m_maxY-m_minY)/m_nBinsY;
    }
  public:
    /// Fill the histogram for the given momentum value
    inline void fill( const double x, const double y )
    {
      if ( x<=m_maxX && x>=m_minX && y<=m_maxY && y>=m_minY )
      {
        const unsigned int xb = binX(x);
        const unsigned int yb = binY(y);
        if ( xb<m_nBinsX && yb<m_nBinsY ) ++((m_data[xb])[yb]);
      }
    }
  private:
    double m_minX;         ///< Minimum x momentum
    double m_maxX;         ///< Maximum x momentum
    double m_minY;         ///< Minimum y momentum
    double m_maxY;         ///< Maximum y momentum
    unsigned int m_nBinsX; ///< Number of x bins
    unsigned int m_nBinsY; ///< Number of y bins
    Data m_data;           ///< The data
  };

  /** @class ParticleHistory ParticleEffPurMoni.h
   *  Utility class containing basic Particle history information for the
   *  ParticleEffPurMoni algorithm
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
  class ParticleHistory
  {
  public:
    ParticleHistory( const LHCb::Particle*        _part,
                     const LHCb::Particle*        _prim_part,
                     const std::string&           _history,
                     const LHCb::ParticleProperty* _prop,
                     const bool _orig )
      : particle       ( _part      ),
        firstParticle  ( _prim_part ),
        history        ( _history   ),
        properties     ( _prop      ),
        topLevel       ( _orig      ) { }
  public:
    const LHCb::Particle * particle;      ///< Pointer to the Particle
    const LHCb::Particle * firstParticle; ///< Pointer to the first Particle
    std::string history;                  ///< Particle history
    const LHCb::ParticleProperty * properties;  ///< Properties
    bool topLevel;                        ///< Was the Particle in one of the original input locations
  public:
    typedef std::map<ParticleHistory,unsigned int> Map;
  public:
    /// Overloaded printout to ostream
    friend inline std::ostream& operator << ( std::ostream& os,
                                              const ParticleHistory & hist )
    {
      return os << "Particle=" << hist.particle << " history=" << hist.history
                << " topLevel=" << hist.topLevel;
    }
  public:
    /// Operator ==
    inline bool operator== ( const ParticleHistory& hist ) const
    {
      return ( this->particle == hist.particle &&
               this->history  == hist.history  );
    }
    /// Operator !=
    inline bool operator!= ( const ParticleHistory& hist ) const
    {
      return ! this->operator==(hist);
    }
    /// Operator <
    inline bool operator<  ( const ParticleHistory& hist ) const
    {
      // CRJ : Could do this more efficiently ...
      return ( boost::lexical_cast<std::string>(this->particle)+this->history <
               boost::lexical_cast<std::string>(hist.particle)+hist.history   );
    }
  };

  /// Type for mapping between ProtoParticle and list of Particles produced from it
  typedef std::map<const LHCb::ProtoParticle *, ParticleHistory::Map > ParticleProtoMap;

  /** @class HistoHandle ParticleEffPurMoni.h
   *  Handle a set of histograms
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
  class HistoHandle
  {
  public:
    /// Constructor
    HistoHandle()
      : m_effVp   (0*Gaudi::Units::GeV,100*Gaudi::Units::GeV,50),
        m_effVpt  (0*Gaudi::Units::GeV, 10*Gaudi::Units::GeV,50)
               //m_effVpVpt(0*Gaudi::Units::GeV,100*Gaudi::Units::GeV,
               //           0*Gaudi::Units::GeV, 10*Gaudi::Units::GeV,
               //           20,20)
    { }
  public:
    /// Fill all histos
    inline void fillAll( const double ptot, const double pt )
    {
      m_effVp.fill(ptot);
      m_effVpt.fill(pt);
      //m_effVpVpt.fill(ptot,pt);
    }
  public:
    /// Access the P histogram
    inline EffVersusMomentum & effVp() const { return m_effVp; }
    /// Access the Pt histogram
    inline EffVersusMomentum & effVpt() const { return m_effVpt; }
    /// Access the P & Pt histogram
    //inline EffVersusMomentum2D & effVpVpt() const { return m_effVpVpt; }
  private:
    mutable EffVersusMomentum   m_effVp;       ///< Eff versus momentum (p) data
    mutable EffVersusMomentum   m_effVpt;      ///< Eff versus momentum (pt) data
    // mutable EffVersusMomentum2D m_effVpVpt;    ///< Eff versus momentum (p&pt) data
  };

  /** @class MCTally ParticleEffPurMoni.h
   *  Simple tally map for each particle type
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
  class MCTally
  {
  public:
    /// Default Constructor
    MCTally() : notclones(0), clones(0) { }
    /// Returns # of all tracks
    unsigned long int all() const { return notclones+clones; }
  public:
    unsigned long int notclones;   ///< Total number of non-clones
    unsigned long int clones;      ///< Number of clones
    typedef std::map<std::string,unsigned long int> Contributions;
    typedef std::map<std::string,HistoHandle> DetailedHistos;
    mutable Contributions notclones_detailed;    ///< Detailed breakdown for each full MC tree decay (not clones)
    mutable Contributions clones_detailed; ///< Detailed breakdown for each full MC tree decay (clones)
    HistoHandle histos;
    mutable DetailedHistos histos_detailed;
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

  /** @class ProtoTESStats ParticleEffPurMoni.h
   *  ProtoParticle TES statistics summary class
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2007-02-21
   */
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
  typedef std::map< std::string, ProtoTESStatsMap > ParticleTESStatsMap;

  /// Map between Particles and MCParticles
  typedef std::map<const LHCb::ProtoParticle*,const LHCb::MCParticle*> P2MCP;

  typedef std::pair<std::string,std::string> StringPair;

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

  /** Returns the full location of the given object in the Data Store
   *
   *  @param pObj contained object
   *
   *  @return Location of given data object
   */
  inline std::string objectLocation( const ContainedObject * pObj ) const
  {
    return ( pObj ? objectLocation(pObj->parent()) : "Null ContainedObject !" );
  }

  /// Create a unique list of Particles to study
  void addParticle( const LHCb::Particle * particle,
                    const LHCb::Particle * firstParticle,
                    const unsigned int recurCount = 0,
                    const std::string & history   = "",
                    const bool toplevel           = true ) const;

  /// Printout statistics
  void printStats() const;

  /// Returns the momentum value for a given ProtoParticle
  inline double momentum( const LHCb::ProtoParticle * proto ) const
  {
    // CRJ : Need to decide what to do for neutrals
    return ( NULL != proto->track() ? proto->track()->p() : 0 );
  }

  /// Returns the transverse momentum value for a given ProtoParticle
  inline double transverseMomentum( const LHCb::ProtoParticle * proto ) const
  {
    // CRJ : Need to decide what to do for neutrals
    return ( NULL != proto->track() ? proto->track()->pt() : 0 );
  }

  /// Returns the ParticleProperty object for a given ParticleID
  const LHCb::ParticleProperty * partProp( const LHCb::ParticleID& id ) const;

  /// Access the Particle Linker appropriate for the given Particle
  Particle2MCLinker * particleLinker( const LHCb::Particle * /* part */ ) const;

  /// Access the ProtoParticle Linker appropriate for the given ProtoParticle
  ProtoParticle2MCLinker * protoLinker( const LHCb::ProtoParticle * proto ) const;

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

  /// Make a 2D histogram
  void makeEffHisto( const std::string title,
                     const EffVersusMomentum2D & top,
                     const EffVersusMomentum2D & bot ) const;

  /// ProtoParticle short location name
  const std::string & shortProtoLoc( const std::string & loc ) const;

  /// ProtoParticle short location name
  const std::string & shortPartLoc( const std::string & loc ) const;

  /// Access on demand the truth tool
  const Rich::Rec::MC::IMCTruthTool * truthTool() const
  {
    if ( !m_truthTool )
    {
      m_truthTool = tool<Rich::Rec::MC::IMCTruthTool>( "Rich::Rec::MC::MCTruthTool",
                                                       "RichRecMCTruth" );
    }
    return m_truthTool;
  }

  /// Creates a unique string to identify the correlation of two ProtoParticle locations
  inline std::string corProtoLocation( const std::string& protoloc1,
                                       const std::string& protoloc2 ) const
  {
    return ( protoloc1 < protoloc2 ?
             shortProtoLoc(protoloc1)+"&"+shortProtoLoc(protoloc2) :
             shortProtoLoc(protoloc2)+"&"+shortProtoLoc(protoloc1) );
  }

  /// Creates a unique string to identify the correlation of two ProtoParticle locations
  inline std::string corPartLocation( const std::string& loc1,
                                      const std::string& loc2 ) const
  {
    return ( loc1 < loc2 ?
             shortPartLoc(loc1)+"&"+shortPartLoc(loc2) :
             shortPartLoc(loc2)+"&"+shortPartLoc(loc1) );
  }

  /// Make a set of plots
  void makeAllPlots1( const std::string hpath,
                      const HistoHandle & partTally,
                      const HistoHandle & protoTally,
                      const HistoHandle & mcTally ) const;

  /// Make a set of plots
  void makeAllPlots2( const std::string hpath,
                      const HistoHandle & partTally,
                      const HistoHandle & mcTally,
                      const StringPair& corName,
                      const HistoHandle & corTally ) const;

  /// Compute the poisson efficiencies
  inline Rich::PoissonEffFunctorResult eff( double top, double bot ) const
  {
    static const Rich::PoissonEffFunctor _eff("%7.2f +-%5.2f");
    // CRJ : temporary hack. Need to find out properly what is causing this
    if ( bot < top ) top = bot;
    return _eff(top,bot);
  }

private: // data

  /// MC truth tool
  mutable const Rich::Rec::MC::IMCTruthTool * m_truthTool;

  /// Particle/Proto map
  mutable ParticleProtoMap m_partProtoMap;

  /// Track selector tool
  ITrackSelector * m_trSel;

  /// MC Reconstructible tool
  IMCReconstructible * m_mcRec;

  /// MCParticle selector
  IMCParticleSelector * m_mcSel;

  /// MC Associations for Particles
  mutable Particle2MCLinker * m_particleLinker;

  /// Statis for Particle and ProtoParticle locations
  mutable ParticleTESStatsMap m_partProtoTESStats;

  /// TES location -> linker map type
  typedef std::map<std::string,ProtoParticle2MCLinker *> ProtoLinkerTESMap;

  /// MC Associations for ProtoParticles
  mutable ProtoLinkerTESMap m_protoLinker;

  /// The overall tally
  mutable LocationMap m_locMap;

  /// Total true number of particles of each type at each TES location
  mutable MCSummaryMapAllProtos m_mcProtoCount;

  /// Protoparticle reco correlations
  mutable MCSummaryMapAllProtos m_correlations;

  /// Raw MC informtion
  mutable MCRecTypeMap m_rawMCMap;

  /// Short name mapping for ProtoParticle locations
  mutable std::map<std::string,std::string> m_ShortNames;
  /// Number of ProtoParticle locations
  mutable unsigned int m_protoCount;
  /// Number of Particle locations
  mutable unsigned int m_partCount;

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

};

#endif // PARTICLEMAKERCHECKER_ParticleEffPurMoni_H
