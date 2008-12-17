// $Id: DecayTreeTuple.h,v 1.3 2008-12-17 20:14:55 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLE_H
#define JBOREL_DECAYTREETUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

class IDecayFinder;
class TupleToolDecay;
class DecayTreeTuple;

class IParticleTupleTool;
class IEventTupleTool;

/** @class DecayTreeTuple DecayTreeTuple.h jborel/DecayTreeTuple.h
 *
 * \brief This is the new version of the so-called DecayChainNTuple
 *
 * \section basics (Basic usage:)
 * 
 * DecayTreeTuple has a decay descriptor (given by the Decay
 * property). One line of the Tuple if filled for each reconstructed
 * candidate. This algorithm is NOT backward compatible with
 * DecayChainNTuple. Yet the base syntax is quite close.
 *
 * Here is a minimalist but working example:
 * \verbatim
 from Configurables import DecayTreeTuple, PhysDesktop, GaudiSequencer
 MyDecayTreeTuple = DecayTreeTuple("MyDecayTreeTuple")
 GaudiSequencer("MySeq").Members =+ [ MyDecayTreeTuple]
 MyDecayTreeTuple.PhysDesktop.InputLocations = ["Phys/SomeSelection"]
 MyDecayTreeTuple.Decay = "B0 -> ^pi- ^pi+"
 MyDecayTreeTuple.TupleName = "MyTuple"\endverbatim
 *
 * \note Any particle to be stored in the Tuple has to be flagged with
 * '^' (an exception here is the decay head which cannot be flagged as
 * DecayFinder will refuse it. Top level particle are therefore always
 * stored).
 * 
 * \section principle How it works:
 *
 * DecayTreeTuple basically owns a set of tools which all implement
 * either the IEventTupleTool or the IParticleTupleTool
 * interfaces. For every reconstructed candidates, it successively
 * call all the tools:
 *
 * - Once for each matched particles for the IParticleTupleTool instances
 *
 * - Once for the candidate for the IEventTupleTool instances
 *
 * The instantiated tool list is defined by the \b ToolList property,
 * a vector of strings. Hence any tool can be added and manipulated as
 * usual, doing things like
 *
 * \verbatim
 MyDecayTreeTuple.ToolList = [ "TupleToolMCTruth", TupleToolTrigger" ]
 MyDecayTreeTuple.ToolList += [ "TupleToolPid/genericPID" ]
 from Configurables import TupleTooPid
 MyDecayTreeTuple.addTool(TupleTooPid("genericPID"))
 MyDecayTreeTuple.genericPID.SomeProperty = True\endverbatim
 *
 * \subsection branches Fine tuning of the branches:
 *
 * The \b Branches property, is a map<string,string>. Each of its
 * entry associates a name with decay descriptor matching a specific
 * part of the main decay.
 *
 * Once an entry exists, specific tools for this entry can be
 * added. They will only act on the particles matched by the
 * associated decay AND the main decay. Specific tools have their own
 * properties, whether or not the tool already exists in the generic
 * tool list.
 *
 * Specific tools are instance of TupleToolDecay, they have a few properties:
 *
 * - ToolList, a vector of string, defining the tool list obviously.
 *
 * - InheritTools, whether or not generic tools should also be applied
 *   to this sub-decay.
 *
 * Here is an example with Bs->Ds h, h as K or pi, Ds => K K pi (a
 * good occasion to remind you the DecayFinder syntax):
 *
 * \verbatim
 Dtt = DecayTreeTuple("Dtt")
 GaudiSequencer("MySeq").Members = [ Dtt ]
 Dtt.Decay = "[[B_s0]cc -> (^D_s- => ^K+ ^K- ^pi-) {^pi+, ^K+}]cc"
 Dtt.Branches = {
    "bachelor" : " [[B_s0]cc -> D_s- {^pi+, ^K+}]cc "
    ,"Ds" : " [ [B_s0]cc -> ^D_s- {pi+,K+} ]cc "
    ,"head" : " [B_s0]cc : [[B_s0]cc -> (D_s- => K+ K- pi- ) pi+ ]cc"
 }

 Dtt.head.ToolList = [ "TupleToolPropertime" ]
 Dtt.head.InheritTools = False
 from Configurables import TupleToolMCTruth, TupleToolDecay
 Dtt.addTool(TupleToolDecay("bachelor"))
 Dtt.bachelor.addTool(TupleToolMCTruth())
 Dtt.bachelor.ToolList = [ "TupleToolMCTruth" ]

 Dtt.bachelor.TupleToolMCTruch.UseChi2Method = True\endverbatim
 *
 * \note Notice the way the decay head is matched as it is a bit unusual.
 *
 * \subsection names Naming the tuple column:
 * 
 * The following rules applies to name the tuple columns:
 *<ol>
 * 
 * <li>By default, the name of the particles from the first matched
 * candidate are taken to prefix the column names (after having
 * sanitized some ugly chars).
 *
 * <li>If a specific decay is given for some particles, the first
 * argument of the \b Branches property is taken as prefix. This is not
 * true anymore if \b UseToolNameForBranchName is set to false.
 *
 * <li>For the nostalgics of DecayChainNTuple, the \b UseLabXSyntax
 * allows to prefix the branches with the good old \em labX style. Yet it
 * will be prefixed, not post fixed.
 *
 * </ol>
 *
 * \sa TupleTollDecay IEventTupleTool IParticleTupleTool
 *
 * \author Jeremie Borel with the help of Patrick and lhcb-davinci@cern.ch
 * \date 2007-11-01 
 */
