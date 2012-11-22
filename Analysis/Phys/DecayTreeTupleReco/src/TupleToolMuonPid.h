// $Id: TupleToolMuonPid.h,v 1.4 2010-03-04 14:02:03 xieyu Exp $
#ifndef TUPLETOOLMUONID_H
#define TUPLETOOLMUONID_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolMuonPid TupleToolMuonPid.h
 *
 *  @author Kazu Akiba
 *  @date   2011-09-01
 *
 */
class TupleToolMuonPid : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolMuonPid( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  virtual ~TupleToolMuonPid(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*, const LHCb::Particle*
                           , const std::string&, Tuples::Tuple& );

private:
};
#endif // TUPLETOOLMUONID_H
