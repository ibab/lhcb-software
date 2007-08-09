// $Id: ITriggerSelectionTisTos.h,v 1.1.1.1 2007-08-09 01:31:19 tskwarni Exp $
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


static const InterfaceID IID_ITriggerSelectionTisTos ( "ITriggerSelectionTisTos", 1, 0 );

/** @class ITriggerSelectionTisTos ITriggerSelectionTisTos.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-06
 *
 * \par Tool purpose 
 *   Primary function of this tool is to determine TIS and TOS categories (see below for definition) 
 *   for given Trigger Selection Summary (or set of Trigger Selection Summaries) with  
 *   respect to a user specified "Offline Input".
 * \par
 *   The Tool can be used to TisTos either Trigger Selections run in Alleys (the summary holds either Tracks or Vertices) or in HltExclusive (the summary holds Particles).
 * \par
 *   The tool can also return a vector of all TOS matches in the Trigger Selection Output (saved in Summary) 
 *   to the Offline Input, which, depending on the Selection Summary can be either a vector of Tracks, RecVertecies or Particles.   
 *   This vector is ordered according to the match quality, with the best match coming first.
 * \par
 *   User can also obtain a list of LHCbIDs of hits derived from Offline Input used by the tool to perform matching between the offline and
 *   trigger content.
 * \par
 *   The interface was designed with hit based matching in mind, but the tool could be implemented without it, in which case LHCbID input or output
 *   functions could do nothing.
 *
 * \par Offline Input
 *   Particle (with or without daughters), ProtoParticle, Track, a vector of LHCbIDs, or combination of these can be passed as Offline Input.
 *   Passing ProtoParticle is not the same as passing the track the ProtoParticle may be built on, since the tool can take input from PID info 
 *   (e.g. to grab Muon hits for matching).
 * \par
 *   \c setOfflineInput() called without parameters initializes new Offline Input.
 *   Then overloaded \c addToOfflineInput( <input> ) should be called once or many times.
 * \par
 *   \c setOfflineInput( <input> ) is a shortcut to the sequence of the above.
 * \par
 *   Once the Offline Input is defined, many calls to the output functions can be made without re-entering the Offline Input.
 * \par
 *   The Offline Input can also be passed in functions which retrieve outputs, but this becomes inefficient if many output function calls are
 *   made passing the same Offline Input (as the Offline Input is pre-processed and stored in an internal representation).
 *   
 * \par Trigger Selection Input
 *   Trigger Selection Summary is defined by its name (\c std::string). The tool does not check the passed names for validity.
 *   If Trigger Selection Summary is not found (because the selection was not run for that event, its output not saved or the name was invalid) 
 *   dummy output is returned without any warnings.
 * \par
 *   All output functions accept single Trigger Selection name.
 * \par
 *   \c selectionTisTos can also accept a vector of Trigger Selection names, in which case the output is OR (or AND depending on the last parameter) between
 *   all specified selections. Use this e.g. to TisTos Alleys with multiple exit selections or entire collection of Alleys. 
 * 
 * \par Outputs of \c selectionTisTos
 *   They are all of \c bool type. 
 * \par
 *   \c decision is true if selection was satisfied (false if it failed and was not run)
 * \par
 *   \c TIS = Trigger Independent of Signal ("Signal" = Offline Input) is true if the selection is satisfied with trigger
 *   tracks/vertices/particles which are not matched to the offline input. Matching criteria are implementation dependent.   
 *    
 *   \c TOS = Trigger on Signal is true if the selection is satisfied with trigger
 *   tracks/vertices/particles which are matched to the offline input. Matching criteria are implementation dependent and
 *   not necessarily the same as for TIS (i.e. not-TIS object is not necessarily the same as TOS object)
 *  
 *   TIS, TOS are not mutually exclusive categories i.e. TIS==true and TOS==true is possible
 *   
 *   Trigger On Both (TOB) = decision && !TIS && !TOS
 *
 *   if decision==false then TIS and TOS are automatically false too   
 */
