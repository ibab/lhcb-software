// $Id: TupleToolBremInfo.h
#ifndef _TUPLETOOLBREMINFO_H
#define _TUPLETOOLBREMINFO_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
// Interfaces
#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IBremAdder.h"
#include <memory>


//============================================================================
class TupleToolBremInfo : public TupleToolBase, virtual public IParticleTupleTool {
//==========================================================================
public:
 // Standard constructor
  TupleToolBremInfo( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  StatusCode initialize();
  virtual ~TupleToolBremInfo( ){}; ///< Destructor
  
  virtual StatusCode fill(const LHCb::Particle*, const LHCb::Particle* 
			  ,  const std::string& ,Tuples::Tuple& );
private: 
  IBremAdder* m_adder;
  std::vector<std::string> m_parts;
  std::vector<long> m_pids;
};
#endif // _TUPLETOOLBREMINFO_H
