// $Id: IEventTupleTool.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef DECAYTUPLE_IEVENTTUPLETOOL_H
#define DECAYTUPLE_IEVENTTUPLETOOL_H  1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IEventTupleTool ( "IEventTupleTool", 1, 0 );

namespace Tuples{
  class Tuple;
};
namespace LHCb {
  class RecVertex;
  class Particle;
};
class IPhysDesktop;
class DVAlgorithm;

// ==================================================
// ==================================================

/** @class IEventTupleTool
 *
 * \brief Base class interface for the tool used by the DecayTreeTuple
 * algorithm
 *
 * The fill method gets called once for every candidates. It gets the
 * tuple to fill as argument.
 *
 * The tuple can be filled with anything that Tuples::Tuple
 * instances can accept, yet the following usages are strongly advised:
 *
 * - Dont call the tuple->write() methode as this result in a
 undefined behavior.
 * 
 * - Avoid filling column with useless values like the propertime of a
 * pions
 *
 * - If you fill column only in the case some condition are verified,
 * be sure either to always fill it or never do so. If an ambiguity
 * comes in the decision for some column, returns a
 * StatusCode::FAILURE. This will prevent the whole candidate to be
 * written to the tuple.
 *
 * - Use meaningful column name and document them. If it happens that
 two columns have the same name, the job will raise a fatal error.
 *
 * \sa DecayTreeTuple IParticleTupleTool
 * 
 * @author Jeremie Borel
 * @date November 2007
 */
class IEventTupleTool : virtual public IAlgTool {
 public:
  virtual ~IEventTupleTool(){};
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IEventTupleTool; }
  virtual StatusCode fill( Tuples::Tuple& )=0;
};



#endif // DECAYTUPLE_IEVENTTUPLETOOL_H
