// $Id: TupleToolPropertime.h,v 1.3 2010-01-26 15:39:26 rlambert Exp $
#ifndef JBOREL_TUPLETOOLPROPERTIME_H
#define JBOREL_TUPLETOOLPROPERTIME_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IDVAlgorithm;
class ILifetimeFitter;

namespace LHCb
{
  class Particle;
  class Vertex;
}

/** @class TupleToolPropertime TupleToolPropertime.h jborel/TupleToolPropertime.h
 * \brief Fills the propertime for DecayTreeTuple
 *
 * Columns filled:
 * - head_TAU
 * - head_TAUERR
 * - head_TAUCHI2
 *
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolPropertime : public TupleToolBase,
                            virtual public IParticleTupleTool
{

public:

  /// Standard constructor
  TupleToolPropertime( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~TupleToolPropertime(){} ///< Destructor

  virtual StatusCode initialize();

  virtual StatusCode fill( const LHCb::Particle*, const LHCb::Particle*
                           , const std::string&, Tuples::Tuple& );

private:

  const LHCb::Vertex* originVertex( const  LHCb::Particle*
                                    , const LHCb::Particle* ) const;

private:

  IDVAlgorithm* m_dva;
  const ILifetimeFitter* m_fit;
  bool        m_fitToPV;

};

#endif // JBOREL_TUPLETOOLPROPERTIME_H
