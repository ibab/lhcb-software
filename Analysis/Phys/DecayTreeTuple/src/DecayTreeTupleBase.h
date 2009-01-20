// $Id: DecayTreeTupleBase.h,v 1.4 2009-01-20 18:31:30 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLEBASE_H
#define JBOREL_DECAYTREETUPLEBASE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "OnePart.h"
#include "TupleToolDecay.h"

class IDecayFinder;

class IMCParticleTupleTool;
class IParticleTupleTool;
class IEventTupleTool;

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


 protected:
  //! Retrieve from the desktop all the top level particles that match
  //! the decay descriptor
  bool getDecayMatches( const LHCb::Particle::ConstVector& src
			, LHCb::Particle::ConstVector& target );

  std::vector<std::string> getEventTools() const;

  bool initializeDecays();

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

  IDecayFinder* m_dkFinder;
  std::vector<std::string> getParticleTools(const std::vector< IParticleTupleTool* > pTools) const;

//=============================================================================
// The templates need to go here
//=============================================================================
  /// Initialize all Particle tools
  template<class ITUPLETOOL>
  void initializeStufferTools(std::vector< ITUPLETOOL* >& pTools){
    std::sort( m_toolList.begin(), m_toolList.end() );
    std::unique( m_toolList.begin(), m_toolList.end() );
    
    // base instantiation:
    std::vector<std::string>::const_iterator it = m_toolList.begin();
    for( ; m_toolList.end()!=it ; ++it ){
      IAlgTool* tt = tool<IAlgTool>( *it, this );
      if( !tt ){
        Error("Can't get the tool '" + *it + "', check you're syntax" );
        continue;
      }
      // splitting in the correct list.   
      SmartIF<IEventTupleTool> test1(tt);
      if( test1 ) m_eTools.push_back( test1 );
      SmartIF<ITUPLETOOL> test2(tt);
      if( test2 ){
        pTools.push_back( test2 );
        
        // inherit by default: give all the tools to the particles:
        std::vector<Decays::OnePart*>::iterator op;
        for( op=m_parts.begin(); op!=m_parts.end(); ++op ){
          (*op)->addTool( test2 );
        }
      }
      
      if( !test1 && !test2 ){
        Error("Can't get the tool '" + *it + "', check your syntax" );
      }
      if( test1 && test2 ){
        Warning("The tool '" + *it +
                "', will be called both by the IParticleTupleTool" +
                " and IEventTupleTool interfaces. That's fine as long as you" +
                " know what you are doing." );
      }
      if( test1 && !test2 )
        if (msgLevel(MSG::VERBOSE)) verbose() << *it << " instantiated as an Event related tool" << endreq;
      if( !test1 && test2 )
        if (msgLevel(MSG::VERBOSE)) verbose() << *it << " instantiated as a Particle related tool" << endreq;
    }
    
    if (msgLevel(MSG::DEBUG)) debug() << "Generic and inherited tool list successfully created" << endreq;
  }
  //=============================================================================
  //=============================================================================
  template<class ITUPLETOOL> void
  initializeOnePartsStufferTools( Decays::OnePart* P
                                  , const TupleToolDecay* m
                                  , std::vector< ITUPLETOOL* >& pTools   ){
    
    // there is a specific descriptor for P, i.e. default settings are wrong
    P->clearTools();
    
    // tool list must become : specific + (inherited-specific)
    std::vector<std::string> remainTools;
    std::vector<std::string> globalTools = getParticleTools(pTools);
    const std::vector<std::string>& locTools = m->getStuffers();
    
    std::insert_iterator<std::vector<std::string> > ii( remainTools
                                                        , remainTools.begin() );
    std::set_difference( globalTools.begin(), globalTools.end()
                         , locTools.begin(), locTools.end()
                         , ii );
    
    if (msgLevel(MSG::VERBOSE)) verbose() << "Remains " << remainTools.size() << " to inherit" << std::endl;
    
    // inherit again the remaining global tools:
    std::vector<std::string>::const_iterator it = remainTools.begin();
    if( m->inheritTools() ){
      for( ; remainTools.end() != it ; ++it ){
        // find the right tool:
      bool flag = false;
      for( unsigned int k=0; k< pTools.size(); ++k ){
        if( *it == pTools[k]->type() ){
          if (msgLevel(MSG::VERBOSE)) verbose() << "Parts " << P->info() << " inherits "
                                                << pTools[k]->type() << endreq;
          P->addTool( pTools[k] );
          flag = true;
          break;
        }
      }
      if( !flag )
        Warning( "Hmm, should not happen, the tool '" +
                 *it + "' will be ignored for some reason..." );
      }
    }
    
    // now instanciate the specific tools from the TupleToolDecay:
    for( it=locTools.begin(); locTools.end()!=it; ++it ){
      ITUPLETOOL* tt = tool<ITUPLETOOL>( *it, m );
      if( !tt ){
        Error("Can't instanciate the tool '" + *it + "', check your syntax");
        continue;
      }
      P->addTool( tt );
    }
  }
  
};
#endif // JBOREL_DECAYTREETUPLEBASE_H
