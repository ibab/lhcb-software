// $Id: NewMCUtilityTool.h,v 1.1.1.1 2001-10-19 17:06:05 gcorti Exp $
#ifndef TOOLSTUTORIAL_NEWMCUTILITYTOOL_H 
#define TOOLSTUTORIAL_NEWMCUTILITYTOOL_H 1

// Include files
#include "MCUtilityTool.h"

/** @class NewMCUtilityTool NewMCUtilityTool.h src.solution/NewMCUtilityTool.h
 *  A MCUtility tool with a different implementation of the 
 *  the printDecay method
 *
 *  @author Gloria Corti
 *  @date   18/10/2001
 */
class NewMCUtilityTool : public MCUtilityTool {
public:

  /// Constructor
  NewMCUtilityTool(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  /// Destructor 
  ~NewMCUtilityTool( ); 

  /// Overwritten method
  void printDecayTree( long depth, const std::string& prefix,
                       const MCParticle* mother);  

  /// A new method ???

};

#endif // TOOLSTUTORIAL_NEWMCUTILITYTOOL_H
