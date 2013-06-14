// $Id: ITriggerTisTos.h,v 1.4 2010-07-22 14:58:26 tskwarni Exp $
#ifndef ITRIGGERTISTOS_H
#define ITRIGGERTISTOS_H 1

// Include files
// from STL
#include <string>
#include <vector>

#include "Kernel/ITriggerSelectionTisTos.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/LHCbID.h"

//forward declarations
namespace LHCb
{
  class Particle;
  class ProtoParticle;
  class Track;
  class RecVertex;
  class HltObjectSummary;
}

/** @class ITriggerTisTos ITriggerTisTos.h
 *
 *
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-20
 *
 * @sa ITriggerSelectionTisTos for additional member functions and documentation.
 *
 * @par Tool purpose
 *   Primary function of this tool is to determine TIS and TOS categories (see below for definition)
 *   for user specified "Trigger Input" with respect to user specified "Offline Input".
 * @par
 *   This tool extends functionality of @c ITriggerSelectionTisTos (from which it inherits). While @c ITriggerSelectionTisTos
 *   member functions accept for trigger input only specific Trigger Selection names (which cannot be checked for validity),
 *   @c ITriggerTisTos tool knows valid Trigger Selection names and their hierarchy. Therefore, trigger input can be specified
 *   via trigger name pattern which contains wild characters. This allows user to define trigger structures independently
 *   of the particular trigger configuration.
 *   Auxiliary output functions allow discovering valid Trigger Selection names,
 *   as well as selections matching decision/TIS/TOS requirements.
 *   @c triggerTisTos output functions belong to @c ITriggerTisTos and allow generic Trigger Input,
 *   while @c selectionTisTos output functions inherited from @c ITriggerSelectionTisTos
 *   and require specific Trigger Selection names.
 * @par
 *   User can also obtain a list of LHCbIDs of hits derived from the Offline Input used by the tool to perform
 *   matching between the offline and trigger content, and best TOS matches of trigger objects (tracks, vertices or
 *   particles dependening on the type of selection) for specific Trigger Selection.
 *   These output functions are inherited from @c ITriggerSelectionTisTos.
 * @par
 *   The interface was designed with hit based matching in mind, but the tool could be implemented without it,
 *   in which case LHCbID input or output functions could do nothing.
 *
 * @par Offline Input  (Derived from @c ITriggerSelectionTisTos).
 *   Particle (with or without daughters), ProtoParticle, Track, a vector of LHCbIDs, or combination of these can
 *   be passed as Offline Input. Passing ProtoParticle is not the same as passing the track the ProtoParticle may be built on,
 *   since the tool can take input from PID info (e.g. to grab Muon hits for matching).
 * @par
 *   @c setOfflineInput() called without parameters initializes new Offline Input.
 *   Then overloaded @c addToOfflineInput( <input> ) should be called once or many times.
 * @par
 *   @c setOfflineInput( <input> ) is a shortcut to the sequence of the above.
 * @par
 *   Once the Offline Input is defined, many calls to the output functions can be made without re-entering the Offline Input.
 * @par
 *   The Offline Input can also be passed in functions which retrieve outputs, but this becomes inefficient
 *   if many output function calls are made passing the same Offline Input (as the Offline Input is
 *   pre-processed and stored in an internal representation).
 *
 * @par Trigger Input (members of @c ITriggerTisTos)
 *   Trigger Input is defined by Trigger Selection Name pattern passed as @c std::string (or std::vector<std::string>)
 *   which may contain multiple wild characters.
 *   Examples:  "Hlt1.*Decision", "Hlt1.*Mu.*Decision", "Hlt1MuonSingleTUVelo", "Hlt2SelB.*Decision",
 * @par
 *   If Trigger Input corresponds to many trigger selection names, the TisTosing output corresponds
 *   to an OR between these selections.
 * @par
 *   @c setTriggerInput() called without parameters initializes new Trigger Input.
 *   Then @c addToTriggerInput( <input> ) should be called once or many times.
 * @par
 *   @c setTriggerInput( <input> ) is a shortcut to the sequence of the above.
 * @par
 *   Once the Trigger Input is defined, many calls to the output functions can be made without
 *   re-entering the Trigger Input.
 * @par
 *   The Trigger Input can also be passed in functions which retrieve outputs, but this becomes inefficient
 *   if many output function calls are made passing the same Trigger Input (as the Trigger Input is
 *   pre-processed and stored in an internal representation).
 * @par
 *   The tool will print a warning message if the passed Trigger Input pattern does not correspond to any trigger selection.
 * @par
 *   User can obtain names of trigger selections corresponding to the Trigger Input by calling @c triggerSelectionNames()
 *   with no arguments (input selection tree can be explored by calling @c inputTriggerSelectionNames(selectionName) ).
 *   @c triggerSelectionNames takes also optional input parameters for (decision,TIS,TOS) pattern to be matched
 *   (0=false, 1=true, 2=anything).
 *   Thus e.g. @c triggerSelectionNames(ITrackTisTos::kAnything,ITrackTisTos:kAnything,ITrackTisTos::kTrueRequired)
 *   returns all TOS selections matching the Trigger Input definition
 * @par
 *   The tool can also return HltObjectSummaries for trigger objects (Tracks, RecVertices, Particles etc.) responsible for
 *   positive decisions via @c hltObjectSummaries() method, which takes optional arguments to restrict output to specific
 *   TOS or TIS requirements.
 * @par Outputs of @c triggerTisTos
 *   @c decision, @c TIS, @c TOS - are all of @c bool type.
 *   They can be retrived either via variables passed via reference or via @c TisTosDecision
 *   helper class (the latter is easier in python).
 * @par
 *   @c decision is true if any selection corresponding to the Trigger Input was satisfied.
 *   This output is independent of the Offline Input!
 * @par
 *   @c TIS = Trigger Independent of Signal ("Signal" = Offline Input) is true if the any selection corresponding
 *   to the Trigger Input is satisfied with trigger tracks/vertices/particles which are not matched to the Offline Input.
 *   Matching criteria are implementation dependent.
 * @par
 *   @c TOS = Trigger on Signal is true if any selection corresponding to the Trigger Input is satisfied with trigger
 *   tracks/vertices/particles which are matched to the Offline Input. Matching criteria are implementation dependent and
 *   not necessarily the same as for TIS (i.e. not-TIS object is not necessarily the same as TOS object)
 * @par
 *   TIS, TOS are not mutually exclusive categories i.e. TIS==true and TOS==true is possible.
 * @par
 *   Trigger On Both (TOB) = decision && !TIS && !TOS.
 * @par
 *   if decision==false then TIS and TOS are automatically false too.
 *
 * @par Simple Example:
 *     @code
 *      // obtain Tis,Tos calssification of the Hlt1 global decision with respect to selected B candidate
 *      ITriggerTisTos* triggerTisTosTool = tool<ITriggerTisTos>("TriggerTisTos",this);
 *      LHCb::Particle signalB =...;
 *      bool decision,tis,tos;
 *      triggerTisTosTool->triggerTisTos( signalB, "Hlt1.*Decision", decision, tis, tos);
 *      bool tob = decision && (!tis) && (!tos);
 *     @endcode
 */
