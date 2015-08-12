#ifndef KERNEL_DVCOMMONBASE_H 
#define KERNEL_DVCOMMONBASE_H 1

// ============================================================================
#include <map>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/HashMap.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// from LHCb
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// from DaVinciInterfaces
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IDecayTreeFit.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Kernel/IMassFit.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IDirectionFit.h"
// ============================================================================
// from DaVinciTypes
// ============================================================================
#include "Kernel/Particle2Vertex.h"
// ============================================================================
// from DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciFun.h"
// ============================================================================
// from DaVinciUtils
// ============================================================================
#include "DaVinciUtils/Guards.h"
// ============================================================================

/** @class DVCommonBase DVCommonBase.h Kernel/DVCommonBase.h
 *
 *  Base Class for DaVinci Selection Algorithms:
 *  Does the retrieval of all necessary DaVinci Tools
 *
 *  The specific configuration properties of the base class:
 *
 *  - <b>Inputs</b> : TES location of input data container.
 *
 *  - <b>Output</b> : TES location of output data container.
 *
 *  - <b>VertexFitters</b> : the map for possible vertex fitters.
 *      @see IVertexFit
 *      The semantic is
 *   @code
 *      MyAgl.VertexFitters = { "" : "OfflineVertexFitter" ,
 *                              "Trigger" : "TrgVertexFitter" ,
 *                              "special" : "MySpecialVertexFitter" } ;
 *   @endcode
 *
 *   - <b>Distance Tools</b>  : the map of possible geometry tools
 *   @see IDistanceCalculator
 *
 *  - <b>ParticleFilters</b> : the map for possible particle filters
 *     @see IParticleFilter
 *
 *  - <b>ParticleCombiners</b> : the map for possible particle combiners
 *     @see IParticleCombiner
 *
 *  - <b>ParticleReFitters</b> : the map for possible particle re-fitters
 *     @see IParticleReFit
 *
 *  - <b>PVReFitters</b> : the map for possible primary vertex re-fitters
 *     @see IPVReFitter
 *
 *  - <b>ReFitPVs</b> : bool. Perform automatic PV re-fitting when asking for
 * best PV. Default: false.
 *
 *  - <b>UseP2PVRelations</b> : bool. Use P2PV relations internally
 * or calculate best PV on the fly. Forced to true if <b>ReFitPVs</b> is true.
 * Default: true.
 *
 *  - <b>WriteP2PVRelations</b> : bool. Write P2PV relations to
 * output TES location "<algorithm output location>/Particle2VertexRelations".
 * Writes the relations for all particles in the TES after saving local
 * particles, including relations for input particles. Does not write anything
 * Default: true.
 *
 *  - <b>IgnoreP2PVFromInputLocations</b> : Do not pick up <Inpus>"/Particle2VertexRelations".
 * Forces to calculate best PV on the fly. To be used specially if you want
 * to use ReFitPVs when the input algorithms didn't or vice versa.
 * Or to be sure that you are re-calculating the best PV in your own algorithm.
 * Bear in mind this involves potentially CPU expensive calculations.
 * Default: false.
 *
 *  - <b>DecayDescriptor</b>  : the decay descriptor ofthe algorithm
 *               in the spirit of (MC)DecayFinder tool by Olivier Dormond.
 *
 *
 *  - <b>ForceOutput</b> : boolean flag (default value is <c>true</c>)
 *    forcing the wrinting of empty containers if no output data are required.
 *
 *  - <b>PreloadTools</b> : the boolean flag (default value is <c>true</c>)
 *    to force the preloading of all nesessary tools at the initialization time
 *
 *  @author Chris Jones
 *  @date   2012-06-22
 */

template <class PBASE>
class DVCommonBase : public extends1<PBASE,IDVAlgorithm>
{

public:

