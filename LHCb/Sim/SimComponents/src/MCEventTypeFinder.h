// $Id: MCEventTypeFinder.h,v 1.4 2009-06-11 12:43:12 rlambert Exp $
#ifndef TOOLS_MCEVENTTYPEFINDER_H 
#define TOOLS_MCEVENTTYPEFINDER_H 1

// Include files
// from STL
#include <string>
#include <set>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "MCInterfaces/IMCEventTypeFinder.h"
#include "Event/MCParticle.h"

class IMCDecayFinder;
class IEvtTypeSvc;


/** @class MCEventTypeFinder MCEventTypeFinder.h
 *
 * \brief 
 * 
 *  @author Rob Lambert
 *  @date   20/02/2009
 * 
 *  The MCEventTypeFinder is similar to the MCDecayFinder.
 *  MCEventTypeFinder uses the definition of event types in the dec files,
 *  and the definition in the LHCb note to classify a given event, MCparticle
 *  or group of MCParticles by their eventType.
 *  The EvtTypeSvc should be told the location of the dec files txt file
 *  e.g. if you have your own local copy, in Gaudi Python, do 

 *  from Configurables import EvtTypeSvc

 *  EvtTypeSvc().EvtTypesFile='~/cmtuser/Gauss_HEAD/Gen/DecFiles/doc/table_event.txt'

 *  
 *  MCEventTypeFinder is an implimentation of IMCEventTypeFinder.
 *  
 *  This is used in the TupleToolMCEventType, and TupleToolMCDecayType, where you should look for examples on how to use this class.
 * 
 *  See LHCbNotes 2009-001 and 2005-034  
 *
 *  METHODS:
 *
 *  There are three ways to get the event type of an event. The genheader, an educated guess, and the decay string (instant, fast, very slow).
 * 
 *  1) From the GenHeader    : see the example in CheckMCEventTool and in TupleToolMCEventType, and TupleToolGeneration
 *                             One type will be in the GenHeader, which will tell you next to nothing about
 *                             other decays in the sample, and not allow you to remove doubly-counted events.
 * 
 *  2) From an educated guess: using the event types definitions, from the LHCb note, all b/c decays in any event can be categorised.
 *                             An event type is "constructed", using the methods constructEventTypes()
 *                             This is quite fast, compared to searching using MCDecayFinder, and will categorise all b/c decays.
 *                             but the result is ambiguous.
 * 
 *  3) Decay string          : In each dec file a decay descriptor is given.
 *                             By searching the decay or event for all decay strings, the event type can be categorised.
 *                             An event type is "found" using the methods findEventTypes()
 *                             The result here is not ambiguous, but this takes a lot of time, and relies on all 
 *                             decay strings being correct in the dec files, which is not true.
 *                             
 *  MULTIPLE TYPES:
 *
 *  any event can, and probably will, have mutiple types. This is because of three things:
 *  
 *  A) Multiple decaying heavy mesons : Different decays will be of different types.
 *  
 *  B) Overlaps in decay desriptors   : perhaps the same decay is allowed in multiple event types, because they are ambiguous.
 *  
 *  C) Parent event types             : each event will be a subset of another event type. Bs->mumu is a subset of Bs->anything
 *                                      which is a subset of inclusive B, and inclusive B di-muon, which are subsets of
 *                                      minimum bias and minimum bias di-muon.
 * 
 *  The EventTypeFinder will return the full set of all possible types.
 * 
 *  OPTIONS:
 *  
 *  Types (std::vector) override the total list of types taken from the decfiles to a smaller sub-class of types
 *                      by default this is set to the total list of types.
 *                      Setting a smaller list can reduce the processing time.
 *                      This has no effect on the constructEventTypes methods, only on the find methods.
 *
 *  acc_min  double     define the minimum acceptance, in pseudorapisity, for a decprodcut
 *                      by default this is -ln tan (0.4 /2), i.e. 400 mrad, eta > 1.60-ish
 *                      you could set this to 2.0 if you really want "in the acceptance" 
 *  acc_max  double     define the maximum acceptance, in pseudorapisity, for a decprodcut
 *                      by default this is -ln tan (0.005 /2), i.e. 5mrad eta<6-ish
 *                      you could set this to 4.9 if you really want "in the acceptance" 
 *
 */
class MCEventTypeFinder : public GaudiTool, virtual public IMCEventTypeFinder
{
public:
  /// Standard Constructor
  MCEventTypeFinder( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  /// Destructor 
  virtual ~MCEventTypeFinder( ); ///< Destructor

  StatusCode initialize( void );

  /// Set up the event types, in case you want to change them with some other code.
  virtual StatusCode setEventTypes(const LHCb::EventTypeSet events);
  
  /// SLOW. Find all event types in this event using the decay descriptors, should work so only accepted particles are counted for DecProdCuts
  virtual StatusCode findEventTypes(LHCb::EventTypeSet& found);
  
  /// SLOW. Find all decay types from this particle using the decay descriptors, should work so only accepted particles are counted for DecProdCuts
  virtual StatusCode findDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle * mc_mother );