class GAUDI_API ITriggerTisTos : virtual public ITriggerSelectionTisTos
{

 public:

  /// Interface ID
  DeclareInterfaceID(ITriggerTisTos, 4, 0);

  /// Destructor
  virtual ~ITriggerTisTos() { }

 public:


  // -------------------------------------------------
  // ------------ Trigger input
  // -------------------------------------------------

  /// erase previous Trigger Input
  virtual void setTriggerInput( ) = 0;

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  virtual void addToTriggerInput( const std::string & selectionNameWithWildChar ) =0;

  // -------------------------------------------------
  // ------------ OUTPUTS  ----------------------
  // -------------------------------------------------

  /// Complete classification of the Trigger Input (see ITisTis::TisTosTob for the meaning)
  virtual unsigned int tisTosTrigger() =0;

  /// check for TOS  - may be faster than using tisTosTrigger()
  virtual bool tosTrigger() =0;

  /// check for TIS  - may be faster than using tisTosTrigger()
  virtual bool tisTrigger() =0;

  /// check for TUS (Trigger Used Signal: TPS or TOS) - may be faster than using tisTosTrigger()
  virtual bool tusTrigger() =0;

  /// analysis report
  virtual std::string analysisReportTrigger() =0;

  /// analysis report: typical use
  std::string analysisReportTrigger( const std::string & selectionNameWithWildChar )
  {
    setTriggerInput();
    addToTriggerInput(selectionNameWithWildChar);
    return ( " Trigger Pattern ="+ selectionNameWithWildChar+ " BEGIN\n"
             + analysisReportTrigger()
             +" Trigger Pattern ="+ selectionNameWithWildChar+ " END\n");
  }

