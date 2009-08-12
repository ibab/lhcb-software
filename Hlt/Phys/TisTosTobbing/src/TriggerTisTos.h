// $Id: TriggerTisTos.h,v 1.9 2009-08-12 21:39:19 graven Exp $
#ifndef TRIGGERTISTOS_H 
#define TRIGGERTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TriggerSelectionTisTos.h"
#include "Kernel/IANSvc.h"

/** @class TriggerTisTos TriggerTisTos.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-20
 *
 *  Hit based implementation of Tis,Tos'ing Trigger specified via trigger name pattern.
 *  @sa  TriggerSelectionTisTos for inherited member functions (e.g. to define Offline Input).
 *  @sa  ITriggerTisTos docs for more explanation.
 *  This interface also defines inlined shortcuts to set Offline Input and or Trigger Input and get an output in one call. 
 */
class TriggerTisTos : public TriggerSelectionTisTos, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  TriggerTisTos( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TriggerTisTos( ); ///< Destructor

  virtual StatusCode         initialize(); 

  /// erase previous Trigger Input 
  void setTriggerInput( );

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  void addToTriggerInput( const std::string & selectionNameWithWildChar);
  
  /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  std::vector< std::string > triggerSelectionNames( unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement      = kAnything,
                                                    unsigned int tosRequirement      = kAnything );

  /// list of HltObjectSummaries from Selections satisfying TOS,TIS requirements (define Trigger and Offline Input before calling)
  std::vector<const LHCb::HltObjectSummary*> hltObjectSummaries( unsigned int tisRequirement      = kAnything,
                                                                 unsigned int tosRequirement      = kAnything );
  
   /// calculate decision,Tis,Tos  (for previously defined Offline and Trigger Inputs)
  void triggerTisTos( bool & decision, bool & tis, bool & tos);
  
private:

  /// obtain all known trigger names
  void getTriggerNames();

  static const std::vector< std::string > m_empty_selections;

  /// content of Trigger Input (list of trigger selection names) 
  std::vector< std::string > m_triggerInput_Selections;

  /// all known trigger names 
  std::vector< std::string > m_triggerNames;


  /// HltANNSvc for valid trigger names
  IANSvc* m_hltANSvc;  

};
#endif // TRIGGERTISTOS_H
