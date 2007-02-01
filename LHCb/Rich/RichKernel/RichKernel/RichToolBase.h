
//-----------------------------------------------------------------------------
/** @file RichToolBase.h
 *
 *  Header file for tool base class : Rich::ToolBase
 *
 *  CVS Log :-
 *  $Id: RichToolBase.h,v 1.12 2007-02-01 17:24:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHTOOLBASE_H
#define RICHKERNEL_RICHTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTool.h"

// local
#include "RichKernel/RichCommonBase.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class ToolBase RichToolBase.h RichKernel/RichToolBase.h
   *
   *  Abstract base class for RICH tools providing
   *  some basic functionality.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class ToolBase : public Rich::CommonBase<GaudiTool>
  {

  public:

    /// Standard constructor
    ToolBase( const std::string& type,
              const std::string& name,
              const IInterface* parent )
      : Rich::CommonBase<GaudiTool> ( type, name, parent ) { }

  };

}

/** For backwards compatibility
 *  @todo Remove this typedef
 */
typedef Rich::ToolBase RichToolBase;

#endif // RICHKERNEL_RICHTOOLBASE_H