class ITriggerSelectionTisTos : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITriggerSelectionTisTos; }

  virtual StatusCode         initialize() = 0;
  
  // ------------  various ways to define off-line input

  virtual void setOfflineInput( ) = 0; // erase previous input 
 
  //    Particle input; for composite particles loop over daughters will be executed
  virtual void addToOfflineInput( const LHCb::Particle & particle ) = 0; // add to input without erasing previous content
  void setOfflineInput(   const LHCb::Particle & particle )
    { setOfflineInput(); addToOfflineInput(particle); }
  
  //    Proto-particle input
  virtual void addToOfflineInput( const LHCb::ProtoParticle & protoParticle ) = 0; // add to input without erasing previous content
  void setOfflineInput(   const LHCb::ProtoParticle & protoParticle )
    { setOfflineInput(); addToOfflineInput(protoParticle); }
  
  //    Track input 
  virtual void addToOfflineInput( const LHCb::Track & track ) = 0;  // add to input without erasing previous content  
  void setOfflineInput(   const LHCb::Track & track )
    { setOfflineInput(); addToOfflineInput(track); }
  
  //    Detector hit input 
  virtual void addToOfflineInput( const std::vector<LHCb::LHCbID> & hitlist ) = 0;
  void setOfflineInput(   const std::vector<LHCb::LHCbID> & hitlist )
    { setOfflineInput(); addToOfflineInput(hitlist); }
  

  // -------------- outputs:
  //
  //    decision =  was trigger satisfied ? (this is independent of offline input passed to this tool!)
  //    tis      =  Trigger Independent of Signal  
  //                (trigger selection was satisified independently of the offline input passed to this tool)
  //    tos      =  Trigger On Signal  
  //                (the offline input passed to this tool was sufficient to satisfy this trigger selection)
  //   
  //    comments:
  //         if decision==false then tis=false and tos=false 
  //
  //         tis, tos are not mutually exclusive ( tis==true and tos=true is possible, TOE in TriggerSource terminology)
  //
  //         TOB (Trigger On Both ) =  decision && !tis && !tos  
  //           

  // ------------ single selection summary TisTos
  virtual void selectionTisTos( std::string selectionName,  
                                bool & decision, bool & tis, bool & tos ) = 0; 

   // do not use shortcuts if calling the tool mutiple times with the same offline input 
   void selectionTisTos( const LHCb::Particle & particle,
                                std::string selectionName,  
                                bool & decision, bool & tis, bool & tos )
     { setOfflineInput(particle);  selectionTisTos(selectionName,decision,tis,tos); }

   void selectionTisTos( const LHCb::ProtoParticle & protoParticle,
                                std::string selectionName,  
                                bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(protoParticle);  selectionTisTos(selectionName,decision,tis,tos); }

   void selectionTisTos( const LHCb::Track & track,
                                std::string selectionName,  
                                bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(track);  selectionTisTos(selectionName,decision,tis,tos); }

   void selectionTisTos( const std::vector<LHCb::LHCbID> hitlist,
                                std::string selectionName,  
                                bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(hitlist);  selectionTisTos(selectionName,decision,tis,tos); }

  // ------------ multiple selections summary TisTos
  virtual void selectionTisTos( std::vector< std::string > selectionNames,
                                bool & decision, bool & tis, bool & tos ,
                                bool selectionOR = true) = 0;  // set to false if an AND between selections is required

   // do not use shortcuts if calling the tool mutiple times with the same offline input 
   void selectionTisTos( const LHCb::Particle & particle,
                                std::vector< std::string > selectionNames, 
                                bool & decision, bool & tis, bool & tos )
     { setOfflineInput(particle);  selectionTisTos(selectionNames,decision,tis,tos); }

   void selectionTisTos( const LHCb::ProtoParticle & protoParticle,
                                std::vector< std::string > selectionNames, 
                                bool & decision, bool & tis, bool & tos ) 
    { setOfflineInput(protoParticle);  selectionTisTos(selectionNames,decision,tis,tos); }


   void selectionTisTos( const LHCb::Track & track,
                                std::vector< std::string > selectionNames, 
                                bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(track);  selectionTisTos(selectionNames,decision,tis,tos); }

   void selectionTisTos( const std::vector<LHCb::LHCbID> hitlist,
                                std::vector< std::string > selectionNames, 
                                bool & decision, bool & tis, bool & tos ) 
     { setOfflineInput(hitlist);  selectionTisTos(selectionNames,decision,tis,tos); }
  
  // ------------ auxiliary output:  list of LHCbIDs corresponding to present offline input 
  //     ( which hits are used in TisTos-ing depends on implementation and/or tool setting!)
  virtual std::vector<LHCb::LHCbID> offlineLHCbIDs()  = 0; 

   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::Particle & particle)
     { setOfflineInput(particle); return offlineLHCbIDs(); }
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::ProtoParticle & protoParticle)
     { setOfflineInput(protoParticle); return offlineLHCbIDs(); }
   std::vector<LHCb::LHCbID> offlineLHCbIDs(const LHCb::Track & track)
     { setOfflineInput(track); return offlineLHCbIDs(); }    
  
  // ------------  additional functionality:  lists of tracks/vertices/particles from selection summary
  //               satisfying TOS, ordered according to TOS quality (best first)
  //               return empty vector in case of a mismatch between the output type and the selection summary
  virtual std::vector<const LHCb::Track*>     matchedTOSTracks( std::string selectionName ) = 0;
  virtual std::vector<const LHCb::RecVertex*> matchedTOSVertices( std::string selectionName ) = 0;
  virtual std::vector<const LHCb::Particle*>  matchedTOSParticles( std::string selectionName ) = 0;

   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::Track & track, std::string selectionName )
     { setOfflineInput(track); return  matchedTOSTracks(selectionName); }
   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::ProtoParticle & protoParticle, std::string selectionName )
     { setOfflineInput(protoParticle); return  matchedTOSTracks(selectionName); }
   std::vector<const LHCb::Track*> matchedTOSTracks(const LHCb::Particle & particle, std::string selectionName )
     { setOfflineInput(particle); return  matchedTOSTracks(selectionName); }
   std::vector<const LHCb::Track*> matchedTOSTracks( const std::vector<LHCb::LHCbID> & hitlist,  std::string selectionName )
     { setOfflineInput(hitlist); return  matchedTOSTracks(selectionName); }

   std::vector<const LHCb::RecVertex*> matchedTOSVertices(const LHCb::Particle & particle, std::string selectionName )
     { setOfflineInput(particle); return  matchedTOSVertices(selectionName); }
   std::vector<const LHCb::RecVertex*> matchedTOSVertices(const std::vector<LHCb::LHCbID> & hitlist, std::string selectionName )
     { setOfflineInput(hitlist); return  matchedTOSVertices(selectionName); }

   std::vector<const LHCb::Particle*>  matchedTOSParticles(const LHCb::Particle & particle, std::string selectionName )
     { setOfflineInput(particle); return  matchedTOSParticles(selectionName); }
   std::vector<const LHCb::Particle*>  matchedTOSParticles(const std::vector<LHCb::LHCbID> & hitlist, std::string selectionName )
     { setOfflineInput(hitlist); return  matchedTOSParticles(selectionName); }


protected:

private:

};
#endif // ITRIGGERSELECTIONTISTOS_H
