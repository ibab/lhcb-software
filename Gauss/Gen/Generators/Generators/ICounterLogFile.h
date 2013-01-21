#ifndef GENERATORS_ICOUNTERLOGFILE_H 
#define GENERATORS_ICOUNTERLOGFILE_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ICounterLogFile ( "ICounterLogFile", 1, 0 );

/** @class ICounterLogFile ICounterLogFile.h Generators/ICounterLogFile.h
 *  
 *
 *  @author Patrick Robbe
 *  @date   2013-01-15
 */
class ICounterLogFile : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICounterLogFile; }

  virtual void addEfficiency( const std::string & name , const unsigned int after , 
                              const unsigned int before , const double fraction , 
                              const double err_fraction ) = 0 ;

  virtual void addCounter( const std::string & name , const unsigned int value ) = 0 ; 

  virtual void addFraction( const std::string & name , const unsigned int number , 
                            const double fraction , const double err_fraction ) = 0 ;

  /// Add cross section value, in mb
  virtual void addCrossSection( const std::string & name , 
                                const unsigned int processId , 
                                const unsigned int number , 
                                const double value ) = 0 ;
  
protected:

private:

};
#endif // GENERATORS_ICOUNTERLOGFILE_H
