// $Id: TupleToolMCPid.h,v 1.2 2008-01-07 17:25:25 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLMCPID_H
#define JBOREL_TUPLETOOLMCPID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
/** @class TupleToolMCPid TupleToolMCPid.h jborel/TupleToolMCPid.h
 *
 *  - head_MCPID : the true PID of the particle. 
 *  - head_MCWEIGHT : the weight of the MC assoc in case of chi2 method.
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolMCPid : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolMCPid( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolMCPid(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

  virtual StatusCode initialize();
  //  virtual StatusCode finalize();

private:

  //  const GaudiAlgorithm* getParent() const ;

  Particle2MCLinker *m_pLink;
  Particle2MCLinker *m_pComp;
  Particle2MCLinker *m_pChi2;

  std::vector<std::string> m_assocInputs;
  bool m_useChi2Method;
};

#endif // JBOREL_TUPLETOOLMCPID_H
