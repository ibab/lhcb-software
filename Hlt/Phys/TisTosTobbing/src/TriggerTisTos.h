// $Id: TriggerTisTos.h,v 1.3 2007-09-03 15:35:09 pkoppenb Exp $
#ifndef TRIGGERTISTOS_H 
#define TRIGGERTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TriggerSelectionTisTos.h"
#include "HltBase/HltConfigurationHelper.h"

namespace LHCb {
  class HltSummary;
};


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

  /// must be called by user before Trigger Input calls if the trigger structure may have changed since the previous call
  void refreshTriggerStructure();

  /// erase previous Trigger Input 
  void setTriggerInput( );

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  void addToTriggerInput( const std::string & selectionNameWithWildChar,
                          bool alleyExitsOnly = true);

  /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  std::vector< std::string > triggerSelectionNames( unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement      = kAnything,
                                                    unsigned int tosRequirement      = kAnything, 
                                                    bool checkInputSelections = kDefaultInputSelectionsChecking );

  ///  list of input Trigger Selections to given trigger selection (notice no wild characters allowed in the selection name)
  const std::vector< std::string > & inputTriggerSelectionNames( const std::string & selectionNameWithNoWildChar );
  
   /// calculate decision,Tis,Tos  (for previously defined Offline and Trigger Inputs)
  void triggerTisTos( bool & decision, bool & tis, bool & tos, bool checkInputSelections = kDefaultInputSelectionsChecking);
  
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

  /// obtain Alley Exit selection names from HltConfiguration
  void getAlleyExitSelections();  

  /// check this input selection (and its inputs) and update tis,tos (previousTis,previousTos - status of tis,tos search)
  void recursiveInputTriggerTisTos( const std::string & selName,  bool & tis,  bool & tos, bool previousTis, bool previousTos);

  static const std::vector< std::string > m_empty_selections;

  /// error report count (to limit error messages)
  int m_error_count;

  /// list of Trigger Selection Names to omit when checking Input Selections (can be changed from options)
  std::vector< std::string > m_InputSelectionsToIgnore;

  /// content of Trigger Input (list of trigger selection names) 
  std::vector< std::string > m_triggerInput_Selections;

  /// names of alley exit selection from HltConfiguration
  std::vector< std::string > m_alleyExitSelections;

};
#endif // TRIGGERTISTOS_H
