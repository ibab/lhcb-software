#ifndef LBGENXICC_GENXICCPRODUCTION_H 
#define LBGENXICC_GENXICCPRODUCTION_H 1

// Include files
#include "LbGenXicc/QQqBaryons.h"
#include "LbPythia/PythiaProduction.h"

/** @class GenXiccProduction GenXiccProduction.h 
 *  
 *  Interface tool to produce events with GenXicc
 * 
 *  @author F. Zhang
 *  @date   2011-04-10
 */

class GenXiccProduction : public PythiaProduction , public QQqBaryons {
 public:
  /// Standard constructor
  GenXiccProduction( const std::string & type , 
		   const std::string & name ,
                   const IInterface * parent ) ;
  
  virtual ~GenXiccProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;


 protected:

  /// Parse GenXicc commands from a string vector
  StatusCode parseGenXiccCommands( const CommandVector & theVector, bool canChangeState = false ) ;


 private:
  std::string m_BaryonState;                  ///< baryon state to be produced
  double m_beamMomentum;                      ///< beam momentum
  CommandVector m_defaultGenXiccSettings ;
  CommandVector m_commandGenXiccVector ;      ///< Commands to setup GenXicc
};
#endif // LBGENXICC_GENXICCPRODUCTION_H