  /// Standard constructor
  DVCommonBase( const std::string& name,
                ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~DVCommonBase( ) { }

public:

  // ==========================================================================
  /** Handle to the concrete implementation, or to the parent if
   *  implementation does not derive from GaudiAlgorithm
   */
  virtual const GaudiAlgorithm* gaudiAlg() const { return this; }

  /// get the best related primary vertex
  virtual const LHCb::VertexBase*
  bestVertex( const LHCb::Particle* particle ) const
  {
    return bestPV(particle);
  }

  /// get all loaded input particles
  virtual const LHCb::Particle::Range particles() const
  {
    return LHCb::Particle::Range ( m_inputParts.begin() ,
                                   m_inputParts.end()   ) ;
  }

  /** direct const access to container of input primary vertices.
   *  @author Juan Palacios juan.palacios@nikhef.nl
   */
  inline const LHCb::RecVertex::Range primaryVertices() const
  {
    return this -> template getIfExists<LHCb::RecVertex::Range>(m_PVLocation);
  }

  /** Accessor for IDistanceCalculator tools by name/typename/nickname
   *  @see IDistanceCalculator
   *  @param name the tool name/typename/nickname
   *  @return pointer to acquired tool
   */
  const IDistanceCalculator*
  distanceCalculator ( const std::string& name = "" ) const
  {
    return this->getTool<IDistanceCalculator>
      ( name ,
        m_distanceCalculatorNames ,
        this->defaultDistanceCalculatorsNames() ,
        m_distanceCalculators     , this ) ;
  }

  /** Accessor for ILifetimeFitter tools by name/typename/nickname
   *  @see ILifetimeFitter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const ILifetimeFitter*
  lifetimeFitter ( const std::string& name = "" ) const
  {
    return this->getTool<ILifetimeFitter>
      ( name                  ,
        m_lifetimeFitterNames ,
        this->defaultLifetimeFittersNames() ,
        m_lifetimeFitters     , this ) ;
  }

  /** Accessor for IVertexFit tools by name/typename/nickname
   *  @see IVertexFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IVertexFit*
  vertexFitter ( const std::string& name = "" ) const
  {
    return this->getTool<IVertexFit>
      ( name ,
        m_vertexFitNames ,
        this->defaultVertexFittersNames() ,
        m_vertexFits     , this ) ;
  }

  /** Accessor for IParticleReFitter tools by name/typename/nickname
   *  @see IParticleReFitter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IParticleReFitter*
  particleReFitter ( const std::string& name = "" ) const
  {
    return this->getTool<IParticleReFitter>
      ( name ,
        m_particleReFitterNames ,
        this->defaultParticleReFittersNames() ,
        m_particleReFitters     , this ) ;
  }

  /** Accessor for IDecayTreeFit tools by name/typename/nickname
   *  @see IDecayTreeFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  IDecayTreeFit*
  decayTreeFitter ( const std::string& name = "" ) const
  {
    return this->getTool<IDecayTreeFit>
      ( name ,
        m_decayTreeFitterNames ,
        this->defaultDecayTreeFittersNames() ,
        m_decayTreeFitters     , this ) ;
  }

  /** Accessor for IParticleCombiner tools by name/typename/nickname
   *  @see IParticleCombiner
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IParticleCombiner*
  particleCombiner ( const std::string& name = "" ) const
  {
    return this->getTool<IParticleCombiner>
      ( name ,
        m_particleCombinerNames ,
        this->defaultParticleCombinersNames() ,
        m_particleCombiners     , this ) ;
  }

  /** Accessor for IMassFit tools by name/typename/nickname
   *  @see IMassFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IMassFit*
  massFitter ( const std::string& name = "" ) const
  {
    return this->getTool<IMassFit>
      ( name              ,
        m_massFitterNames ,
        this->defaultMassFittersNames() ,
        m_massFitters     , this ) ;
  }

  /** Accessor for IDirectionFit tools by name/typename/nickname
   *  @see IDirectionFit
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IDirectionFit*
  directionFitter ( const std::string& name = "" ) const
  {
    return this->getTool<IDirectionFit>
      ( name                   ,
        m_directionFitterNames ,
        this->defaultDirectionFittersNames() ,
        m_directionFitters     , this ) ;
  }

  /** Accessor for IPVReFitter tools by name/typename/nickname
   *  @see IPVReFitter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IPVReFitter*
  primaryVertexReFitter ( const std::string& name = "" ) const
  {
    return this->getTool<IPVReFitter>
      ( name ,
        m_pvReFitterNames ,
        this->defaultPVReFittersNames() ,
        m_pvReFitters     , this ) ;
  }

  /** Accessor for Particle Filter Tool
   *  @see IPArticleFilter
   *  @param name the tool name/typename/nickname
   *  @return pointer to aquired tool
   */
  const IParticleFilter*
  particleFilter ( const std::string& name = "" ) const
  {
    return this->getTool<IParticleFilter>
      ( name          ,
        m_filterNames ,
        this->defaultParticleFiltersNames() ,
        m_filters     , this ) ;
  }

  /// Tagging Tool
  IBTaggingTool* flavourTagging() const
  {
    return this->getTool<IBTaggingTool>( m_taggingToolName,
                                         m_taggingTool,
                                         this );
  }

  // ==========================================================================
public:
  // ==========================================================================

  /// Overridden from Gaudi Algo to produce a warning if not called by user
  virtual void setFilterPassed( bool state );

  /// Overridden from Gaudi Algorithm
  virtual StatusCode sysExecute ();

  // Overridden from Gaudi Algorithm
  virtual StatusCode initialize ();

  // Overridden from Gaudi Algorithm
  virtual StatusCode finalize ();

  // ==========================================================================
public:
  // ==========================================================================

  // Get decay descriptor
  const std::string& getDecayDescriptor() const
  {
    return m_decayDescriptor;
  }

  // Set decay descriptor
  void setDecayDescriptor(const std::string& dd)
  {
    m_decayDescriptor = dd;
  }

  /**
   * Direct const access to the tool that calculates the Particle->PV
   * weighted relations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   **/
  inline const IRelatedPVFinder* relatedPVFinder() const
  {
    return this->getTool<IRelatedPVFinder>( m_pvRelatorName,
                                            m_pvRelator, this );
  }

  /**
   * Cached access to default IPVReFitter tool.
   *
   * @author Juan Palacios palacios@physik.uzh.ch
   *
   * @attention TO be removed. Use primaryVertexReFitter() instead.
   **/
  inline const IPVReFitter* defaultPVReFitter() const
  {
    return this->primaryVertexReFitter();
  }

  /**
   * Inline access to best PV for a given particle.
   *
   **/
  inline const LHCb::VertexBase*
  bestPV(const LHCb::Particle* particle) const
  {
    return i_bestVertex(particle);
  }

  /**
   * Calculate the best related PV for a particle and return it to the user
   * If property "ReFitPVs" is set to true, this triggers a re-fit of the PVs
   * after removing tracks coming from the particle in question. If not, then
   * it uses the container of PVs obtained from method primaryVertices().
   * Returns a newed pointer to a vertex either from the TES or stored in the
   * local storage.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @param p LHCb::Particle to be related
   * @return newed pointer to related vertex. TES or DV Algorithm in charge of
   * memory management.
   **/
  const LHCb::VertexBase* calculateRelatedPV(const LHCb::Particle* p) const;

  /**
   * Get the related PV from the relations table, or call calculateRelatedPV
   * if it isn't already there.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 10/02/2009
   **/
  const LHCb::VertexBase* getRelatedPV(const LHCb::Particle* p) const;

  /**
   * Relate a VertexBase to a Particle.
   * Overwrites existing relation to that particle.
   *
   * @param part (INPUT) LHCb::Particle to which an LHCb::VertexBase
   *                     will be related
   * @param vert (INPUT) LHCb::VertexBase that will be related to part.
   *
   * @author Juan Palacios palacios@physik.uzh.ch
   **/
  inline void relate(const LHCb::Particle*   particle,
                     const LHCb::VertexBase* vertex) const
  {
    if ( particle && vertex ) { m_p2PVMap[particle] = vertex; }
  }

  /**
   *
   * Remove the PV relation of a particle if there is one.
   *
   * @param particle
   * @author Juan Palacios palacios@physik.uzh.ch
   **/
  inline void unRelatePV(const LHCb::Particle* particle) const
  {
    m_p2PVMap.erase(particle);
  }

public:

  /// Accessor for CheckOverlap Tool
  inline ICheckOverlap* checkOverlap()const
  {
    return this->getTool<ICheckOverlap>(m_checkOverlapName,m_checkOverlap);
  }

  /// Descendants
  inline IParticleDescendants* descendants() const
  {
    return this->getTool<IParticleDescendants>( m_descendantsName,
                                                m_descendants );
  }

  // ==========================================================================

  /** Accessor for ParticlePropertySvc
   *  @see LHCb::ParticlePropertySvc
   *  @return pointer to Particle Property Service
   */
  inline const LHCb::IParticlePropertySvc* ppSvc() const
  {
    if ( !m_ppSvc )
    {
      m_ppSvc =
        this -> template svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" ,
                                                           true ) ;
    }
    return m_ppSvc;
  }

