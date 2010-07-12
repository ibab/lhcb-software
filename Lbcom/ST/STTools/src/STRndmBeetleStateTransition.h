// $Id: $
#ifndef STRndmBeetleStateTransition_H
#define STRndmBeetleStateTransition_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// random numbers
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// LHCbKernel
#include "GaudiKernel/IAlgTool.h"

/** @class STRndmBeetleStateTransition
 *
 *  Tool for making Beetles transition from one state to another
 *  Selection is random !
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STRndmBeetleStateTransition: public ST::ToolBase {

 public: 
   
  /// constructer
  STRndmBeetleStateTransition( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STRndmBeetleStateTransition();

  /// initialize
  virtual StatusCode initialize();

 private:

  // smart interface to generator
  SmartIF<IRndmGen> m_uniformDist; 

  double m_fracToChange;
  std::string m_newState;


};

#endif // STBeetleStateTransition
