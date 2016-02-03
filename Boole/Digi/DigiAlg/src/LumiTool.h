// $Id: LumiTool.h,v 1.6 2010-04-26 15:30:37 cattanem Exp $ 
#ifndef LUMITOOL_H
#define LUMITOOL_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"
#include "ILumiTool.h"

// Forward declarations
class IRndmGenSvc;
class IDataProviderSvc;


/** @class LumiTool LumiTool.h
 *
 *  Determines probability of interactions for a given luminosity and
 *  returns a number of interactions according to this probability.
 *
 *  The instantaneous luminosity is taken from /Event/Gen/Header
 *  The beam crossing rate and total cross-section are taken from beam.cdf
 *
 *  @author Marco Cattaneo
 *  @date   2003-09-29
 */

class LumiTool : public GaudiTool, 
                 virtual public ILumiTool {

public:

  // Standard Constructor
  LumiTool( const std::string& type, 
            const std::string& name, 
            const IInterface* parent );

  // Standard Destructor
  ~LumiTool() override = default;

  /// standard methods
  StatusCode initialize();

  /// Return number of interactions for given luminosity 
  StatusCode numInteractions( int& number );

private:
  double m_bunchCrossRate; ///< bunch crossing rate (MHz)
  double m_totalXSection;  ///< total cross-section (mbarn)

  SmartIF<IRndmGenSvc> m_randSvc;        ///< pointer to random numbers service
};

#endif // LUMITOOL_H
