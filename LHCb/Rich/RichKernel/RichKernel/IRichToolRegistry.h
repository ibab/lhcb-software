// $Id: IRichToolRegistry.h,v 1.1 2004-06-17 12:00:46 cattanem Exp $
#ifndef RICHUTILS_IRICHTOOLREGISTRY_H
#define RICHUTILS_IRICHTOOLREGISTRY_H 1

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
  virtual const std::string & toolType( const std::string & name ) const = 0;

};

#endif // RICHUTILS_IRICHTOOLREGISTRY_H