class DecayTreeTuple : public DVAlgorithm  {
  // forward declaration
  class OnePart;

  // ==================================================

  class OnePart {
  public:
    OnePart( DecayTreeTuple* parent
	     , const LHCb::Particle& me 
	     , const std::string& head );
    ~OnePart();
    
    //! Call successively all the tool's fill method
    bool fill( Tuples::Tuple&, const LHCb::Particle* mother
	       , const LHCb::Particle* );

    //! Refers to the tuple column name prefix
    std::string headName();
    //! Refers to the tuple column name prefix
    void headName( const std::string& h );
    
    //! Refers to the particle's real syntax (not escaped)
    std::string getRealName();
    //! Refers to the particle's real syntax (not escaped)
    const std::string& getRealName() const;

    //! Prints the tree strucutre, if verbose, also prints the tool list
    void printStructure( std::ostream& os, bool verbose=false ) const ;
    int depth() const;
    void setMother( const OnePart* );
    DecayTreeTuple* parent();
    const OnePart* getMother() const;
    void addDaughter( OnePart* );

    std::string info() const;

    void addTool( IParticleTupleTool* tool );

    std::vector< IParticleTupleTool* >& tools();
    std::vector<std::string> toolList() const;
    void clearTools();

  private:
    std::string m_head, m_realname;

    OnePart();
    OnePart( const OnePart& );

    DecayTreeTuple* m_parent;

    const OnePart* m_mother;
    std::vector<const OnePart*> m_daughters;
    std::vector< IParticleTupleTool* > m_tools;
  };

 public:
  /// Standard constructor
  DecayTreeTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecayTreeTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  //! Escape a string, removing all the char that cannot be used for a tuple column name
  static std::string escape( const std::string& );

  //! Retrun a string with all the element joined by sep. If empty, returns ifemtpy
  template <class ForwardIterator>
  static std::string join( ForwardIterator first
			   , ForwardIterator last
			   , const char* sep=", "
			   , const char* ifempty="none" );


 protected:
  //! Trigger all the fill procedures
  StatusCode fillTuple( Tuples::Tuple&, const LHCb::Particle::ConstVector& );
  //! Call the fill methode of all the particles
  StatusCode fillParticles( Tuples::Tuple&, const LHCb::Particle::ConstVector& );
  //! Call the fill methode which does not take a particle as argument
  StatusCode fillEventRelatedVariables( Tuples::Tuple& );


 private:
  //! Retrieve from the desktop all the top level particles that match
  //! the decay descriptor
  bool getDecayMatches( const LHCb::Particle::ConstVector& src
			, LHCb::Particle::ConstVector& target );

  //! Check if ready to fill or trigger the initialization
  bool sizeCheckOrInit( const LHCb::Particle::ConstVector& );
  void matchSubDecays( const LHCb::Particle::ConstVector& );

  std::vector<std::string> getEventTools() const;
  std::vector<std::string> getParticleTools() const;

  bool initializeDecays();
  void initializeStufferTools();
  void initializeOnePartsStufferTools( OnePart*, const TupleToolDecay* );

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

  std::vector<OnePart*> m_parts;

  std::vector< IParticleTupleTool* > m_pTools;
  std::vector< IEventTupleTool* > m_eTools;

  IDecayFinder* m_dkFinder;

  unsigned int m_nSuccessEvent, m_nSuccessCandidate, m_nFailedEvent, m_nFailedCandidate;

};

// ===============================================================
// ======================= inline & template body ================
// ===============================================================

/** join a container with a separation char. */
template <class ForwardIterator>
std::string DecayTreeTuple::join( ForwardIterator first
				  , ForwardIterator last, 
				  const char* sep
				  , const char* ifempty ){
  std::stringstream  ret;
  ForwardIterator it(first), it2( first );

  if( first == last ) return std::string(ifempty);
  ++it2;
  if( it2 == last ){
    ret << *first;
    return ret.str();
  }

  it2=first;

  while( it2!=last ){
    ret << *it << sep;
    ++it;
    it2=it;
    ++it2;
  }
  ret << *it;
  return ret.str();
};


#endif // JBOREL_DECAYTREETUPLE_H
