// $Id: IRichToolRegistry.h,v 1.2 2003-07-03 13:08:33 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHTOOLREGISTRY_H
#define RICHRECTOOLS_IRICHTOOLREGISTRY_H 1

/** @class IRichToolRegistry IRichToolRegistry.h
 *
 *  Interface to the tool providing a mapping between tool names and types
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

  /// Method to return the tool type from the name
  virtual std::string toolType( std::string name ) = 0;

};

#endif // RICHRECTOOLS_IRICHTOOLREGISTRY_H
