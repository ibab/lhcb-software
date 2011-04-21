// $Id: ITriggerSelectionTisTos.h,v 1.4 2010-07-22 14:58:26 tskwarni Exp $
#ifndef ITRIGGERSELECTIONTISTOS_H 
#define ITRIGGERSELECTIONTISTOS_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include "boost/foreach.hpp"

// from Gaudi
//#include "GaudiKernel/IAlgTool.h"

#include "Kernel/IParticleTisTos.h"

#include "Kernel/LHCbID.h"

//forward declarations
namespace LHCb {
  class Particle;
  class ProtoParticle;
  class Track;
  class RecVertex;
  class HltObjectSummary;  
}

/** @class ITriggerSelectionTisTos ITriggerSelectionTisTos.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-06
 *
 * @par Tool purpose 
 *   Primary function of this tool is to determine TIS and TOS categories (see below for definition) 
 *   for given Trigger Selection Summary (or set of Trigger Selection Summaries) with  
 *   respect to a user specified "Offline Input". @c triggerSelectionTisTos method provides this
 *   functionality.
 * @par
 *   The tool can also return pointers to HltObjectSummaries for trigger objects (Tracks, RecVertices, Particles etc.) 
 *   responsible for positive decisions via @c hltSelectionObjectSummaries() method, which takes 
 *   optional arguments to restrict output to specific TOS or TIS requirements.
 *   Use @c hltObjects template to convert these lists into lists of pointers to the objects themselves.
 *   The latter may not work in true offline environment, since the objects are not made persistent.
 *   @c matchedTOSxxx methods use this functionality and are preserved for backwards compatibility.
 * @par
 *   User can also obtain a list of LHCbIDs of hits derived from Offline Input used 
 *   by the tool to perform matching between the offline and
 *   trigger content.
 * @par
 *   The interface was designed with hit based matching in mind, but the tool could be 
 *   implemented without it, in which case LHCbID input or output
 *   functions could do nothing.
 *
 * @par Offline Input
 *   Particle (with or without daughters), ProtoParticle, Track, a vector of LHCbIDs, 
 *   or combination of these can be passed as Offline Input.
 *   Passing ProtoParticle is not the same as passing the track the ProtoParticle may 
 *   be built on, since the tool can take input from PID info 
 *   (e.g. to grab Muon hits for matching).
 * @par
 *   @c setOfflineInput() called without parameters initializes new Offline Input.
 *   Then overloaded @c addToOfflineInput( <input> ) should be called once or many times.
 * @par
 *   @c setOfflineInput( <input> ) is a shortcut to the sequence of the above.
 * @par
 *   Once the Offline Input is defined, many calls to the output functions can be 
 *   made without re-entering the Offline Input.
 * @par
 *   The Offline Input can also be passed in functions which retrieve outputs, 
 *   but this becomes inefficient if many output function calls are
 *   made passing the same Offline Input (as the Offline Input is pre-processed 
 *   and stored in an internal representation).
 *   
 * @par Trigger Selection Input
 *   Trigger Selection Input is passed as an argument to output functions.
 *   Trigger Selection Summary is defined by its name (@c std::string). 
 *   The tool does not check the passed names for validity.
 *   If Trigger Selection Summary is not found (because the selection was 
 *   not run for that event, its output was not saved or the name is invalid) 
 *   dummy output is returned without any warnings.
 * @par
 *   All output functions accept single Trigger Selection name.
 * @par
 *   @c selectionTisTos can also accept a vector of Trigger Selection names, 
 *   in which case the output is OR (or AND depending on the last parameter) between
 *   all specified selections. Use this e.g. to TisTos Alleys with multiple 
 *   exit selections or entire collection of Alleys. 
 * 
 * @par Outputs of @c selectionTisTos. 
 *   @c decision, @c tis, @c tos - are all of @c bool type.
 *   They can be retrived either via parameters passed via reference or via @c TisTosDecision
 *   helper class (the latter is easier in python).
 * @par
 *   @c decision is true if the trigger selection was satisfied 
 *  (false if it failed or was not run). This output is independent of the Offline Input!
 * @par
 *   @c TIS = Trigger Independent of Signal ("Signal" = Offline Input) 
 *   is true if the selection is satisfied with trigger
 *   tracks/vertices/particles which are not matched to the Offline 
 *   Input. Matching criteria are implementation dependent.   
 * @par    
 *   @c TOS = Trigger on Signal is true if the selection is satisfied with trigger
 *   tracks/vertices/particles which are matched to the Offline Input. 
 *   Matching criteria are implementation dependent and
 *   not necessarily the same as for TIS (i.e. not-TIS object is not 
 *   necessarily the same as TOS object).
 * @par 
 *   TIS, TOS are not mutually exclusive categories i.e. TIS==true and TOS==true is possible.
 * @par  
 *   Trigger On Both (TOB) = decision && !TIS && !TOS.
 * @par
 *   if decision==false then TIS and TOS are automatically false too.   
 * 
 * @par Simple Example:
 *     @code
 *      // obtain Tis,Tos calssification of the Hadron Alley with respect 
 *      to selected B candidate 
 *      ITriggerSelectionTisTos* triggerSelectionTisTosTool = 
 *          tool<ITriggerSelectionTisTos>("TriggerSelectionTisTos",this);
 *      LHCb::Particle signalB =...
 *      bool decision,tis,tos;
 *      triggerSelectionTisTosTool->selectionTisTos( signalB, "Hlt2SelB2HHDecision", decision, tis, tos);  
 *      bool tob = decision && (!tis) && (!tos);  
 *     @endcode
 */
