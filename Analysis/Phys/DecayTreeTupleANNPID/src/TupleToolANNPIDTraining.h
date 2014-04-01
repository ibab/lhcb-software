// $Id: TupleToolProtopinfo.h,v 1.1 2010-05-21 17:03:11 odescham Exp $
#ifndef _TupleToolANNPIDTraining_H
#define _TupleToolANNPIDTraining_H 1

// Gaudi
#include "GaudiKernel/ToolFactory.h"
//#include "GaudiAlg/Tuple.h"
//#include "GaudiAlg/TupleObj.h"

// Base class
#include "DecayTreeTupleBase/TupleToolBase.h"

// Interfaces
#include "Kernel/IParticleTupleTool.h"
#include "RecInterfaces/IChargedProtoANNPIDTupleTool.h"

//============================================================================

class TupleToolANNPIDTraining : public TupleToolBase,
                                virtual public IParticleTupleTool
{

public:

  // Standard constructor
  TupleToolANNPIDTraining( const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  virtual ~TupleToolANNPIDTraining() {}

  StatusCode initialize(); ///< Initialise

public:

  virtual StatusCode fill(const LHCb::Particle*,
                          const LHCb::Particle*,
                          const std::string& ,
                          Tuples::Tuple& );

private:

  /// Pointer to the ANNPID tuple tool
  const ANNGlobalPID::IChargedProtoANNPIDTupleTool * m_tuple;
  
};

#endif // _TupleToolANNPIDTraining_H
