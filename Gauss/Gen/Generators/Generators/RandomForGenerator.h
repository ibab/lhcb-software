// $Id: RandomForGenerator.h,v 1.1 2007-10-10 20:06:47 robbep Exp $
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

  static Rndm::Numbers & getNumbers() { return s_randgaudi ; }

protected:

private:
  static Rndm::Numbers s_randgaudi ;
};
#endif // GENERATORS_RANDOMFORGENERATOR_H
