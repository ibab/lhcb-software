// $Id: InitRndmTool.h,v 1.1 2005-12-14 13:22:21 cattanem Exp $
#ifndef INITRNDMTOOL_H 
#define INITRNDMTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRndmTool.h"            // Interface

// Forward declarations
class IRndmEngine;
class IRndmGenSvc;


/** @class InitRndmTool InitRndmTool.h
 *  
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-14
 */
class InitRndmTool : public GaudiTool, virtual public IRndmTool {
public: 
  /// Standard constructor
  InitRndmTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~InitRndmTool( ); ///< Destructor

  StatusCode initialize();   ///< Tool initialization

  /// Initialize with an unsigned int and an unsigned 64 bit seed
  void initRndm( unsigned int, ulonglong );

protected:

private:
  /// Property to skip some random numbers (default is zero)
  int  m_skipFactor;

  /// Property to use only one seed (default is false)
  bool m_singleSeed;

  // Member data
  IRndmEngine*  m_engine;   ///< Pointer to random number engine
  IRndmGenSvc*  m_randSvc;  ///< Pointer to random number service
};
#endif // INITRNDMTOOL_H
