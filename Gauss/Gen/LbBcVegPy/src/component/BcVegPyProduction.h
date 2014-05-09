// $Id: BcVegPyProduction.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $
#ifndef LBBCVEGPY_BCVEGPYPRODUCTION_H 
#define LBBCVEGPY_BCVEGPYPRODUCTION_H 1

// Include files
#include "LbPythia/PythiaProduction.h"

/** @class BcVegPyProduction BcVegPyProduction.h 
 *  
 *  Interface tool to produce events with BcVegPy
 * 
 *  @author hejb & yangzw
 *  @date   2006-03-02
 */

class BcVegPyProduction : public PythiaProduction {
 public:
  /// Standard constructor
  BcVegPyProduction( const std::string & type , 
		   const std::string & name ,
                   const IInterface * parent ) ;
  
  virtual ~BcVegPyProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;


 protected:

  /// Parse BcVegPy commands from a string vector
  StatusCode parseBcVegPyCommands( const CommandVector & theVector ) ;


 private:

  CommandVector m_defaultBcVegPySettings ;
  CommandVector m_commandBcVegPyVector ;      ///< Commands to setup BcVegPy
};
#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
