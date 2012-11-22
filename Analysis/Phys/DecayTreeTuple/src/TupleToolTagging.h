// $Id: TupleToolTagging.h,v 1.5 2010-01-27 11:43:50 rlambert Exp $
#ifndef JBOREL_TUPLETOOLTAGGING_H
#define JBOREL_TUPLETOOLTAGGING_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IDVAlgorithm;
class IBTaggingTool;

/** @class TupleToolTagging TupleToolTagging.h jborel/TupleToolTagging.h
 *
 * \brief Fill the tagging information in the tuple.
 *
 * There are many taggers for whom there is an enum, and maybe even code,
 * but are not active at the moment.
 * The list of active taggers can be defined here with the property ActiveTaggers
 *
 * Typically however these are 
 * - ("OS_Muon");
 * - ("OS_Electron");
 * - ("OS_Kaon");
 * - ("SS_Kaon");
 * - ("SS_Pion");
 * - ("VtxCharge");
 * 
 * Since only one of SS_Kaon and SS_Pion will fire at any on time, these typical
 * cases are packed into a single variable: head+_TAGGER
 *
 *  @code
 *   if(tdec) switch ( taggers[i].type() ) {
 *   case Tagger::OS_Muon     : taggers_code +=  10000 *(tdec+2); break;
 *   case Tagger::OS_Electron : taggers_code +=   1000 *(tdec+2); break;
 *   case Tagger::OS_Kaon     : taggers_code +=    100 *(tdec+2); break;
 *   case Tagger::SS_Kaon     : taggers_code +=     10 *(tdec+2); break;
 *   case Tagger::SS_Pion     : taggers_code +=     10 *(tdec+2); break;
 *   case Tagger::VtxCharge   : taggers_code +=      1 *(tdec+2); break;
 *     
 *   }
 *  @endcode
 * 
 * If the list of active taggers is different, you will need to switch to Verbose mode
 *
 * if Verbose is set the probability and decision for each tagger will be filled.
 * - head+tagger+DEC
 * - head+tagger+PROB
 * 
 * @see  BTaggingTool
 * @see  Tagger
 *
 * @author Jeremie Borel
 * @date   2007-11-07
 */
class TupleToolTagging : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolTagging( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

  virtual ~TupleToolTagging(){}; ///< Destructor

  virtual StatusCode initialize();
  
  virtual StatusCode fill( const LHCb::Particle*, const LHCb::Particle*
			   , const std::string&, Tuples::Tuple& );


private:

  std::string m_inputLocation, m_toolName;
  IDVAlgorithm* m_dva;
  IBTaggingTool* m_tagging;
  
  std::map<int, std::string> m_tagger_map; ///<map of enum to string
  std::map<std::string, int> m_tagger_rmap;///<reverse, map of string to enum
  
  std::vector<std::string> m_activeTaggers; ///<List of active taggers
  //  bool m_extendedTagging;
};

#endif // JBOREL_TUPLETOOLTAGGING_H