  /** helper method to get a proper ParticleProperty for the given name
   *
   *  @code
   *
   *   const LHCb::ParticleProperty* pion = pid ( "pi+" ) ;
   *
   *  @endcode
   *
   *  @see LHCb::ParticleProperty
   *  @see LHCb::IParticlePropertySvc
   *  @param name the particle name
   *  @return pointer to particle property
   */
  inline const LHCb::ParticleProperty* pid ( const std::string& name ) const
  {
    const LHCb::ParticleProperty* pp = ppSvc()->find( name ) ;
    if ( !pp )
    {
      this->Error ( "pid('" + name + "') : invalid LHCb::ParticleProperty!" ).ignore();
    }
    return pp ;
  }

  /** helper method to get a proper ParticleProperty for the given pid
   *
   *  @code
   *
   *   const LHCb::Particle* p = ... ;
   *
   *   const LHCb::ParticleProperty* pp = pid ( p->particleID() ) ;
   *
   *  @endcode
   *
   *  @see LHCb::ParticleProperty
   *  @see LHCb::IParticlePropertySvc
   *  @see LHCb::ParticleID
   *  @param  id the particle pid
   *  @return pointer to particle property
   */
  inline const LHCb::ParticleProperty* pid ( const LHCb::ParticleID& id ) const
  {
    const LHCb::ParticleProperty* pp = ppSvc()->find ( id ) ;
    if ( !pp )
    {
      this->Error ( "pid() : invalid LHCb::ParticleProperty!" ).ignore();
    }
    return pp ;
  }

