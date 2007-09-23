// $Id: ITriggerTisTos.h,v 1.2 2007-09-23 13:57:05 pkoppenb Exp $
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
namespace LHCb {
  class Particle;
  class ProtoParticle;
  class Track;
  class RecVertex;
};


static const InterfaceID IID_ITriggerTisTos ( "ITriggerTisTos", 1, 0 );

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
 *   of the particular trigger configuration.  The Trigger Selections by default are restricted to be among exit points from 
 *   the Alley system, but this restrictions can be waived.
 *   TIS,TOS classifications can be drilled down entire trigger tree by following input Trigger Selections 
 *   to the Trigger Selections requested by the caller. 
 *   Auxiliary output functions allow discovering valid Trigger Selection names and their input Trigger Selections, 
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
 *   Examples:  "*", "MuonTrigger*", "MuonTrigger*DiMu*", "MuonTriggerSingle"
 * @par
 *   If Trigger Input corresponds to many trigger selection names, the TisTosing output corresponds 
 *   to an OR between these selections.
 * @par
 *   Trigger Selections are restricted to belong to exit selections from any trigger Alley, unless optional 
 *   input parameter @c alleyExitsOnly is set to false. 
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
 * 
 * @par Outputs of @c triggerTisTos 
 *   @c decision, @c TIS, @c TOS - are all of @c bool type. 
 * @par
 *   @c decision is true if any selection corresponding to the Trigger Input was satisfied. 
 *   This output is independent of the Offline Input!
 * @par
 *   @c TIS = Trigger Independent of Signal ("Signal" = Offline Input) is true if the any selection corresponding 
 *   to the Trigger Input is satisfied with trigger tracks/vertices/particles which are not matched to the Offline Input. 
 *   Matching criteria are implementation dependent.   
 *   Input selections for given TIS selection are also checked for TIS unless requested otherwise.
 * @par   
 *   @c TOS = Trigger on Signal is true if any selection corresponding to the Trigger Input is satisfied with trigger
 *   tracks/vertices/particles which are matched to the Offline Input. Matching criteria are implementation dependent and
 *   not necessarily the same as for TIS (i.e. not-TIS object is not necessarily the same as TOS object)
 *   Input selections for given TOS selection are also checked for TOS unless requested otherwise.
 * @par  
 *   TIS, TOS are not mutually exclusive categories i.e. TIS==true and TOS==true is possible.
 * @par   
 *   Trigger On Both (TOB) = decision && !TIS && !TOS.
 * @par
 *   if decision==false then TIS and TOS are automatically false too.
 * @par
 *   By default TisTosing is performed only on the trigger selections directly corresponding to the Trigger Input.
 *   The tool will also include all their input trigger selections (with arbitrary depth) if the optional input parameter 
 *   @c checkInputSelections is set to true. For the latter option to work properly the tool may have to be configured
 *   in trigger dependent way, thus use it with care.
 * 
 * @par Simple Example:
 *     @code
 *      // obtain Tis,Tos calssification of the Hlt Alleys with respect to selected B candidate 
 *      ITriggerTisTos* triggerTisTosTool = tool<ITriggerTisTos>("TriggerTisTos","TriggerTisTos",this);
 *      LHCb::Particle signalB =...;
 *      bool decision,tis,tos;
 *      triggerTisTosTool->triggerTisTos( signalB, "*", decision, tis, tos);
 *      bool tob = decision && (!tis) && (!tos);  
 *     @endcode
 */
class ITriggerTisTos : virtual public ITriggerSelectionTisTos {
public: 