class GAUDI_API ITriggerSelectionTisTos : virtual public IParticleTisTos {
public: 

  DeclareInterfaceID(ITriggerSelectionTisTos, 4, 0);

  /// meaning of optional unsigned int input parameters for Tis/Tos requirements
  enum TisTosRequirement 
    {kFalseRequired,kTrueRequired,kAnything};

  /// meaning of bool input parameter in case of multiple selection names passed to @c selectionTisTos()
  enum {kSelectionAND=0,kSelectionOR,kSelectionDefaultLogic=kSelectionOR};

  class TisTosDecision 
  {
  public:

    TisTosDecision():m_decision(false),m_tis(false),m_tos(false){}
    TisTosDecision(bool dec,bool tis,bool tos):m_decision(dec),m_tis(tis),m_tos(tos){}
    virtual ~TisTosDecision(){};
  
    bool decision() const {return m_decision;}
    bool tis() const {return m_tis;}
    bool tos() const {return m_tos;}

    void set_decision(bool decision){m_decision=decision;}
    void set_tis(bool tis){m_tis=tis;}
    void set_tos(bool tos){m_tos=tos;}

  private:

    bool m_decision;
    bool m_tis;
    bool m_tos;
  };
  

  // -------------------------------------------------
  // ------------ basic interface (must be implemented)
  // -------------------------------------------------

  // -------------------------- OFFLINE INPUT (SIGNAL) -----------------------

  /// erase previous Offline Input 
  virtual void setOfflineInput( ) = 0; 
 

  /// Particle input; for composite particles loop over daughters will be executed
  virtual void addToOfflineInput( const LHCb::Particle & particle ) = 0; 

  /// Proto-particle input
  virtual void addToOfflineInput( const LHCb::ProtoParticle & protoParticle ) = 0; 

  /// Track input 
  virtual void addToOfflineInput( const LHCb::Track & track ) = 0;  

