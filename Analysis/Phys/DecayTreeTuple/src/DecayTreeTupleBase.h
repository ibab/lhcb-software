// $Id: DecayTreeTupleBase.h,v 1.6 2009-01-21 11:22:41 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLEBASE_H
#define JBOREL_DECAYTREETUPLEBASE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "OnePart.h"
#include "TupleToolDecay.h"
#include "Kernel/IEventTupleTool.h"

/** @class DecayTreeTupleBase DecayTreeTupleBase.h jborel/DecayTreeTupleBase.h
 *
 * Base class for algorithms providing a DecayTreeTuple
 *
 * \sa DecayTreeTuple, MCDecayTreeTuple
 *
 * \author Patrick Koppenburg based on Jérémie Borel's DecayTreeTuple
 * \date 2009-01-20
 */
class DecayTreeTupleBase : public DVAlgorithm  {

 public:
  /// Standard constructor
  DecayTreeTupleBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecayTreeTupleBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
 protected:
  //! Call the fill methode which does not take a particle as argument
  StatusCode fillEventRelatedVariables( Tuples::Tuple& );

  //! Retrieve from the desktop all the top level particles that match 
  //! the decay descriptor
  bool getDecayMatches( const LHCb::Particle::ConstVector& src, LHCb::Particle::ConstVector& target );
  //! Retrieve from the desktop all the top level particles that match 
  //! the decay descriptor
  bool getDecayMatches( const LHCb::MCParticle::ConstVector& src, LHCb::MCParticle::ConstVector& target );
  //! Check if ready to fill or trigger the initialization
  void matchSubDecays( const LHCb::Particle::ConstVector& );
  /// Call successively all OnePart's fill method 
  bool fillOnePart( Decays::OnePart*, Tuples::Tuple&, const LHCb::Particle* mother, const LHCb::Particle* );
  /// Initialize all Particle tools
  template<class ITUPLETOOL>
  void initializeStufferTools(std::vector< ITUPLETOOL* >& pTools);
  template<class ITUPLETOOL> void
  initializeOnePartsStufferTools( Decays::OnePart* P
                                  , const TupleToolDecay* m
                                  , std::vector< ITUPLETOOL* >& pTools   ); 
  /// check for unicity of names
  bool checkUnicity() const ;  
  /// print infos
  void printInfos() const ;
  
 protected:
  std::vector<std::string> getEventTools() const;

  /// Initialize the main decay
  bool initializeDecays(bool isMC);

  std::string getBranchName( const LHCb::Particle* p );

  int getOffset( const LHCb::Particle* p
		 , const LHCb::Particle::ConstVector& v
		 , bool secure=true );

  std::map< std::string, std::string > m_decayMap;
  std::vector<TupleToolDecay*> m_decays;
  std::string m_headDecay;

  std::string m_tupleName; //, m_eventColName;
  bool m_useLabName;
  bool m_tupleNameAsToolName;

  std::vector<std::string> m_toolList;

  std::vector<Decays::OnePart*> m_parts;

  std::vector< IMCParticleTupleTool* > m_mTools; // TMP
  std::vector< IEventTupleTool* > m_eTools;

  IDecayFinder* m_dkFinder; ///< Decay finder
  IMCDecayFinder* m_mcdkFinder; ///< MC truth decay finder
  std::vector<std::string> getParticleTools(const std::vector< IParticleTupleTool* > pTools) const;
  std::vector< IParticleTupleTool* > m_pTools;

  //=============================================================================
  // Templated methods accessed from other classes need to be in the header
  // to make sure the linker builds them.
  //=============================================================================
  /// Recursively fill (MC)Particles. TYPE is LHCb::(MC)Particle::ConstVector
  template<class TYPE>
  StatusCode fillParticles( Tuples::Tuple& tuple,
                            const TYPE& row ){
    if ( sizeCheckOrInit( row ) ){
      bool test = true;
      const int size = m_parts.size();
      for( int k=0; size>k; ++k ){ // row[0] is the deday head.
        test &= fillOnePart( m_parts[k], tuple, row[0], row[k] );
      }
      return StatusCode(test);
    }
    return StatusCode::FAILURE;
  }
  //=============================================================================
  /// Initialize Tuple, or check all is consistent TYPE is LHCb::(MC)Particle
  template<class TYPE> 
  bool sizeCheckOrInit( const std::vector<const TYPE*> & row ){
    const unsigned int size = row.size();
    if( m_parts.size() == size ) return true;
    
    if( !m_parts.empty() ){
      Error( "The number of matched particles with the DecayFinder ("
             + m_headDecay
             + ") has changed. Skipping the candidate.");
      return false;
    }
    
    info() << "Entering the initialization process" << endreq;
    
    // initializing the particles object.
    m_parts.reserve( size );
    for( unsigned int i=0; i<size; ++i ){
      Decays::OnePart *p = new Decays::OnePart( ppSvc()->find ( row[i]->particleID() )->particle(), getBranchName(row[i]) );
      // inherit the default properties:
      m_parts.push_back( p );
    }
    
    if (msgLevel(MSG::DEBUG)) debug() << "There is " << m_parts.size()
                                      << " particle to initialize." << endreq;
    
    // set the base properties...
    initializeStufferTools(m_pTools);
    // set the branch names and inherit the particle specific tools
    matchSubDecays( row );
    
    // re-creating mother->daughter relationship,
    // allows better printout later on
    for( int i=0; i<(int)row.size(); ++i ){
      Decays::OnePart* Mother = m_parts[i];
      
      std::vector<const TYPE*> dau = row[i]->daughtersVector();
      typename std::vector<const TYPE*>::const_iterator dauit, f ;
      for( dauit=dau.begin(); dau.end()!=dauit; ++dauit ){
        // am I in the search decay ?
        f = std::find( row.begin(), row.end(), *dauit );
        if( f==row.end() ) continue;
        int off = getOffset( *f, row );
        Mother->addDaughter( m_parts[off] );
        m_parts[off]->setMother( Mother );
      }
    }
    
    if (!checkUnicity()) return false ;
    printInfos();
    return true ;
  }
};


#endif // JBOREL_DECAYTREETUPLEBASE_H
