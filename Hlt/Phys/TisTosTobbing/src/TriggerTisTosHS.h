// $Id: TriggerTisTosHS.h,v 1.1 2008-11-14 06:55:39 tskwarni Exp $
#ifndef TRIGGERTISTOS_H 
#define TRIGGERTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TriggerSelectionTisTosHS.h"
#include "Kernel/IANSvc.h"

namespace LHCb {
  class HltSummary;
};


/** @class TriggerTisTosHS TriggerTisTosHS.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2007-08-20
 *
 *  Hit based implementation of Tis,Tos'ing Trigger specified via trigger name pattern.
 *  @sa  TriggerSelectionTisTosHS for inherited member functions (e.g. to define Offline Input).
 *  @sa  ITriggerTisTos docs for more explanation.
 *  This interface also defines inlined shortcuts to set Offline Input and or Trigger Input and get an output in one call. 
 */
class TriggerTisTosHS : public TriggerSelectionTisTosHS, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  TriggerTisTosHS( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~TriggerTisTosHS( ); ///< Destructor

  virtual StatusCode         initialize(); 

  /// must be called by user before Trigger Input calls if the trigger structure may have changed since the previous call
  void refreshTriggerStructure();

  /// erase previous Trigger Input 
  void setTriggerInput( );

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  void addToTriggerInput( const std::string & selectionNameWithWildChar );
  
  /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  std::vector< std::string > triggerSelectionNames( unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement      = kAnything,
                                                    unsigned int tosRequirement      = kAnything );
  
  /// list of HltObjectSummaries from Selections satisfying TOS,TIS requirements (define Trigger and Offline Input before calling)
  std::vector<const LHCb::HltObjectSummary*> hltObjectSummaries( unsigned int tisRequirement      = kAnything,
                                                                 unsigned int tosRequirement      = kAnything );
  
   /// calculate decision,Tis,Tos  (for previously defined Offline and Trigger Inputs)
  void triggerTisTos( bool & decision, bool & tis, bool & tos);
  
  /** @par wild character name matching functions:
   *  @par
   *     both "*" (matching any number of characters including none) and "?" (matching exactly one character) are wild 
   *  @par
   *     they can appear multiply times (or not at all), including beginning and and of the string
   *  @par
   *     returns 0 for no match; 1 for match
   */  
  /// function to do wild character matching
  inline static int wildcmp(const char *wild, const char *string) {
  // Written by Jack Handy 
  const char *cp = NULL, *mp = NULL;
  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }
  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }
  while (*wild == '*') {
    wild++;
  }
  return !*wild;
  }
  
protected:

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
