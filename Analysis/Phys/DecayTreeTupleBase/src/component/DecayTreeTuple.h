// $Id: DecayTreeTuple.h,v 1.10 2009-02-19 13:50:41 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLE_H
#define JBOREL_DECAYTREETUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DecayTreeTupleBase/OnePart.h"
#include "DecayTreeTupleBase/DecayTreeTupleBase.h"

/** @class DecayTreeTuple DecayTreeTuple.h jborel/DecayTreeTuple.h
 *
 * \brief This is the new version of the so-called DecayChainNTuple
 *
 * \section basics (Basic usage:)
 * 
 * DecayTreeTuple has a decay descriptor (given by the Decay
 * property). One line of the Tuple if filled for each reconstructed
 * candidate. This algorithm is NOT backward compatible with
 * DecayChainNTuple. Yet the base syntax is quite close.
 *
 * Here is a minimalist but working example:
 * \verbatim
 from Configurables import DecayTreeTuple, PhysDesktop, GaudiSequencer
 MyDecayTreeTuple = DecayTreeTuple("MyDecayTreeTuple")
 GaudiSequencer("MySeq").Members =+ [ MyDecayTreeTuple]
 MyDecayTreeTuple.addTool(PhysDesktop())
 MyDecayTreeTuple.PhysDesktop.InputLocations = ["SomeSelection"]
 MyDecayTreeTuple.Decay = "B0 -> ^pi- ^pi+"
 MyDecayTreeTuple.TupleName = "MyTuple"\endverbatim
 *
 * Do not forget to give the context if you want to use Hlt2 particles:
 *\verbatim
MyDecayTreeTuple.PhysDesktop.InputLocations = ["Hlt2SelB2HH"]
MyDecayTreeTuple.Context = "HLT" \endverbatim       
 *
 * \note Any particle to be stored in the Tuple has to be flagged with
 * '^' (an exception here is the decay head which cannot be flagged as
 * DecayFinder will refuse it. Top level particle are therefore always
 * stored).
 * 
 * \section principle How it works:
 *
 * DecayTreeTuple basically owns a set of tools which all implement
 * either the IEventTupleTool or the IParticleTupleTool
 * interfaces. For every reconstructed candidates, it successively
 * call all the tools:
 *
 * - Once for each matched particles for the IParticleTupleTool instances
 *
 * - Once for the candidate for the IEventTupleTool instances
 *
 * The instantiated tool list is defined by the \b ToolList property,
 * a vector of strings. Hence any tool can be added and manipulated as
 * usual, doing things like
 *
 * \verbatim
 MyDecayTreeTuple.ToolList = [ "TupleToolMCTruth", TupleToolTrigger" ]
 MyDecayTreeTuple.ToolList += [ "TupleToolPid/genericPID" ]
 from Configurables import TupleTooPid
 MyDecayTreeTuple.addTool(TupleTooPid("genericPID"))
 MyDecayTreeTuple.genericPID.SomeProperty = True\endverbatim
 *
 * \subsection branches Fine tuning of the branches:
 *
 * The \b Branches property, is a map<string,string>. Each of its
 * entry associates a name with decay descriptor matching a specific
 * part of the main decay.
 *
 * Once an entry exists, specific tools for this entry can be
 * added. They will only act on the particles matched by the
 * associated decay AND the main decay. Specific tools have their own
 * properties, whether or not the tool already exists in the generic
 * tool list.
 *
 * Specific tools are instance of TupleToolDecay, they have a few properties:
 *
 * - ToolList, a vector of string, defining the tool list obviously.
 *
 * - InheritTools, whether or not generic tools should also be applied
 *   to this sub-decay.
 *
 * Here is an example with Bs->Ds h, h as K or pi, Ds => K K pi (a
 * good occasion to remind you the DecayFinder syntax):
 *
 * \verbatim
 Dtt = DecayTreeTuple("Dtt")
 GaudiSequencer("MySeq").Members = [ Dtt ]
 Dtt.Decay = "[[B_s0]cc -> (^D_s- => ^K+ ^K- ^pi-) {^pi+, ^K+}]cc"
 Dtt.Branches = {
    "bachelor" : " [[B_s0]cc -> D_s- {^pi+, ^K+}]cc "
    ,"Ds" : " [ [B_s0]cc -> ^D_s- {pi+,K+} ]cc "
    ,"head" : " [B_s0]cc : [[B_s0]cc -> (D_s- => K+ K- pi- ) pi+ ]cc"
 }

 Dtt.head.ToolList = [ "TupleToolPropertime" ]
 Dtt.head.InheritTools = False
 from Configurables import TupleToolMCTruth, TupleToolDecay
 Dtt.addTool(TupleToolDecay("bachelor"))
 Dtt.bachelor.addTool(TupleToolMCTruth())
 Dtt.bachelor.ToolList = [ "TupleToolMCTruth" ]

 Dtt.bachelor.TupleToolMCTruch.UseChi2Method = True\endverbatim
 *
 * \note Notice the way the decay head is matched as it is a bit unusual.
 *
 * \subsection names Naming the tuple column:
 * 
 * The following rules applies to name the tuple columns:
 *<ol>
 * 
 * <li>By default, the name of the particles from the first matched
 * candidate are taken to prefix the column names (after having
 * sanitized some ugly chars).
 *
 * <li>If a specific decay is given for some particles, the first
 * argument of the \b Branches property is taken as prefix. This is not
 * true anymore if \b UseToolNameForBranchName is set to false.
 *
 * <li>For the nostalgics of DecayChainNTuple, the \b UseLabXSyntax
 * allows to prefix the branches with the good old \em labX style. Yet it
 * will be prefixed, not post fixed.
 *
 * </ol>
 *
 * \sa TupleToolDecay IEventTupleTool IParticleTupleTool
 *
 * \author Jeremie Borel with the help of Patrick and lhcb-davinci@cern.ch
 * \date 2007-11-01 
 */
class DecayTreeTuple : public DecayTreeTupleBase  
{

 public:

  /// Standard constructor
  DecayTreeTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecayTreeTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

};

#endif // JBOREL_DECAYTREETUPLE_H