  /// shortcut to get result in TisTosTob format
  TisTosTob tisTosTobTrigger()  { return TisTosTob(tisTosTrigger()); }

  // ------------ auxiliary output --------------------------------------------


  /** @par triggerSelectionNames
   *       returns Trigger Selection Names among the Trigger Input matching optional pattern of decision,tis,tos
   *       (tis,tos depend on the Offline Input).
   *  @par
   *       Possible decision,tis,tos pattern values are: 0=false is required; 1=true is required; >=2 no requirement.
   *       Use ITriggerTisTos::kFalseRequired, ITriggerTisTos::kTrueRequired, ITriggerTisTos::kAnything to avoid magic numbers.
   *  @par
   *       Some combinations make no sense, thus will always return empty vector (e.g. decision==false && tos==true).
   *  @par
   *       When no pattern is imposed (default) all Trigger Selection names corresponding to the Trigger Input are returned.
   *  @par
   *       Inlined shortcuts to define the Trigger Input and get outputs in one call are provided.
   */
  /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  virtual std::vector< std::string >
    triggerSelectionNames(unsigned int decisionRequirement = kAnything,
                          unsigned int tisRequirement = kAnything,
                          unsigned int tosRequirement = kAnything,
                          unsigned int tpsRequirement = kAnything)  = 0;


  /** @par hltObjectSummaries
   *       returns HltObjectSummaries for trigger objects which match optional pattern of tis,tos
   *       (tis,tos depend on the Offline Input) for all triggers in the Trigger Input
   *  @par
   *       Possible tis,tos pattern values are: 0=false is required; 1=true is required; >=2 no requirement.
   *       Use ITriggerTisTos::kFalseRequired, ITriggerTisTos::kTrueRequired, ITriggerTisTos::kAnything to avoid magic numbers.
   *  @par
   *       When no pattern is imposed (default) HltObjectSummaries for all trigger objects in the Trigger Input are returned.
   *  @par
   *       Guide to Tis-Tos pattern meaning:
   *       kFalseRequired,kFalseRequired -> TOB trigger objects (i.e. neither TIS nor TOS);
   *       kFalseRequired,kTrueRequired  -> TOS trigger objects (same as kAnything,kTrueRequired);
   *       kTrueRequired,kFalseRequired  -> TIS trigger objects (same as kTrueRequired,kAnything);
   *       kTrueRequired,kTrueRequired   -> cannot be satisfied;
   *       kFalseRequired,kAnything      -> TOS and TOB trigger objects;
   *       kAnything,kFalseRequired      -> TIS and TOB trigger objects;
   *       kAnything,kAnything           -> all trigger objects;
   *  @par
   *       Optional TPS requirement should be redundant with the above for simple objects.
   *       Has independent meaning for objects with substructure, when CompositeTPSviaTOSonly in ParticleTisTos is used.
   *  @par
   *       Inlined shortcuts to define the Offline and Trigger Inputs and get outputs in one call are provided.
   */

  /// list of HltObjectSummaries from Selections satisfying TOS,TIS requirements (define Trigger and Offline Input before calling)
  virtual std::vector<const LHCb::HltObjectSummary*>
    hltObjectSummaries( unsigned int tisRequirement      = kAnything,
                        unsigned int tosRequirement      = kAnything,
                        unsigned int tpsRequirement      = kAnything ) = 0;



  // ------------------------------ LEGACY INTERFACE ------------------------------------------------------

