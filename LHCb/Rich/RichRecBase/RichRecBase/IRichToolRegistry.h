// $Id: IRichToolRegistry.h,v 1.1 2003-06-30 15:11:56 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHTOOLREGISTRY_H
#define RICHRECTOOLS_IRICHTOOLREGISTRY_H 1

/** @class IRichToolRegistry IRichToolRegistry.h
 *
 *  Interface to the tool providing a sigle point of definition for all tool
 *  names and types.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichToolRegistry( "IRichToolRegistry" , 1 , 0 );

class IRichToolRegistry : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichToolRegistry; }

  /// Method returning a pointer to  a particular tool
  virtual IAlgTool * acquireTool( std::string name ) = 0;

  /// Method to inform that a tool is no longer required
  virtual void releaseTool( std::string name ) = 0;

};

#endif // RICHRECTOOLS_IRICHTOOLREGISTRY_H