  //===========================================================================

protected:

  /// the actual type for mapping "tool nickname -> the actual type/name"
  typedef std::map<std::string,std::string> ToolMap;

  //===========================================================================

private:

  /** helper protected function to load the tool on-demand
   *  @param name name of tool
   *  @param t    tool
   *  @param ptr the pointer to this or NULL for private or common tools
   *  @return tool
   */
  template<class TYPE>
  TYPE* getTool ( const std::string& name,
                  TYPE *& t,
                  const IInterface* ptr = NULL ) const
  {
    if ( !t )
    {
      t = this -> template tool<TYPE>( name, ptr );
    }
    return t;
  }

  /** helper method to locate the tool by nickname
   *
   *  @attention it is for internal usage ONLY,
   *             used for implementation of
   *             concrete accessor functions
   *
   *  It is assumed that the map "nickname -> type/name" is performed
   *  through the algorithm properties
   *
   *  @param nickName the nickname for the tool
   *  @param nameMap  the actual mapping "nickname -> type/name"
   *                 (to be specified through the properties)
   *  @param toolMap  the actual storage of located tool, e.g.
   *         std::map<std::string,TYPE*> or
   *         Gaudi::Utils::VectorMap<std::string,TYPE*>
   *  @param parent the parent of the tools
   *  @return the located tool
   */
  template <class TYPE, class STORAGE>
  TYPE* getTool ( const std::string& nickName      ,
                  const ToolMap&     customMap     ,
                  const ToolMap&     defaultMap    ,
                  STORAGE&           toolMap       ,
                  const IInterface*  parent = NULL ) const
  {
    TYPE* t = NULL;
    // look within the local list of already located tools of given type
    typename STORAGE::const_iterator ifind = toolMap.find ( nickName );
    // tool is in the list?
    if ( toolMap.end() != ifind )
    {
      t = ifind->second;
      if ( !t )
      {
        this->Exception ( "getTool<" +
                          System::typeinfoName( typeid ( TYPE ) )
                          + ">('" + nickName +
                          "'): tool points to NULL" ) ;
      }
    }
    else
    {
      // get the actual tool type
      std::string toolType = nickName;
      ToolMap::const_iterator iname = customMap.find ( nickName );
      if ( iname != customMap.end() )
      {
        toolType = iname->second;
      }
      else
      {
        iname = defaultMap.find ( nickName );
        if ( iname != defaultMap.end() ) { toolType = iname->second; }
      }
      // locate the tool
      t = this -> template tool<TYPE> ( toolType , parent );
      // add the located tool into the container
      typename STORAGE::value_type value( nickName , t );
      toolMap.insert( value );
    }
    return t;                                               // RETURN
  }

  //===========================================================================

protected:

  /**
   * Access to the list of TES input locations given by the Inputs
   * property
   *
   * @return vector or strings with TES input locations
   */
  inline const std::vector<std::string>& inputLocations() const
  {
    return m_inputLocations;
  }

  /**
   * Access to the list of TES input locations given by the Inputs
   * property
   *
   * @return vector or strings with TES input locations
   */
  inline std::vector<std::string>& inputLocations()
  {
    return m_inputLocations;
  }

  /// Marks a single particle for saving, ignoring it's decay tree.
  /// Particle must be on the heap, either already in the TES or
  /// newed by the client. Particle's decay products must be in the TES.
  /// If these conditions are not met, saving to the TES via saveInTES will
  /// fail.
  ///
  /// @attention use markTree, markNewTree, cloneAndMarkTree methods
  /// unless you are really sure all the decay products are in the TES.
  ///
  /// @attention if <b>particle</b> is on the TES they will not be saved
  /// by default, special action is required via over-writing of _saveInTES.
  ///
  inline void markParticle( const LHCb::Particle* particle )
  {
    if ( m_parts.end() == std::find ( m_parts.begin() ,
                                      m_parts.end()   ,
                                      particle        ) )
    { m_parts.push_back(particle); }
  }