  /// Detector hit input 
  virtual void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist ) = 0;

  // -------------------------- OUTPUTS  -----------------------

  /// Complete classification of the Trigger Selection (see ITisTis::TisTosTob for the meaning)
  virtual unsigned int tisTosSelection( const std::string & selectionName ) =0;


  /// check for TOS  - may be faster than using tisTosSelection()
  virtual bool tosSelection(const std::string & selectionName) =0; 
 
  /// check for TIS  - may be faster than using tisTosSelection()
  virtual bool tisSelection(const std::string & selectionName) =0;

  /// check for TUS (Trigger Used Signal: TPS or TOS) - may be faster than using tisTosSelection()
  virtual bool tusSelection(const std::string & selectionName) =0;

  /// analysis report
  virtual std::string analysisReportSelection(const std::string & selectionName) =0;


  /// shortcut to get result in TisTosTob format 
  TisTosTob tisTosTobSelection( const std::string & selectionName )
  { return TisTosTob(tisTosSelection(selectionName)); }

  /// shortcut to get result for OR of selections
  unsigned int tisTosSelection( const std::vector< std::string > & selectionNames )
  {
      unsigned int result =0;    
      BOOST_FOREACH( std::string selName, selectionNames )
        { 
          result |= tisTosSelection(selName);
          if( (result&kTPS) && (result&kTOS) && (result&kTIS) && (result&kDecision) )break;
        }
      return result;
  }
  /// shortcut to get result in TisTosTob format for OR of selections
  TisTosTob tisTosTobSelection( const std::vector< std::string > & selectionNames )
  { return TisTosTob(tisTosSelection(selectionNames)); }

  bool tosSelection(const std::vector< std::string > & selectionNames )
  {
      BOOST_FOREACH( std::string selName, selectionNames )
        { 
          if( tosSelection(selName) )return true;
        }
      return false;      
  }
    
  bool tisSelection(const std::vector< std::string > & selectionNames )
  {
      BOOST_FOREACH( std::string selName, selectionNames )
        { 
          if( tisSelection(selName) )return true;
        }
      return false;      
  }
    
  bool tusSelection(const std::vector< std::string > & selectionNames )
  {
      BOOST_FOREACH( std::string selName, selectionNames )
        { 
          if( tusSelection(selName) )return true;
        }
      return false;      
  }
    
 


  // ------------ auxiliary outputs

  /// list of LHCbIDs corresponding to present Offline Input (only hits used in matching are returned)
  virtual std::vector<LHCb::LHCbID> offlineLHCbIDs()  = 0; 


   /** @par hltSelectionObjectSummaries
   *       returns HltObjectSummaries for trigger objects which match optional pattern of tis,tos
   *       (tis,tos depend on the Offline Input i.e. Signal) for specific Trigger identified by selection name
   *  @par
   *       Possible tis,tos pattern values are: 0=false is required; 1=true is required; >=2 no requirement.
   *       Use ITriggerTisTos::kFalseRequired, ITriggerTisTos::kTrueRequired, ITriggerTisTos::kAnything to avoid magic numbers. 
   *  @par
   *       When no pattern is imposed (default) HltObjectSummaries for all trigger objects in the Trigger Input are returned.
   *  @par
   *       Guide to Tis-Tos pattern meaning: 
   *       kFalseRequired,kFalseRequired -> TOB trigger objects (neither TIS nor TOS); [should be TPS]
   *       kFalseRequired,kTrueRequired  -> TOS trigger objects (same as kAnything,kTrueRequired);
   *       kTrueRequired,kFalseRequired  -> TIS trigger objects (same as kTrueRequired,kAnything);
   *       kTrueRequired,kTrueRequired   -> cannot be satisfied;
   *       kFalseRequired,kAnything      -> TOS and TOB trigger objects; [TUS]
   *       kAnything,kFalseRequired      -> TIS and TOB trigger objects;
   *       kAnything,kAnything           -> all trigger objects;
   *  @par
   *       Optional TPS requirement should be redundant with the above for simple objects.
   *       Has independent meaning for objects with substructure, when CompositeTPSviaTOSonly in ParticleTisTos is used. 
   *  @par
   *       Inlined shortcuts to define the Offline Input and get outputs in one call are provided.
   */

  /// list of HltObjectSummaries from Selection Summary satisfying TOS,TIS requirements (define Offline Input before calling)
  virtual std::vector<const LHCb::HltObjectSummary*> hltSelectionObjectSummaries( const std::string & selectionName,
                                                                                  unsigned int tisRequirement = kAnything,
                                                                                  unsigned int tosRequirement = kAnything,
                                                                                  unsigned int tpsRequirement = kAnything) =0;


  // ------------------------------ LEGACY INTERFACE ------------------------------------------------------

  // matchedTOSxxx these are legacy methods - to be retired 

  // list of tracks from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  //virtual std::vector<const LHCb::Track*>     matchedTOSTracks( const std::string & selectionName ) =0;

  // list of vertices from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  //virtual std::vector<const LHCb::RecVertex*> matchedTOSVertices( const std::string & selectionName ) =0;

  // list of particles from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  //virtual std::vector<const LHCb::Particle*>  matchedTOSParticles( const std::string & selectionName ) =0;

  // -------------------------------------------------
  // ------------ inlined shortcuts for user convenience
  // -------------------------------------------------

  // old style calling - to be retired 

  /// single Trigger Selection Summary TisTos  (define Offline Input before calling)
  void selectionTisTos( const std::string & selectionName,  
                                bool & decision, bool & tis, bool & tos )
  {
    unsigned int result = tisTosSelection(selectionName);
    decision = ((result&kDecision)!=0);
    tis = ((result&kTIS)!=0);
    tos = ((result&kTOS)!=0);
  }
  

  /// multiple Trigger Selection Summaries TisTos (set selectionOR=false for an AND) (define Offline Input before calling)
  void selectionTisTos( const std::vector< std::string > & selectionNames,
                        bool & decision, bool & tis, bool & tos ,
                        bool selectionOR = kSelectionDefaultLogic )  // false->AND
  {
    if( selectionOR ){
      unsigned int result =0;    
      BOOST_FOREACH( std::string selName, selectionNames )
        { 
          result |= tisTosSelection(selName);
          if( (result&kTOS) && (result&kTIS) && (result&kDecision) )break;
        }
      decision = ((result&kDecision)!=0);
      tis = ((result&kTIS)!=0);
      tos = ((result&kTOS)!=0);
      return;
    } else {
      unsigned int result =(kTOSandTIS|kDecision);      
      BOOST_FOREACH( std::string selName, selectionNames )
        { 
          result &= tisTosSelection(selName);
          if( !(result&kTOS) && !(result&kTIS) && !(result&kDecision) )break;
        }
      decision = ((result&kDecision)!=0);
      tis = ((result&kTIS)!=0);
      tos = ((result&kTOS)!=0);
      return;
    }
  }
      
  // --------------------------------------------------------------------------------------

  /// python friendly -  single Trigger Selection Summary TisTos  (define Offline Input before calling)
  TisTosDecision selectionTisTos( const std::string & selectionName )
  { bool decision,tis,tos; selectionTisTos(selectionName,decision,tis,tos);return TisTosDecision(decision,tis,tos);}
  
  /// python friendly - multiple Trigger Selection Summaries TisTos (set selectionOR=false for an AND) 
  TisTosDecision selectionTisTos( const std::vector< std::string > & selectionNames,
                                bool selectionOR = kSelectionDefaultLogic )
  { bool decision,tis,tos; selectionTisTos(selectionNames,decision,tis,tos,selectionOR);return TisTosDecision(decision,tis,tos);}

  // ------------  various ways to define off-line input ------------------------------

  /// Shortcut to initalize input and add a particle to it
  void setOfflineInput(   const LHCb::Particle & particle )
    { setOfflineInput(); addToOfflineInput(particle); }
  
  /// Shortcut to initalize input and add a proto-particle to it
  void setOfflineInput(   const LHCb::ProtoParticle & protoParticle )
    { setOfflineInput(); addToOfflineInput(protoParticle); }
  
  /// Shortcut to initalize input and add a track to it
  void setOfflineInput(   const LHCb::Track & track )
    { setOfflineInput(); addToOfflineInput(track); }
  
  /// Shortcut to initalize input and add hits to it
  void setOfflineInput(   const std::vector<LHCb::LHCbID> & hitlist )
    { setOfflineInput(); addToOfflineInput(hitlist); }
  

  // -------------- outputs:

   // shortcuts to define Offline Input and get results in one call
   // do not use shortcuts if calling the tool mutiple times with the same offline input 

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const LHCb::Particle & particle,
                         const std::string & selectionName,  
                         bool & decision, bool & tis, bool & tos )
     { setOfflineInput(particle);  selectionTisTos(selectionName,decision,tis,tos); }

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const LHCb::ProtoParticle & protoParticle,
                         const std::string & selectionName,  
                         bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(protoParticle);  selectionTisTos(selectionName,decision,tis,tos); }

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const LHCb::Track & track,
                         const std::string & selectionName,  
                         bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(track);  selectionTisTos(selectionName,decision,tis,tos); }

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                         const std::string & selectionName,  
                         bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(hitlist);  selectionTisTos(selectionName,decision,tis,tos); }

  // ------------ auxiliary outputs

   /// shortcuts to define Offline Input and get list of hits in one call
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::Track & track)
     { setOfflineInput(track); return offlineLHCbIDs(); }
   /// shortcuts to define Offline Input and get list of hits in one call
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::Particle & particle)
     { setOfflineInput(particle); return offlineLHCbIDs(); }

  // ------------  additional functionality:  lists of tracks/vertices/particles from selection summary
  //               satisfying TOS, ordered according to TOS quality (best first)
  //               return empty vector in case of a mismatch between the output type and the selection summary

   // shortcuts to define Offline Input and get matched TOS objects in one call
  //   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::Track & track, const std::string & selectionName )
  // { setOfflineInput(track); return  matchedTOSTracks(selectionName); }
   // shortcuts to define Offline Input and get matched TOS objects in one call
  // std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::Particle & particle, const std::string & selectionName )
  //   { setOfflineInput(particle); return  matchedTOSTracks(selectionName); }

   // shortcuts to define Offline Input and get matched TOS objects in one call
   //std::vector<const LHCb::RecVertex*> matchedTOSVertices(const LHCb::Particle & particle, const std::string & selectionName )
   //  { setOfflineInput(particle); return  matchedTOSVertices(selectionName); }

   // shortcuts to define Offline Input and get matched TOS objects in one call
   //std::vector<const LHCb::Particle*>  matchedTOSParticles(const LHCb::Particle & particle, const std::string & selectionName )
   //  { setOfflineInput(particle); return  matchedTOSParticles(selectionName); }

 // ------------ auxiliary output --------------------------------------------
  

  /// shortcut to set Offline and Trigger Input and call hltSelectionObjectSummaries 
  std::vector<const LHCb::HltObjectSummary*> hltSelectionObjectSummaries( const LHCb::Track & track,
                                                                          const std::string & selectionName,
                                                                 unsigned int tisRequirement      = kAnything,
                                                                 unsigned int tosRequirement      = kAnything,
                                                                 unsigned int tpsRequirement      = kAnything )
  { setOfflineInput(track); 
    return hltSelectionObjectSummaries(selectionName,tisRequirement,tosRequirement,tpsRequirement); }

  /// shortcut to set Offline and Trigger Input and call hltSelectionObjectSummaries 
  std::vector<const LHCb::HltObjectSummary*> hltSelectionObjectSummaries( const LHCb::Particle & particle,
                                                                          const std::string & selectionName,
                                                                 unsigned int tisRequirement      = kAnything,
                                                                 unsigned int tosRequirement      = kAnything,
                                                                 unsigned int tpsRequirement      = kAnything )
  { setOfflineInput(particle); 
    return hltSelectionObjectSummaries(selectionName,tisRequirement,tosRequirement,tpsRequirement); }


protected:

private:

};
#endif // ITRIGGERSELECTIONTISTOS_H