  /// SLOW. Find all decay types from this particle using the decay descriptors, should work so only accepted particles are counted for DecProdCuts
  virtual StatusCode findDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle::ConstVector & mc_mothers );

  /// SLOW. Find all decay types from this particle using the decay descriptors, should work so only accepted particles are counted for DecProdCuts
  virtual StatusCode findDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticles & mc_mothers );

  /// Guess event types in this event based on the quarks/decays in the event, only accepted particles are counted
  virtual StatusCode constructEventTypes(LHCb::EventTypeSet& found);
  
  /// Guess decay types from this particle, no acceptance cut although the type is appended with a 1 if all daughter particles are in the acceptance
  virtual StatusCode constructDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle * mc_mother );
  
  /// Guess decay types from this particle based on its decay, only accepted particles are counted
  virtual StatusCode constructDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle::ConstVector & mc_mothers );
  
    /// Guess decay types from this particle based on its decay
  virtual StatusCode constructDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticles & mc_mothers );
  
  /// Given an input code, will return the list of event types this belongs too.
  /// e.g. all events are covered in MinimumBias, any bb-event is covered by inc-b.
  /// so an input of 11154101 ->  11154100    ->   11000000    -> 10000000 -> 30000000
  ///        [specific decay] -> [decay type] -> [hadron mode] -> [inclusive] -> [minbias]
  virtual StatusCode parentEventTypes(long unsigned int evtType, LHCb::EventTypeSet& parents);

 private:

    ///using the std::set to append parents
    StatusCode appendParents(LHCb::EventTypeSet& aset);
    ///convert a std::set to a std::vector
    bool set2vec(LHCb::EventTypeSet& aset,std::vector<long unsigned int>& avec);
    ///convert a std::set to a std::vector
    bool vec2set(std::vector<long unsigned int>& avec,LHCb::EventTypeSet& aset);
 
    IEvtTypeSvc * m_evtTypeSvc;        ///< pointer to the event type service
    std::vector<IMCDecayFinder*> m_mcFinders;        ///< Vector of pointers to tools :)
    StatusCode fillMCTools(void);
    std::vector<long unsigned int> m_inputTypes; ///<Set in the options of this tool, "Types"
    LHCb::EventTypeSet m_allTypes; ///<what types are being searched for?
    std::vector<bool> m_decProdCut;///<"Is this decay a DecProdCut??"
    
    long unsigned int m_mbias;///=30000000, modified in fillMCTools, if required
    const long unsigned int m_incb;///=10000000
    const long unsigned int m_incc;///=20000000
    const long unsigned int m_diMuon;///=12000
    const int m_muonp;///=-13
    const int m_muonm;///=13
    const int m_tau;///=15
    double m_acc_min;///n=1.6-ish, 400 mrad, set using option acc_min
    double m_acc_max;///n=6-ish, 5 mrad, set using option acc_max
    const std::string m_decProdStr1; ///<"DecProdCut"
    const std::string m_decProdStr2; ///<"InAcc"
    
    static bool m_temp;
 
 
    ///method to categorise hadron decays type, gs______
    long int categoriseHadron(const LHCb::MCParticle * mc_mother, bool & dimuon=m_temp);
    
    ///method to categorise tau decays type, gsd_____
    long int categoriseTau(const LHCb::MCParticle * mc_mother, bool & dimuon=m_temp);
    
  
    ///method to create the decay type, gs______
    long int constructDecayType(const LHCb::MCParticle * mc_mother, bool & dimuon=m_temp);
    
    ///method to create the decay type, __dctnxu
    long int determineDecay(const LHCb::MCParticle * mc_mother, bool & dimuon=m_temp);
    
    ///method to create the decay type, __dctnxu
    long int determineTauDecay(const LHCb::MCParticle * mc_mother, bool & dimuon=m_temp);
    
    ///iterative method looking for all relevent features of the decay, used to create the decay type, __dctnxu
    int determineDecay(const LHCb::MCParticle * mc_mother, bool & nJPsi, int & ncharm,
                       bool & nmuon, bool & pmuon, bool & nmudec, bool & pmudec, 
                       bool & nelectron, int & ntracks, int & neut, bool & dec, bool & neutrino);
    
    ///iterative method looking for all relevent features of Tau decays, used to create the decay type, ___ctnxu
    int determineTauDecay(const LHCb::MCParticle * mc_mother, int & nV0,
                       bool & nmuon, bool & pmuon, bool & nmudec, bool & pmudec, 
                       bool & nelectron, int & ntracks, int & neut, bool & dec,
                       int & nPip, int & nPim, int & nKp, int & nKm);
    
    ///Annoying way to check if the particle is stable :S
    bool isStable(const LHCb::MCParticle * mc_mother);
    
    ///are there two muons in this event?
    bool dimuon(const LHCb::MCParticle::ConstVector & mc_mothers);
    ///are there two muons in this event? overloaded
    bool dimuon(const LHCb::MCParticles & mc_mothers);
    
    ///append the dimuon types to the set found
    bool appendDiMuon(LHCb::EventTypeSet & found );

    ///are two strings the same, once all whitespace is removed
    bool strcompNoSpace(std::string first, std::string second);
    
};
bool MCEventTypeFinder::m_temp=false;
#endif // TOOLS_MCEVENTTYPEFINDER_H
