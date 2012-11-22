// $Id: MCDecayTreeTuple.h,v 1.2 2009-02-19 13:50:41 pkoppenb Exp $
#ifndef MCDECAYTREETUPLE_H
#define MCDECAYTREETUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DecayTreeTupleBase/OnePart.h"
#include "DecayTreeTupleBase/DecayTreeTupleBase.h"

/** @class MCDecayTreeTuple MCDecayTreeTuple.h jborel/DecayTreeTuple.h
 *
 * \brief This is the new version of the so-called DecayChainNTuple
 *
 * \section basics (Basic usage:)
 *
 * MCDecayTreeTuple has a decay descriptor (given by the Decay
 * property). One line of the Tuple if filled for each reconstructed
 * candidate. This algorithm is NOT backward compatible with
 * DecayChainNTuple. Yet the base syntax is quite close.
 *
 * Here is a minimalist but working example:
 * \verbatim
 from Configurables import MCDecayTreeTuple, GaudiSequencer
 MyMCDecayTreeTuple = MCDecayTreeTuple("MyMCDecayTreeTuple")
 GaudiSequencer("MySeq").Members =+ [ MyMCDecayTreeTuple]
 MyMCDecayTreeTuple.Decay = "B0 -> ^pi- ^pi+"
 MyMCDecayTreeTuple.TupleName = "MyTuple"\endverbatim
 *
 * \note Any particle to be stored in the Tuple has to be flagged with
 * '^' (an exception here is the decay head which cannot be flagged as
 * DecayFinder will refuse it. Top level particle are therefore always
 * stored).

 * \sa DecayTreeTuple IEventTupleTool IMCParticleTupleTool
 *
 * \author Jeremie Borel with the help of Patrick and lhcb-davinci@cern.ch
 * \date 2007-11-01
 */
class MCDecayTreeTuple : public DecayTreeTupleBase
{

public:

  /// Standard constructor
  MCDecayTreeTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCDecayTreeTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

};

#endif // MCDECAYTREETUPLE_H
