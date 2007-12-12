// $Id: IParticleTupleTool.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef DECAYTUPLE_IPARTICLETUPLETOOL_H
#define DECAYTUPLE_IPARTICLETUPLETOOL_H  1

// Include files
// from STL
#include <string>

// from Gaudi
// #include "Event/Particle.h"
// #include "Event/MCParticle.h"
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IParticleTupleTool ( "IParticleTupleTool", 1, 0 );

namespace Tuples{
  class Tuple;
};
namespace LHCb {
  class RecVertex;
  class Particle;
};
class IPhysDesktop;
class DVAlgorithm;

/** @class IParticleTupleTool
 *
 * \brief Base class interface for the tool used by the DecayTreeTuple
 * algorithm
 *
 * The fill method gets called for every particles of every
 * candidates. It has the following arguments:
 *
 * - \b top is a pointer to the top of the reconstructed decay. It
 allows to see where \b part is located in the structure.

 * - \b part is the particle to consider.
 *
 * - \b head is a prefix that should be applied to the column name
     (e.g. tuple->column( head+"_PT", p->pt() ) );
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
 * comes in the decision for some candidates, returns a
 * StatusCode::FAILURE. This will prevents the whole candidate to be
 * written to the tuple.
 *
 * - Use the head as prefix if the information you stored is indeed
 related to the particle. Nothing prevents you not doing so. If it
 happens that two columns have the same name, the job will raise a
 fatal error.
 *
 * - tulpe column names are in principle composed of the head, a '_'
 and a variable name. Stick as much as possible to 
 * https://twiki.cern.ch/twiki/bin/view/LHCb/LoKiParticleFunctions   
 *
 *
 * \sa DecayTreeTuple IEventTupleTool
 * 
 * @author Jeremie Borel
 * @date November 2007
 */
class IParticleTupleTool : virtual public IAlgTool {
 public:
  virtual ~IParticleTupleTool(){};
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleTupleTool; }
  virtual StatusCode fill( const LHCb::Particle* top
			   , const LHCb::Particle* part
			   , const std::string& head
			   , Tuples::Tuple& tuple )=0;
};

#endif // DECAYTUPLE_IPARTICLETUPLETOOL_H
