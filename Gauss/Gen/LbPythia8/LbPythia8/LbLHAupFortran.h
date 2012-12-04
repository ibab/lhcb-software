#ifndef LBPYTHIA8_LBLHAUPFORTRAN_H 
#define LBPYTHIA8_LBLHAUPFORTRAN_H 1

// Include files
#include "LesHouches.h"
#include "LHAFortran.h"

class IProductionTool ;

/** @class LbLHAupFortran LbLHAupFortran.h LbPythia8/LbLHAupFortran.h
 *  
 *  Base class for interfaces to Pythia Fortran User Processes
 *
 *  @author Patrick Robbe
 *  @date   2012-11-30
 */
class LbLHAupFortran : public Pythia8::LHAup {
public: 
  /// Standard constructor
  LbLHAupFortran( ); 

  virtual ~LbLHAupFortran( ); ///< Destructor

  /// Initialization part
  bool setInit() ;

  /// Generate one event
  bool setEvent( int idProcIn = 0 ) ;

  /// Set production tool
  void setProductionTool( IProductionTool * tool )  {
    m_productionTool = tool ;
  }
  
private:
  // Save beam energies
  double eBeamA , eBeamB ;

  // initialization routine
  virtual bool fillHepRup( ) ;
  
  // event routine
  virtual bool fillHepEup() ;

  // production tool
  IProductionTool * m_productionTool ;
};
#endif // LBPYTHIA8_LBLHAUPFORTRAN_H
