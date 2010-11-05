// $Id: TESTisTos.h,v 1.2 2010-07-21 21:22:16 tskwarni Exp $
#ifndef TESTISTOS_H 
#define TESTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITriggerTisTos.h"            // Interface

#include "TESSelectionTisTos.h"

/** @class TESTisTos TESTisTos.h
 *  
 *  @author Tomasz Skwarnicki
 *  @date   2009-12-03
 * 
 *  This is a special version of TisTosTobbing of TES Selections with candidates saved on TES
 *  This code is general enough to TisTos any Selection of Particles, Tracks, RecVertices saved on TES 
 *   with respect to user defined "Signal"   
 *
 *  Hit based implementation of Tis,Tos'ing TES specified via stripping name pattern.
 *  @sa  TESSelectionTisTos for inherited member functions (e.g. to define Offline Input).
 *  @sa  ITriggerTisTos docs for more explanation.
 *  This interface also defines inlined shortcuts to set Offline Input and or TES Input and get an output in one call. 
 */
class TESTisTos : public TESSelectionTisTos, virtual public ITriggerTisTos {
public: 
  /// Standard constructor
  TESTisTos( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TESTisTos( ); ///< Destructor

  virtual StatusCode         initialize(); 

  /// erase previous Trigger Input 
  void setTriggerInput( );

  /// add Trigger Selection Name pattern to Trigger Input; pattern may contain wild character *, all matches will be added
  void addToTriggerInput( const std::string & selectionNameWithWildChar);
  
   /// Complete classification of the Trigger Input (see ITisTis::TisTosTob for the meaning)
  unsigned int tisTosTrigger();

  /// check for TOS  - may be faster than using tisTosTrigger()
  bool tosTrigger(); 
 
  /// check for TIS  - may be faster than using tisTosTrigger()
  bool tisTrigger();

  /// check for TUS (Trigger Used Signal: TPS or TOS) - may be faster than using tisTosTrigger()
  bool tusTrigger();

  /// analysis report
  std::string analysisReportTrigger();

 /// returns Trigger Selection names matching optional pattern of decision,tis,tos for previously set Trigger and Offline Inputs
  std::vector< std::string > triggerSelectionNames( unsigned int decisionRequirement = kAnything, 
                                                    unsigned int tisRequirement      = kAnything,
                                                    unsigned int tosRequirement      = kAnything,
                                                    unsigned int tpsRequirement      = kAnything);

  /// list of HltObjectSummaries from Selections satisfying TOS,TIS requirements (define Trigger and Offline Input before calling)
  std::vector<const LHCb::HltObjectSummary*> hltObjectSummaries( unsigned int tisRequirement      = kAnything,
                                                                 unsigned int tosRequirement      = kAnything,
                                                                 unsigned int tpsRequirement      = kAnything );  
private:

  /// if true then warning about empty trigger inputs are printed
  bool m_trigInputWarn;
  /// flag to set if objects in TES are allowed to be preloaded
  bool m_preLoad;  

  /// obtain all known trigger names
  void getTriggerNames(const std::string & selectionNameWithWildChar);
  void exploreTES(const std::string & path, const std::string & preloadPath );  

  static const std::vector< std::string > m_empty_selections;

  /// content of Trigger Input (list of trigger selection names) 
  std::vector< std::string > m_triggerInput_Selections;

  /// all known trigger names 
  std::vector< std::string > m_triggerNames;

};
#endif // TESTISTOS_H
