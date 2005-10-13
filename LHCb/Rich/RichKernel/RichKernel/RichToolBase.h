
//-----------------------------------------------------------------------------
/** @file RichToolBase.h
 *
 *  Header file for tool base class : RichToolBase
 *
 *  CVS Log :-
 *  $Id: RichToolBase.h,v 1.11 2005-10-13 15:03:42 jonrob Exp $
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
/** @class RichToolBase RichToolBase.h RichKernel/RichToolBase.h
 *
 *  Abstract base class for RICH tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichToolBase : public RichCommonBase<GaudiTool>
{

public:

  /// Standard constructor
  RichToolBase( const std::string& type,
                const std::string& name,
                const IInterface* parent )
    : RichCommonBase<GaudiTool> ( type, name, parent ) { }

};

#endif // RICHKERNEL_RICHTOOLBASE_H
