// $Id: TupleToolP2VV.h,v 1.1 2007-12-18 18:12:26 pkoppenb Exp $
#ifndef TUPLETOOLP2VV_H 
#define TUPLETOOLP2VV_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
/** @class TupleToolP2VV TupleToolP2VV.h
 *  
 *  \brief Fills three angles as defined in IP2VVPartAngleCalculator.
 *
 * - head_ThetaL : angle of the L+ wrt B in dilepton frame
 * - head_ThetaK : angle of the K+ wrt B in dilepton frame
 * - head_Phi : angle of the K* and LL decay planes in dilepton frame
 *
 *  @author Patrick Koppenburg
 *  @date   2007-12-18
 */
class IP2VVPartAngleCalculator;

class TupleToolP2VV : public GaudiTool, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolP2VV( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TupleToolP2VV( ); ///< Destructor

  StatusCode initialize() ;

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

protected:
  
private:
  IP2VVPartAngleCalculator*   m_pB2LLKstar;

};
#endif // TUPLETOOLP2VV_H
