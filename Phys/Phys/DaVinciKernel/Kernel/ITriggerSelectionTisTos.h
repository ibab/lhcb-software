// $Id: ITriggerSelectionTisTos.h,v 1.2 2007-10-10 20:22:21 tskwarni Exp $
#ifndef ITRIGGERSELECTIONTISTOS_H 
#define ITRIGGERSELECTIONTISTOS_H 1

// Include files
// from STL
#include <string>
#include <vector>

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


static const InterfaceID IID_ITriggerSelectionTisTos ( "ITriggerSelectionTisTos", 1, 1 );

/** @class ITriggerSelectionTisTos ITriggerSelectionTisTos.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-06
 *
 * @par Tool purpose 
 *   Primary function of this tool is to determine TIS and TOS categories (see below for definition) 
 *   for given Trigger Selection Summary (or set of Trigger Selection Summaries) with  
 *   respect to a user specified "Offline Input".
 * @par
 *   The Tool can be used to TisTos either Trigger Selections run in Alleys (
 *   the summary holds either Tracks or Vertices) or in HltExclusive (the summary holds Particles).
 * @par
 *   The tool can also return a vector of all TOS matches in the Trigger Selection Output (saved in Summary) 
 *   to the Offline Input, which, depending on the Selection Summary can be either a 
 *   vector of Tracks, RecVertecies or Particles.   
 *   This vector is ordered according to the match quality, with the best match 
 *   (implementation dependent!) coming first. 
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
 *      triggerSelectionTisTosTool->selectionTisTos( signalB, "HadTrigger", decision, tis, tos);  
 *      bool tob = decision && (!tis) && (!tos);  
 *     @endcode
 */
class ITriggerSelectionTisTos : virtual public IAlgTool {
public: 

  /// meaning of bool input parameter in case of multiple selection names passed to @c selectionTisTos()
  enum {kSelectionAND=0,kSelectionOR,kSelectionDefaultLogic=kSelectionOR};

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITriggerSelectionTisTos; }


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

  /// erase previous Offline Input 
  virtual void setOfflineInput( ) = 0; 
 

  /// Particle input; for composite particles loop over daughters will be executed
  virtual void addToOfflineInput( const LHCb::Particle & particle ) = 0; 

  /// Proto-particle input
  virtual void addToOfflineInput( const LHCb::ProtoParticle & protoParticle ) = 0; 

  /// Track input 
  virtual void addToOfflineInput( const LHCb::Track & track ) = 0;  

  ///    Detector hit input 
  virtual void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist ) = 0;

  /// single Trigger Selection Summary TisTos  (define Offline Input before calling)
  virtual void selectionTisTos( const std::string & selectionName,  
                                bool & decision, bool & tis, bool & tos ) = 0;

  /// multiple Trigger Selection Summaries TisTos (set selectionOR=false for an AND) (define Offline Input before calling)
  virtual void selectionTisTos( const std::vector< std::string > & selectionNames,
                                bool & decision, bool & tis, bool & tos ,
                                bool selectionOR = kSelectionDefaultLogic ) = 0;  // false->AND

  // ------------ auxiliary outputs

  /// list of LHCbIDs corresponding to present Offline Input (only hits used in matching are returned)
  virtual std::vector<LHCb::LHCbID> offlineLHCbIDs()  = 0; 

  /// ordered list of tracks from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  virtual std::vector<const LHCb::Track*>     matchedTOSTracks( const std::string & selectionName ) = 0;
  /// ordered list of vertices from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  virtual std::vector<const LHCb::RecVertex*> matchedTOSVertices( const std::string & selectionName ) = 0;
  /// ordered list of particles from Selection Summary (none if mismatch) satisfying TOS (define Offline Input before calling)
  virtual std::vector<const LHCb::Particle*>  matchedTOSParticles( const std::string & selectionName ) = 0;

  // -------------------------------------------------
  // ------------ inlined shortcuts for user convenience
  // -------------------------------------------------


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

   // ----

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const LHCb::Particle & particle,
                         const std::vector< std::string > & selectionNames, 
                         bool & decision, bool & tis, bool & tos )
     { setOfflineInput(particle);  selectionTisTos(selectionNames,decision,tis,tos); }

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const LHCb::ProtoParticle & protoParticle,
                         const std::vector< std::string > & selectionNames, 
                         bool & decision, bool & tis, bool & tos ) 
    { setOfflineInput(protoParticle);  selectionTisTos(selectionNames,decision,tis,tos); }


   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const LHCb::Track & track,
                         const std::vector< std::string > & selectionNames, 
                         bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(track);  selectionTisTos(selectionNames,decision,tis,tos); }

   /// shortcut to define Offline Input and get Tis,Tos results in one call
   void selectionTisTos( const std::vector<LHCb::LHCbID> & hitlist,
                         const std::vector< std::string > & selectionNames, 
                         bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(hitlist);  selectionTisTos(selectionNames,decision,tis,tos); }
  
  // ------------ auxiliary outputs

   /// shortcuts to define Offline Input and get list of hits in one call
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::Particle & particle)
     { setOfflineInput(particle); return offlineLHCbIDs(); }
   /// shortcuts to define Offline Input and get list of hits in one call
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::ProtoParticle & protoParticle)
     { setOfflineInput(protoParticle); return offlineLHCbIDs(); }
   /// shortcuts to define Offline Input and get list of hits in one call
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::Track & track)
     { setOfflineInput(track); return offlineLHCbIDs(); }    
  
  // ------------  additional functionality:  lists of tracks/vertices/particles from selection summary
  //               satisfying TOS, ordered according to TOS quality (best first)
  //               return empty vector in case of a mismatch between the output type and the selection summary

   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::Track & track, const std::string & selectionName )
     { setOfflineInput(track); return  matchedTOSTracks(selectionName); }
   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::ProtoParticle & protoParticle, const std::string & selectionName )
     { setOfflineInput(protoParticle); return  matchedTOSTracks(selectionName); }
   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::Particle & particle, const std::string & selectionName )
     { setOfflineInput(particle); return  matchedTOSTracks(selectionName); }
   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::Track*> matchedTOSTracks( const std::vector<LHCb::LHCbID> & hitlist,const std::string & selectionName)
     { setOfflineInput(hitlist); return  matchedTOSTracks(selectionName); }

   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::RecVertex*> matchedTOSVertices(const LHCb::Particle & particle, const std::string & selectionName )
     { setOfflineInput(particle); return  matchedTOSVertices(selectionName); }
   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::RecVertex*> matchedTOSVertices(const std::vector<LHCb::LHCbID> & hitlist, 
                                                          const std::string & selectionName )
     { setOfflineInput(hitlist); return  matchedTOSVertices(selectionName); }

   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::Particle*>  matchedTOSParticles(const LHCb::Particle & particle, const std::string & selectionName )
     { setOfflineInput(particle); return  matchedTOSParticles(selectionName); }
   /// shortcuts to define Offline Input and get matched TOS objects in one call
   std::vector<const LHCb::Particle*>  matchedTOSParticles(const std::vector<LHCb::LHCbID> & hitlist, 
                                                           const std::string & selectionName )
     { setOfflineInput(hitlist); return  matchedTOSParticles(selectionName); }


protected:

private:

};
#endif // ITRIGGERSELECTIONTISTOS_H
