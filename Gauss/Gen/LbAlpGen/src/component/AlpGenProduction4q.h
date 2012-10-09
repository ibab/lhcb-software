#ifndef LBALPGEN_ALPGENPRODUCTION4Q_H 
#define LBALPGEN_ALPGENPRODUCTION4Q_H 1

// Include files
#include "AlpGenProduction.h"

// Forward declarations
class IRndmGenSvc ;
class IRndmEngine ;

/** @class AlpGenProduction AlpGenProduction.h 
 *  
 *  Interface tool to produce events with AlpGen
 * 
 *  @author Stephane Tourneur
 *  @author Patrick Robbe
 *  @date   2012-07-13
 */




class AlpGenProduction4q : public AlpGenProduction {
 public:
  /// Standard constructor
  AlpGenProduction4q( const std::string & type , 
                    const std::string & name ,
                    const IInterface * parent ) ;
  
  virtual ~AlpGenProduction4q( ); ///< Destructor
  
  virtual StatusCode initialize( ) ;   ///< Initialize method

  virtual StatusCode finalize( ) ; ///< Finalize method
  
  virtual StatusCode generateEvent( HepMC::GenEvent * theEvent , 
                                    LHCb::GenCollision * theCollision ) ;

 protected:


};
#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