  /// Mark particles for saving, ignoring it's decay tree.
  /// Particles must be on the heap, either already in the TES or
  /// newed by the client. Particle's decay products must be in the TES.
  /// If these conditions are not met, savinf to the TES via saveInTES will
  /// fail.
  ///
  /// @attention use markTrees, markNewTrees, cloneAndMarkTrees methods
  /// unless you are really sure all the decay products are in the TES.
  ///
  /// @attention if <b>particles</b> are on the TES they will not be saved
  /// by default, special action is required via over-writing of _saveInTES.
  ///
  template<class PARTICLES>
  inline void markParticles( const PARTICLES& particles )
  {
    for ( typename PARTICLES::const_iterator iPart = particles.begin();
          iPart != particles.end(); ++iPart ) { markParticle(*iPart); }
  }

  /// Mark a decay tree for saving. Scans decay tree marking
  /// elements for saving. Each branch is followed and each vertex
  /// is cloned until a vertex is found which is in the TES.
  ///
  /// @attention Cloning stops at vertices that are in the TES.
  /// @param particle (INPUT) The head of the decay to be cloned and marked.
  /// Ownership remains unchanged (either client's or TES).
  /// @return the cloned head of the decay or the input if in TES.
  ///
  const LHCb::Particle* markTree(const LHCb::Particle* particle);

  /// Mark particles for saving. Scans decay trees marking
  /// elements for saving. Each branch is followed and each vertex
  /// is cloned until a vertex is found which is in the TES.
  ///
  /// @attention Cloning stops at vertices that are in the TES.
  /// @param heads (INPUT) The heads of the decays to be cloned and marked.
  /// Ownership remains unchanged (either client's or TES).
  ///
  template <class PARTICLES>
  inline void markTrees(const PARTICLES heads)
  {
    for ( typename PARTICLES::const_iterator iHead = heads.begin();
          iHead != heads.end(); ++iHead) { markTree(*iHead); }
  }

  /// Clone a particle and mark for saving. Scans descendants cloning
  /// and marking elements for saving. Each branch is followed until
  /// a vertex is found which is in the TES.
  ///
  /// @attention particle is always cloned, but cloning of decays
  /// stops at vertices that are in the TES.
  /// @param particle (INPUT) The head of the decay to be cloned and marked.
  /// Ownership remains unchanged (either client's or TES).
  /// @return the cloned head of the decay or the input if in TES.
  ///
  const LHCb::Particle* cloneAndMarkTree(const LHCb::Particle* particle);

  /// Clone particles and mark for saving. Scans descendants cloning
  /// and marking elements for saving. Each branch is followed until
  /// a vertex is found which is in the TES.
  ///
  /// @attention head is always cloned, but cloning of decays
  /// stops at vertices that are in the TES.
  /// @param heads (INPUT) The heads of the decays to be cloned and marked.
  /// Ownership remains unchanged (either client's or TES).
  /// @return the cloned head of the decay or the input if in TES.
  ///
  template <class PARTICLES>
  void cloneAndMarkTrees(const PARTICLES& heads)
  {
    for ( typename PARTICLES::const_iterator iHead = heads.begin();
          iHead != heads.end(); ++iHead ) { cloneAndMarkTree(*iHead); }
  }

  ///
  /// Mark Particles head and its decay products for saving.
  /// Elements already on the TES or their descendants will not be marked.
  /// @param head (INPUT) Particle, head of decays to be stored.
  ///      Algorithm takes over ownership. Element <b>must be on the heap</b>.
  ///
  inline void markNewTree(const LHCb::Particle* head)
  {
    DaVinci::Utils::findDecayTree( head, m_parts, m_secVerts, &m_inTES);
  }

  ///
  /// Mark the particles in heads and their decay products for saving.
  /// Elements already on the TES or their descendants will not be marked.
  /// @param heads (INPUT) vector of heads of decays to be stored.
  ///      Algorithm takes over ownership. Elements <b>must be on the heap</b>.
  ///
  template<class PARTICLES>
  inline void markNewTrees(const PARTICLES& heads)
  {
    for ( typename PARTICLES::const_iterator iHead = heads.begin();
          iHead != heads.end(); ++iHead ) { markNewTree(*iHead); }
  }

  /// Return the output location where data will be written to the TES
  inline const std::string& outputLocation() const
  {
    return m_outputLocation;
  }
  /// Return the output location where Particles will be written to the TES
  inline const std::string& particleOutputLocation() const
  {
    return m_particleOutputLocation;
  }
  /// Return the output location where decay vertices will be written
  /// to the TES
  inline const std::string& decayVertexOutputLocation() const
  {
    return m_decayVertexOutputLocation;
  }

  /// Return the output location where data will be written to the TES
  inline const std::string& tableOutputLocation() const
  {
    return m_tableOutputLocation;
  }