  /** @par triggerTisTos
   *       Calculates decision,tis,tos for previously defined Trigger Input (set of Trigger Selection Names)
   *       with respect to previously defined Offline Input (set of Particles,tracks or hits).
   *       Only tis and tos depend on the Offline Input.
   * @par
   *       To set the Offline Input see methods inherited from @c ITriggerSelectionTisTos.
   * @par
   *       Inlined shortcuts to define the inputs and get outputs in one call are provided.
   */

  /// calculate decision,Tis,Tos  (for previously defined Trigger and Offline Inputs) - see inheritance for Offline Input def
  void triggerTisTos( bool & decision, bool & tis, bool & tos )
  {
    const unsigned int result = tisTosTrigger();
    decision = ((result&kDecision)!=0);
    tis = ((result&kTIS)!=0);
    tos = ((result&kTOS)!=0);
  }

  // -------------------------------------------------
  // ------------ inlined shortcuts for user convenience
  // -------------------------------------------------

  /// python friendly - calculate decision,Tis,Tos  (for previously defined Trigger and Offline Inputs)
  TisTosDecision triggerTisTos( )
  {
    bool decision(false),tis(false),tos(false);
    triggerTisTos(decision,tis,tos);
    return TisTosDecision(decision,tis,tos);
  }

  // ------------  various ways to define trigger input ------------------------------

  /// shortcut to add initialize Trigger Input and add Trigger Name Pattern to it
  void setTriggerInput( const std::string & selectionNameWithWildChar )
  {
    setTriggerInput();
    addToTriggerInput(selectionNameWithWildChar);
  }

  /// shortcut to add initialize Trigger Input and add Trigger Name Patterns to it
  void setTriggerInput( const std::vector<std::string> & selectionNamesWithWildChar )
  {
    setTriggerInput();
    for( std::vector<std::string>::const_iterator iName = selectionNamesWithWildChar.begin();
         iName != selectionNamesWithWildChar.end(); ++iName )
    {
      addToTriggerInput(*iName);
    }
  }

  //  -------- shortcuts to specify trigger input and get results in one call (for previously defined offline input)
  // do not use shortcuts if calling the tool mutiple times with the same trigger input but changing offline input

  /// shortcut to define Trigger Input via name pattern and return decision,tis,tos with respect to previously set Offline Input
  void triggerTisTos( const std::string & selectionNameWithWildChar,
                      bool & decision, bool & tis, bool & tos )
  { setTriggerInput(selectionNameWithWildChar); triggerTisTos(decision,tis,tos); }

  /// python friendly - define Trigger Input and return decision,tis,tos with respect to previously set Offline Input
  TisTosDecision triggerTisTos( const std::string & selectionNameWithWildChar )
  {
    bool decision(false),tis(false),tos(false);
    triggerTisTos(selectionNameWithWildChar,decision,tis,tos);
    return TisTosDecision(decision,tis,tos);
  }

  /// shortcut to define Trigger Input via name patterns and return decision,tis,tos with respect to previously set Offline Input
  void triggerTisTos( const std::vector< std::string > & selectionNamesWithWildChar,
                      bool & decision, bool & tis, bool & tos )
  {
    setTriggerInput(selectionNamesWithWildChar);
    triggerTisTos(decision,tis,tos);
  }