  /// meaning of optional bool input parameter passed together with Trigger Pattern string
  enum TriggerRestrictionsFlag
    {kAllTriggerSelections=0,kAlleyExitsOnly,kDefaultTriggerRestrictions=kAlleyExitsOnly};
  /// meaning of optional bool input parameter passed when calling TisTos output functions
  enum CheckInputSelectionsFlag 
    {kDontCheckInputSelections=0,kDefaultInputSelectionsChecking=kDontCheckInputSelections,kCheckInputSelections};
  /// meaning of 3 (dec,tis,tos) optional unsigned int input parameters passed to @c triggerSelectionNames() output function 
  enum TisTosRequirement 
    {kFalseRequired,kTrueRequired,kAnything};

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITriggerTisTos; }

  // -------------------------------------------------
  // ------------ basic interface (must be implemented)
  // -------------------------------------------------

  /// must be called by user before Trigger Input calls if the trigger structure may have changed since the previous call
  virtual void refreshTriggerStructure( ) =0; 

  /// erase previous Trigger Input 
  virtual void setTriggerInput( ) = 0; 

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  virtual void addToTriggerInput( const std::string & selectionNameWithWildChar,
                                  bool alleyExitsOnly = kDefaultTriggerRestrictions) =0;
   
  /** @par triggerTisTos
   *       Calculates decision,tis,tos for previously defined Trigger Input (set of Trigger Selection Names)
   *       with respect to previously defined Offline Input (set of Particles,tracks or hits).
   *       Only tis and tos depend on the Offline Input.
   * @par
   *       To set the Offline Input see methods inherited from @c ITriggerSelectionTisTos.
   * @par
   *       Inlined shortcuts to define the inputs and get outputs in one call are provided.
   * @par  
   *       By default Tis,Tos classifications are not drilled down the trigger selection tree and are based
   *       on top level selection specified by the user. To check for consistency through lower trigger levels
   *       set the optional @c checkInputSelections to true (= ITriggerTisTos::kCheckInputSelections).
   *       For this option to work properly the tool may have to be configured in trigger dependent way, 
   *       thus use it with care.
   */  
  /// calculate decision,Tis,Tos  (for previously defined Trigger and Offline Inputs) - see inheritance for Offline Input def 
  virtual void triggerTisTos( bool & decision, bool & tis, bool & tos, 
                              bool checkInputSelections = kDefaultInputSelectionsChecking ) =0;

  // ------------ auxiliary output --------------------------------------------


  /** @par triggerSelectionNames
   *       returns Trigger Selection Names among the Trigger Input matching optional pattern of decision,tis,tos
   *       (tis,tos depend on the Offline Input and optional @c checkInputSelections parameter).
   *  @par
   *       Possible decision,tis,tos pattern values are: 0=false is required; 1=true is required; >=2 no requirement.
   *       Use ITriggerTisTos::kFalseRequired, ITriggerTisTos::kTrueRequired, ITriggerTisTos::kAnything to avoid magic numbers. 
   *  @par
   *       Some combinations make no sense, thus will always return empty vector (e.g. decision==false && tos==true).
   *  @par
   *       When no pattern is imposed (default) all Trigger Selection names corresponding to the Trigger Input are returned.
   *  @par
   *       Returned Trigger Selection names do not include their input selection names, 
   *       even if @c checkInputSelections is set to true.
   *       The latter parameter affects outcome of Tis,Tos pattern matching.  
   * @par
   *       Inlined shortcuts to define the Trigger Input and get outputs in one call are provided.
   */
  /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  virtual std::vector< std::string > triggerSelectionNames(unsigned int decisionRequirement = kAnything, 
                                                           unsigned int tisRequirement = kAnything,
                                                           unsigned int tosRequirement = kAnything,
                                                           bool checkInputSelections = kDefaultInputSelectionsChecking )  = 0; 

  // ------------ auxiliary output:  list of input trigger selections to given trigger selection

  ///  list of input Trigger Selections to given trigger selection (notice no wild characters allowed in the selection name)
  virtual const std::vector< std::string > & inputTriggerSelectionNames( const std::string & selectionNameWithNoWildChar )  = 0;

  // -------------------------------------------------
  // ------------ inlined shortcuts for user convenience
  // -------------------------------------------------

  // ------------  various ways to define trigger input ------------------------------

  /// shortcut to add initialize Trigger Input and add Trigger Name Pattern to it
  void setTriggerInput(  const std::string & selectionNameWithWildChar,
                         bool alleyExitsOnly = kDefaultTriggerRestrictions)
    { setTriggerInput(); addToTriggerInput(selectionNameWithWildChar,alleyExitsOnly); }

  /// shortcut to add initialize Trigger Input and add Trigger Name Patterns to it
  void setTriggerInput(  const std::vector< std::string > & selectionNamesWithWildChar, 
                         bool alleyExitsOnly = kDefaultTriggerRestrictions )
    { setTriggerInput(); 
      for( std::vector< std::string >::const_iterator iName=selectionNamesWithWildChar.begin();
           iName!=selectionNamesWithWildChar.end();++iName){
        addToTriggerInput(*iName,alleyExitsOnly);
      }
    }

  //  -------- shortcuts to specify trigger input and get results in one call (for previously defined offline input)
  // do not use shortcuts if calling the tool mutiple times with the same trigger input but changing offline input 

  /// shortcut to define Trigger Input via name pattern and return decision,tis,tos with respect to previously set Offline Input
  void triggerTisTos( const std::string & selectionNameWithWildChar,
                      bool & decision, bool & tis, bool & tos,
                      bool alleyExitsOnly = kDefaultTriggerRestrictions ,
                      bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setTriggerInput(selectionNameWithWildChar,alleyExitsOnly);
    triggerTisTos(decision,tis,tos,checkInputSelections); }

  /// shortcut to define Trigger Input via name patterns and return decision,tis,tos with respect to previously set Offline Input
  void triggerTisTos( const std::vector< std::string > & selectionNamesWithWildChar,
                      bool & decision, bool & tis, bool & tos, 
                      bool alleyExitsOnly = kDefaultTriggerRestrictions,
                      bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly);
    triggerTisTos(decision,tis,tos,checkInputSelections); }

  //  -------- shortcuts to specify offline input and get results in one call (keep previous trigger definition)
  // do not use shortcuts if calling the tool mutiple times with the same offline input but changing trigger input 

  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
   void triggerTisTos( const LHCb::Particle & particle,
                       bool & decision, bool & tis, bool & tos, bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(particle); triggerTisTos(decision,tis,tos,checkInputSelections); }

  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
   void triggerTisTos( const LHCb::ProtoParticle & protoParticle,
                       bool & decision, bool & tis, bool & tos, bool checkInputSelections = kDefaultInputSelectionsChecking)
     { setOfflineInput(protoParticle); triggerTisTos(decision,tis,tos,checkInputSelections); }

  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
   void triggerTisTos( const LHCb::Track & track,
                       bool & decision, bool & tis, bool & tos, bool checkInputSelections = kDefaultInputSelectionsChecking)
     { setOfflineInput(track); triggerTisTos(decision,tis,tos,checkInputSelections); }


  /// shortcut to define Offline Input and calculate decision,tis,tos for previously defined Trigger Input
   void triggerTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                       bool & decision, bool & tis, bool & tos, bool checkInputSelections = kDefaultInputSelectionsChecking)
     { setOfflineInput(hitlist); triggerTisTos(decision,tis,tos,checkInputSelections); }

   //  -------- shortcuts to specify offline input, trigger input and get results in one call 
   // do not use shortcuts if calling the tool mutiple times with the same offline input or same trigger input 

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos 
   void triggerTisTos( const LHCb::Particle & particle,
                       const std::string & selectionNameWithWildChar,
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setOfflineInput(particle); setTriggerInput(selectionNameWithWildChar,alleyExitsOnly); 
    triggerTisTos(decision,tis,tos,checkInputSelections); }

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos 
   void triggerTisTos( const LHCb::ProtoParticle & protoParticle,
                       const std::string & selectionNameWithWildChar,
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(protoParticle); setTriggerInput(selectionNameWithWildChar,alleyExitsOnly);
       triggerTisTos(decision,tis,tos,checkInputSelections); }

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos 
   void triggerTisTos( const LHCb::Track & track,
                       const std::string & selectionNameWithWildChar,
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(track); setTriggerInput(selectionNameWithWildChar,alleyExitsOnly);
       triggerTisTos(decision,tis,tos,checkInputSelections); } 

  /// shortcut to define Offline Input and Trigger Input via trigger name pattern and calculate decision,tis,tos 
   void triggerTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                       const std::string & selectionNameWithWildChar,
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(hitlist);  setTriggerInput(selectionNameWithWildChar,alleyExitsOnly);
       triggerTisTos(decision,tis,tos,checkInputSelections); }  

  // ---
  
  /// shortcut to define Offline Input and Trigger Input via trigger name patterns and calculate decision,tis,tos 
   void triggerTisTos( const LHCb::Particle & particle,
                       const std::vector< std::string > & selectionNamesWithWildChar, 
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(particle); setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly); 
       triggerTisTos(decision,tis,tos,checkInputSelections); } 


  /// shortcut to define Offline Input and Trigger Input via trigger name patterns and calculate decision,tis,tos 
   void triggerTisTos( const LHCb::ProtoParticle & protoParticle,
                       const std::vector< std::string > & selectionNamesWithWildChar, 
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(protoParticle); setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly); 
       triggerTisTos(decision,tis,tos,checkInputSelections); }  

  /// shortcut to define Offline Input and Trigger Input via trigger name patterns and calculate decision,tis,tos 
   void triggerTisTos( const LHCb::Track & track,
                       const std::vector< std::string > & selectionNamesWithWildChar, 
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(track); setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly); 
       triggerTisTos(decision,tis,tos,checkInputSelections); }

  /// shortcut to define Offline Input and Trigger Input via trigger name patterns and calculate decision,tis,tos 
   void triggerTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                       const std::vector< std::string > & selectionNamesWithWildChar, 
                       bool & decision, bool & tis, bool & tos,
                       bool alleyExitsOnly = kDefaultTriggerRestrictions,
                       bool checkInputSelections = kDefaultInputSelectionsChecking )
     { setOfflineInput(hitlist); setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly); 
       triggerTisTos(decision,tis,tos,checkInputSelections); }  
  
   // ------------ auxiliary output --------------------------------------------
      
  /// shortcut to set Trigger Input and return Trigger Sel. names matching dec.,tis,tos pattern for previously set Offline Input 
  std::vector< std::string > triggerSelectionNames( const std::string & selectionNameWithWildChar,
                                                    bool alleyExitsOnly = kDefaultTriggerRestrictions, 
                                                    unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement = kAnything,
                                                    unsigned int tosRequirement = kAnything,
                                                    bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setTriggerInput(selectionNameWithWildChar,alleyExitsOnly);
    return triggerSelectionNames(decisionRequirement,tisRequirement,tosRequirement,checkInputSelections); }
  
  /// shortcut to set Trigger Input and return Trigger Sel. names matching dec.,tis,tos pattern for previously set Offline Input 
  std::vector< std::string > triggerSelectionNames( const std::vector< std::string > & selectionNamesWithWildChar, 
                                                    bool alleyExitsOnly = kDefaultTriggerRestrictions,
                                                    unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement = kAnything,
                                                    unsigned int tosRequirement = kAnything,
                                                    bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly);
    return triggerSelectionNames(decisionRequirement,tisRequirement,tosRequirement,checkInputSelections); }

  /// shortcut to set Offline and Trigger Input and return Trigger Sel. names matching dec.,tis,tos pattern 
  std::vector< std::string > triggerSelectionNames( const LHCb::Particle & particle,
                                                    const std::string & selectionNameWithWildChar,
                                                    bool alleyExitsOnly = kDefaultTriggerRestrictions, 
                                                    unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement = kAnything,
                                                    unsigned int tosRequirement = kAnything,
                                                    bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setTriggerInput(selectionNameWithWildChar,alleyExitsOnly);
    return triggerSelectionNames(decisionRequirement,tisRequirement,tosRequirement,checkInputSelections); }
  
  /// shortcut to set Offline and Trigger Input and return Trigger Sel. names matching dec.,tis,tos pattern 
  std::vector< std::string > triggerSelectionNames( const LHCb::Particle & particle,
                                                    const std::vector< std::string > & selectionNamesWithWildChar, 
                                                    bool alleyExitsOnly = kDefaultTriggerRestrictions,
                                                    unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement = kAnything,
                                                    unsigned int tosRequirement = kAnything,
                                                    bool checkInputSelections = kDefaultInputSelectionsChecking )
  { setTriggerInput(selectionNamesWithWildChar,alleyExitsOnly);
    return triggerSelectionNames(decisionRequirement,tisRequirement,tosRequirement,checkInputSelections); }



protected:

private:

};
#endif // ITRIGGERTISTOS_H
