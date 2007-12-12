// $Id: TupleToolMCBackgroundInfo.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLMCBACKGROUNDINFO_H
#define JBOREL_TUPLETOOLMCBACKGROUNDINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IBackgroundCategory;

namespace LHCb{
  class Particle;
};

/** @class TupleToolMCBackgroundInfo TupleToolMCBackgroundInfo.h jborel/TupleToolMCBackgroundInfo.h
 *
 * \brief Fill the info from IBackgroundCategory
 *
 * An answer is only filled for composite particles.
 * 
 * head_BKGCAT : category.
 * 
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolMCBackgroundInfo : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolMCBackgroundInfo( const std::string& type,
			     const std::string& name,
			     const IInterface* parent);

  virtual ~TupleToolMCBackgroundInfo( ){}; ///< Destructor

  virtual StatusCode initialize();

  StatusCode fill( const LHCb::Particle*
		   , const LHCb::Particle*
		   , const std::string&
		   , Tuples::Tuple& );

private:
  IBackgroundCategory* m_bkg;
};
#endif // JBOREL_TUPLETOOLMCBACKGROUNDINFO_H