  /// Get the best related PV from the local relations table. Return 0 if
  /// nothing is there. Does not invoke any calculations.
  inline const LHCb::VertexBase*
  getStoredBestPV(const LHCb::Particle* particle) const
  {
    P2PVMap::const_iterator iPV = m_p2PVMap.find(particle);
    return ( iPV != m_p2PVMap.end() ? iPV->second : NULL );
  }

  /// Inline access to local input Particle storage.
  inline const LHCb::Particle::ConstVector& i_particles() const
  {
    return m_inputParts;
  }

  /// Inline access to local marked Particle storage.
  inline const LHCb::Particle::ConstVector& i_markedParticles() const
  {
    return m_parts;
  }

private:

  /** Returns the full location of the given object in the Data Store
   *
   *  @param pObj Data object
   *
   *  @return Location of given data object
   */
  inline std::string objectLocation( const DataObject * pObj ) const
  {
    return ( !pObj ? "" :
             (pObj->registry() ? pObj->registry()->identifier() : "") );
  }

  /** Try and find the related PV by predicting the relations table
   *  location from the Particle TES container location
   */
  const LHCb::VertexBase* 
  relatePVViaParticleTESLoc(const LHCb::Particle* p) const;

  /**
   * Inline access to best PV for a given particle.
   *
   **/
  inline const LHCb::VertexBase* i_bestVertex(const LHCb::Particle* p) const
  {
    return ( useP2PV() ? getRelatedPV(p) : calculateRelatedPV(p) );
  }

  /// Initialise relative Inputs 
  void initializeInputLocations();

  /// Initialise Particle->PV relations input locations.
  void initializeOutputLocations();

  /// Load all tools.
  /// The base class provides an instance of all type of tools
  StatusCode loadTools() ;

  /// Load particles, primary vertices, relations tables.
  virtual StatusCode loadEventInput();

  /// Load particles from Inputs
  StatusCode loadParticles();

  /// Load Particle->PV relations for loaded particles.
  StatusCode loadParticle2PVRelations();

  /// Take a range of Particle -> PV relations and store them locally,
  /// overwriting existing relations with the same From.
  void loadRelations(const Particle2Vertex::Table::Range& relations) const;

  /// Does the particle have a relation to a PV stored in the local
  /// relations table?
  inline bool hasStoredRelatedPV(const LHCb::Particle* particle) const
  {
    return ( m_p2PVMap.find(particle) != m_p2PVMap.end() );
  }

  /// Does the event have more than 1 primary vertex?
  inline bool multiPV() const
  {
    return ( this->primaryVertices().size() > 1 );
  }

  /// Should PVs be re-fitted when bestVertex is asked for?
  inline bool refitPVs() const
  {
    return m_refitPVs;
  }

  /// Should Particle->PV relations table be used?
  inline bool useP2PV() const
  {
    return ( refitPVs() ? true : m_useP2PV );
  }

  /// Should Particle->PV relations be stored in the TES?
  inline bool saveP2PV() const
  {
    return ( m_writeP2PV && !m_noPVs );
  }

  /// Save the local Particle->Vertex relations table to the TES
  StatusCode saveP2PVRelations() const;

  /// Scan the marked particles' decay trees and get or calculate
  /// best PV relations and store them on the local map.
  void buildP2PVMap() const;

  /// Mark a local PV for saving.
  const LHCb::RecVertex* mark(const LHCb::RecVertex* PV) const;

  template<class PARTICLES>
  void i_markTrees(const PARTICLES& heads)
  {
    for ( typename PARTICLES::const_iterator iHead = heads.begin();
          iHead != heads.end(); ++iHead )
    {
      DaVinci::Utils::findDecayTree( *iHead, m_parts,
                                     m_secVerts, &m_inTES );
    }
  }

  /// Save local re-fitted PVs related to saved particles.
  /// Only saves PVs that are not already in the TES.
  void saveRefittedPVs(const LHCb::RecVertex::ConstVector& vToSave) const;

  /// Save all marked local particles not already in the TES.
  /// Saves decay tree elements not already in TES, plus related
  /// PV and relation table entry when applicable.
  StatusCode saveInTES();

protected:

  /// Save all marked local particles not already in the TES.
  /// Saves decay tree elements not already in TES
  /// This method can be overwritten if special TES saving actions are
  /// required. Avoid if possible!
  virtual StatusCode _saveInTES();


  /// Write an empty Particles container of the same type as that in
  /// saveInTES(). Can be overwritten for specialist actions.
  virtual void writeEmptyTESContainers() ;
  
  /// handle the update of "Inputs" property"
  void handleInputsUpdate ( Property& /* p */ ) ;  // update the factory
  
private:

  std::string m_outputLocation; ///< Output location TES folder.

  std::string m_particleOutputLocation; ///< Output location TES folder.

  std::string m_decayVertexOutputLocation; ///< Output location TES folder.

  std::string m_tableOutputLocation; ///< Output location TES folder.

  LHCb::Particle::ConstVector m_parts;      ///< Local Container of local particles

  LHCb::Particle::ConstVector m_inputParts; ///< Local container of input particles.

  LHCb::Vertex::ConstVector m_secVerts;     ///< Local Container of secondary vertices

  mutable LHCb::RecVertex::ConstVector m_refittedPVs;  ///< Local Container of re-fitted primary vertices

  /// Type for mapping between Particles and PVs
  typedef GaudiUtils::HashMap<const LHCb::Particle*, const LHCb::VertexBase*> P2PVMap;
  //typedef std::map<const LHCb::Particle*, const LHCb::VertexBase*> P2PVMap;

  mutable P2PVMap m_p2PVMap; ///< Local store of Particle->PV relations.

  /// Related PV finder type
  std::string m_pvRelatorName;

protected:

  /// Mapping of "nickname -> type/name" for Vertex Fitters:
  ToolMap                                                    m_vertexFitNames ;
  /// The actual map of "nickname -> tool" for Vertex Fitters
  mutable GaudiUtils::VectorMap<std::string,IVertexFit*>         m_vertexFits ;

  /// Mapping of "nickname ->type/name" for Geometry Tools
  ToolMap                                                     m_geomToolNames ;

  /// Mapping of "nickname ->type/name" for Particle Filters
  ToolMap                                                       m_filterNames ;
  /// The actual map of "nickname -> tool" for Particle Filters
  mutable GaudiUtils::VectorMap<std::string,IParticleFilter*>       m_filters ;

  /// Mapping of "nickname ->type/name" for Particle Combiners
  ToolMap                                             m_particleCombinerNames ;
  /// The actual map of "nickname -> tool" for Particle Combiners
  mutable GaudiUtils::VectorMap<std::string,IParticleCombiner*> m_particleCombiners ;

  /// Mapping of "nickname ->type/name" for Particle Refitters
  ToolMap                                             m_particleReFitterNames ;
  /// The actual map of "nickname -> tool" for Particle Refitters
  mutable GaudiUtils::VectorMap<std::string,IParticleReFitter*> m_particleReFitters ;

  /// Mapping of "nickname ->type/name" for Particle Refitters
  ToolMap                                             m_pvReFitterNames ;
  /// The actual map of "nickname -> tool" for Particle Refitters
  mutable GaudiUtils::VectorMap<std::string,IPVReFitter*> m_pvReFitters ;

  /// Mapping of "nickname ->type/name" for Decay Tree Fitters
  ToolMap                                                   m_decayTreeFitterNames  ;
  /// The actual map of "nickname -> tool" for Decay Tree fitters
  mutable GaudiUtils::VectorMap<std::string,IDecayTreeFit*> m_decayTreeFitters ;
  /// Mapping of "nickname ->type/name" for mass-constrained fitters
  ToolMap                                              m_massFitterNames ;
  /// The actual map of "nickname -> tool" for mass-constrained fitters
  mutable GaudiUtils::VectorMap<std::string,IMassFit*> m_massFitters     ;

  /// Mapping of "nickname ->type/name" for lifetime fitters
  ToolMap                                              m_lifetimeFitterNames   ;
  /// The actual map of "nickname -> tool" for lifetime fitters
  mutable GaudiUtils::VectorMap<std::string,ILifetimeFitter*> m_lifetimeFitters ;

  /// Mapping of "nickname ->type/name" for direction fitters
  ToolMap                                              m_directionFitterNames   ;
  /// The actual map of "nickname -> tool" for direction fitters
  mutable GaudiUtils::VectorMap<std::string,IDirectionFit*> m_directionFitters ;

  /// mapping of "nickname -> tool" for distance calculators
  ToolMap                                                         m_distanceCalculatorNames ;
  /// the actual map of "nickname -> distance calculator"
  mutable GaudiUtils::VectorMap<std::string,IDistanceCalculator*> m_distanceCalculators     ;

protected:

  /// Concrete type of CheckOverlap tool
  std::string m_checkOverlapName;

  /// Reference to CheckOverlap
  mutable ICheckOverlap* m_checkOverlap;

  /// Concrete Type of FlavourTagging tool
  std::string m_taggingToolName;
  /// Reference to FlavourTagging
  mutable IBTaggingTool* m_taggingTool;

  /// Reference to ParticleDescendants
  mutable IParticleDescendants* m_descendants;
  /// Concrete Type of ParticleDescendants tool
  std::string m_descendantsName;

