// $Id: RandomForGenerator.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef GENERATORS_RANDOMFORGENERATOR_H 
#define GENERATORS_RANDOMFORGENERATOR_H 1

// Include files
#include "GaudiKernel/RndmGenerators.h"

/** @class RandomForGenerator RandomForGenerator.h Generators/RandomForGenerator.h
 *  Simple class to interface with Gaudi Random generator
 *
 *  @author Patrick Robbe
 *  @date   2007-10-10
 */
class RandomForGenerator {
public:
  /// Standard constructor
  //  RandomForGenerator( ){ } ; 

  //  virtual ~RandomForGenerator( ) {} ; ///< Destructor

  static Rndm::Numbers & getNumbers() ;

protected:

private:
  static Rndm::Numbers s_randgaudi ;
};
#endif // GENERATORS_RANDOMFORGENERATOR_H
