// $Id: IRichToolRegistry.h,v 1.2 2004-06-18 09:39:02 jonrob Exp $
#ifndef RICHKERNEL_IRICHTOOLREGISTRY_H
#define RICHKERNEL_IRICHTOOLREGISTRY_H 1

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

#endif // RICHKERNEL_IRICHTOOLREGISTRY_H