  /// Find the related PV
  mutable IRelatedPVFinder* m_pvRelator;

  /// Reference to ParticlePropertySvc
  mutable const LHCb::IParticlePropertySvc* m_ppSvc;

private:

  /// Decay description (Property)
  std::string m_decayDescriptor;

  /// avoid the writeup of empty containers
  bool m_forceOutput;

  /// Has setFilterPassed() already been called in current event?
  bool m_setFilterCalled;

  /// Re-fit PVs. Default: false.
  bool m_refitPVs;

  /// Do we have more than one PV in the event?
  bool m_multiPV;

  /// Do we want to use the Particle -> PV relations? Default: true.
  bool m_useP2PV;

  /// Do we want to write the Particle -> PV relations table to the TES?
  /// Default: true
  bool m_writeP2PV;

  /** Force building of Particle-> best PV relations for selected
   *  particles and their decay products where applicable. Only has effect
   *  if <b>WriteP2PVRelations</b> is true. This is a post-execute action
   *  and has no effect over the user's execute actions. <b>Default</b> false.
   *  Takes existing relations and adds missing ones. */
  bool m_forceP2PVBuild;

  /** Ignore Particle->PV relations from Inputs?
   *  User-defined ones are kept. */
  bool m_ignoreP2PVFromInputLocations;

  /** Switch PreloadTools to false to not preload any tools.
   *  This will have the effect that they will be loaded on demand, when needed,
   *  at any event. This option is thus only recommended for use of DV Algorithm
   *  to do something unrelated to physics analysis.
   */
  bool m_preloadTools;

  /// InputLocations
  std::vector<std::string> m_inputLocations ;

  /// User-defined Particle->PV relations locations
  std::vector<std::string> m_p2PVInputLocations ;

  /// TES location of input PVs.
  std::string m_PVLocation;

  /// Don't use PVs
  bool m_noPVs;

  /// Functor determining if LHCb::Particle is in TES.
  DaVinci::Utils::ParticleInTES m_inTES;

private: // classes

  /** @class DVAlgGuard
   *
   * Guard class to clear containers and Particle->PV relations table.
   */
  class DVAlgGuard
  {

  public:

    /// Constructor with data values
    DVAlgGuard( LHCb::Particle::ConstVector& inputParticles,
                LHCb::Particle::ConstVector& localParticles,
                LHCb::Vertex::ConstVector& localSecondaryVertices,
                LHCb::RecVertex::ConstVector& primaryVertices,
                P2PVMap& tableP2PV )
      : m_guardInputP  ( inputParticles         ),
        m_guardLocalP  ( localParticles         ),
        m_guardLocalSV ( localSecondaryVertices ),
        m_guardPV      ( primaryVertices        ),
        m_table        ( tableP2PV              )
    { }

    /// Destructor
    ~DVAlgGuard() { m_table.clear(); }

  private:

    /// Disallow default constructor
    DVAlgGuard();

  private:

    DaVinci::Utils::OrphanPointerContainerGuard<LHCb::Particle::ConstVector>  m_guardInputP;
    DaVinci::Utils::OrphanPointerContainerGuard<LHCb::Particle::ConstVector>  m_guardLocalP;
    DaVinci::Utils::OrphanPointerContainerGuard<LHCb::Vertex::ConstVector>    m_guardLocalSV;
    DaVinci::Utils::OrphanPointerContainerGuard<LHCb::RecVertex::ConstVector> m_guardPV;
    P2PVMap& m_table;

  };

private:

  /// Default nickname mapping for VertexFitters
  const ToolMap & defaultVertexFittersNames() const;

  /// Default nickname mapping for ParticleFilters
  const ToolMap & defaultParticleFiltersNames() const;

  /// Default nickname mapping for ParticleCombiners
  const ToolMap & defaultParticleCombinersNames() const;

  /// Default nickname mapping for ParticleReFitters
  const ToolMap & defaultParticleReFittersNames() const;

  /// Default nickname mapping for PVReFitters
  const ToolMap & defaultPVReFittersNames() const;

  /// Default nickname mapping for DecayTreeFitters
  const ToolMap & defaultDecayTreeFittersNames() const;

  /// Default nickname mapping for MassFitters
  const ToolMap & defaultMassFittersNames() const;

  /// Default nickname mapping for LifetimeFitters
  const ToolMap & defaultLifetimeFittersNames() const;

  /// Default nickname mapping for DirectionFitters
  const ToolMap & defaultDirectionFittersNames() const;

  /// Default nickname mapping for DistanceCalculators
  const ToolMap & defaultDistanceCalculatorsNames() const;

};

#endif // KERNEL_DVCOMMONBASE_H