  //  -------- shortcuts to specify offline input and get results in one call (keep previous trigger definition)
  // do not use shortcuts if calling the tool mutiple times with the same offline input but changing trigger input

  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
  void triggerTisTos( const LHCb::Particle & particle,
                      bool & decision, bool & tis, bool & tos )
  { setOfflineInput(particle); triggerTisTos(decision,tis,tos); }

  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
  void triggerTisTos( const LHCb::ProtoParticle & protoParticle,
                      bool & decision, bool & tis, bool & tos )
  { setOfflineInput(protoParticle); triggerTisTos(decision,tis,tos); }

  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
  void triggerTisTos( const LHCb::Track & track,
                      bool & decision, bool & tis, bool & tos )
  { setOfflineInput(track); triggerTisTos(decision,tis,tos); }


  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
  void triggerTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                      bool & decision, bool & tis, bool & tos )
  { setOfflineInput(hitlist); triggerTisTos(decision,tis,tos); }

  //  -------- shortcuts to specify offline input, trigger input and get results in one call
  // do not use shortcuts if calling the tool mutiple times with the same offline input or same trigger input

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos
  void triggerTisTos( const LHCb::Particle & particle,
                      const std::string & selectionNameWithWildChar,
                      bool & decision, bool & tis, bool & tos )
  {
    setOfflineInput(particle);
    setTriggerInput(selectionNameWithWildChar);
    triggerTisTos(decision,tis,tos);
  }

  /// python friendly - shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos
  TisTosDecision triggerTisTos( const LHCb::Particle & particle,
                                const std::string & selectionNameWithWildChar )
  {
    bool decision(false),tis(false),tos(false);
    triggerTisTos(particle,selectionNameWithWildChar,decision,tis,tos);
    return TisTosDecision(decision,tis,tos);
  }

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos
  void triggerTisTos( const LHCb::ProtoParticle & protoParticle,
                      const std::string & selectionNameWithWildChar,
                      bool & decision, bool & tis, bool & tos )
  {
    setOfflineInput(protoParticle);
    setTriggerInput(selectionNameWithWildChar);
    triggerTisTos(decision,tis,tos);
  }

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos
  void triggerTisTos( const LHCb::Track & track,
                      const std::string & selectionNameWithWildChar,
                      bool & decision, bool & tis, bool & tos )
  {
    setOfflineInput(track);
    setTriggerInput(selectionNameWithWildChar);
    triggerTisTos(decision,tis,tos);
  }

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos
  void triggerTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                      const std::string & selectionNameWithWildChar,
                      bool & decision, bool & tis, bool & tos )
  {
    setOfflineInput(hitlist);
    setTriggerInput(selectionNameWithWildChar);
    triggerTisTos(decision,tis,tos);
  }

  // ------------ auxiliary output --------------------------------------------

  /// shortcut to set Trigger Input and return Trigger Sel. names matching dec.,tis,tos pattern for previously set Offline Input
  std::vector< std::string > triggerSelectionNames( const std::string & selectionNameWithWildChar,
                                                    unsigned int decisionRequirement = kAnything,
                                                    unsigned int tisRequirement = kAnything,
                                                    unsigned int tosRequirement = kAnything,
                                                    unsigned int tpsRequirement = kAnything )
  {
    setTriggerInput(selectionNameWithWildChar);
    return triggerSelectionNames(decisionRequirement,tisRequirement,tosRequirement,tpsRequirement);
  }


  /// shortcut to set Offline and Trigger Input and return Trigger Sel. names matching dec.,tis,tos pattern
  std::vector< std::string > triggerSelectionNames( const LHCb::Particle & particle,
                                                    const std::string & selectionNameWithWildChar,
                                                    unsigned int decisionRequirement = kAnything,
                                                    unsigned int tisRequirement = kAnything,
                                                    unsigned int tosRequirement = kAnything,
                                                    unsigned int tpsRequirement = kAnything )
  {
    setOfflineInput(particle);
    setTriggerInput(selectionNameWithWildChar);
    return triggerSelectionNames(decisionRequirement,tisRequirement,tosRequirement,tpsRequirement);
  }


  // ------------ auxiliary output --------------------------------------------


  /// shortcut to set Trigger Input and call hltObjectSummaries
  std::vector<const LHCb::HltObjectSummary*>
    hltObjectSummaries( const std::string & selectionNameWithWildChar,
                        unsigned int tisRequirement      = kAnything,
                        unsigned int tosRequirement      = kAnything,
                        unsigned int tpsRequirement      = kAnything )
  {
    setTriggerInput(selectionNameWithWildChar);
    return hltObjectSummaries(tisRequirement,tosRequirement,tpsRequirement);
  }

  /// shortcut to set Offline and Trigger Input and call hltObjectSummaries
  std::vector<const LHCb::HltObjectSummary*>
    hltObjectSummaries( const LHCb::Particle & particle,
                        const std::string & selectionNameWithWildChar,
                        unsigned int tisRequirement      = kAnything,
                        unsigned int tosRequirement      = kAnything,
                        unsigned int tpsRequirement      = kAnything)
  {
    setOfflineInput(particle);
    setTriggerInput(selectionNameWithWildChar);
    return hltObjectSummaries(tisRequirement,tosRequirement,tpsRequirement);
  }

};

#endif // ITRIGGERTISTOS_H
