#ifndef LBALPGEN_ALPGENPRODUCTION_H 
#define LBALPGEN_ALPGENPRODUCTION_H 1

// Include files
#include "LbPythia/PythiaProduction.h"

/** @class AlpGenProduction AlpGenProduction.h 
 *  
 *  Interface tool to produce events with AlpGen
 * 
 *  @author Stephane Tourneur
 *  @author Patrick Robbe
 *  @date   2012-07-13
 */

class AlpGenProduction : public PythiaProduction {
 public:
  /// Standard constructor
  AlpGenProduction( const std::string & type , 
                    const std::string & name ,
                    const IInterface * parent ) ;
  
  virtual ~AlpGenProduction( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;


 protected:

 private:
  void generateWeightedEvents( ) ;
  void generateUnweightedEvents( ) ;
};
#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
