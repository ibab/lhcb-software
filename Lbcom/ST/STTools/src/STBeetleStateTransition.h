// $Id: $
#ifndef STBeetleStateTransition_H
#define STBeetleStateTransition_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "GaudiKernel/IAlgTool.h"

/** @class STBeetleStateTransition
 *
 *  Tool for making Beetles transition from one state to another
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STBeetleStateTransition: public ST::ToolBase {

 public: 
   
  /// constructer
  STBeetleStateTransition( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STBeetleStateTransition();

  /// initialize
  virtual StatusCode initialize();

 private:

  std::string m_initialState;
  std::string m_finalState;


};

#endif // STBeetleStateTransition
