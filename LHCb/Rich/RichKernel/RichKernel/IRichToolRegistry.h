
//-----------------------------------------------------------------------------
/** @file IRichToolRegistry.h
 *
 *  Header file for utility class : IRichToolRegistry
 *
 *  CVS Log :-
 *  $Id: IRichToolRegistry.h,v 1.6 2006-03-12 16:12:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHTOOLREGISTRY_H
#define RICHKERNEL_IRICHTOOLREGISTRY_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/// Static Interface Identification
static const InterfaceID IID_IRichToolRegistry( "IRichToolRegistry" , 1 , 0 );

//-----------------------------------------------------------------------------
/** @class IRichToolRegistry IRichToolRegistry.h
 *
 *  Interface to the tool providing a mapping between tool "nicknames" and types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichToolRegistry : public virtual IAlgTool 
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichToolRegistry; }

  /** Converts a tool "nickname" into a particular class name
   *
   *  @param nickname The tool nickname for which the class name is requested
   * 
   *  @return The class name of the tool associated to the given "nickname"
   */
  virtual const std::string & toolType( const std::string & nickname ) const = 0;

  /** Converts a tool "nickname" into a particular instance name
   *
   *  Takes into account the Context of the tool.
   *
   *  @param nickname The tool nickname for which the class name is requested
   * 
   *  @return The instance name of the tool associated to the given "nickname"
   */
  virtual const std::string toolName( const std::string & nickname ) const = 0;

  /// Returns the context for the tool registry
  virtual const std::string getContext() const = 0;

};

#endif // RICHKERNEL_IRICHTOOLREGISTRY_H
