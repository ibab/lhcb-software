// $Id: TupleToolProtopinfo.h,v 1.1 2010-05-21 17:03:11 odescham Exp $
#ifndef _TupleToolANNPID_H
#define _TupleToolANNPID_H 1

// STL
#include <string>
#include <vector>

// Gaudi
#include "GaudiKernel/ToolFactory.h"

// Base class
#include "DecayTreeTupleBase/TupleToolBase.h"

// Interfaces
#include "Kernel/IParticleTupleTool.h"
#include "RecInterfaces/IChargedProtoANNPIDTool.h"

//============================================================================

class TupleToolANNPID : public TupleToolBase,
                        virtual public IParticleTupleTool
{

public:

  // Standard constructor
  TupleToolANNPID( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);
  
  virtual ~TupleToolANNPID() {}

  StatusCode initialize(); ///< Initialise

public:

  virtual StatusCode fill( const LHCb::Particle*,
                           const LHCb::Particle*,
                           const std::string& ,
                           Tuples::Tuple& );

private:

  /// Pointer to the ANNPID tool
  const ANNGlobalPID::IChargedProtoANNPIDTool * m_pidTool = nullptr;

  /// The ANNPID tunes to fill
  std::vector<std::string> m_pidTunes;
  
};

#endif // _TupleToolANNPID_H
