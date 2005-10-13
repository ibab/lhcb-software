
//-----------------------------------------------------------------------------
/** @file RichMoniToolBase.h
 *
 *  Header file for tool base class : RichMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichMoniToolBase.h,v 1.6 2005-10-13 15:03:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMONITOOLBASE_H
#define RICHKERNEL_RICHMONITOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "RichKernel/RichCommonBase.h"

//-----------------------------------------------------------------------------
/** @class RichMoniToolBase RichMoniToolBase.h RichKernel/RichMoniToolBase.h
 *
 *  Abstract base class for RICH tools providing
 *  some basic functionality. In addition, uses the histogramming and ntupling
 *  functionality from the base class GaudiTupleAlg.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichMoniToolBase : public RichCommonBase<GaudiTupleTool>
{

public:

  /// Standard constructor
  RichMoniToolBase( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
    : RichCommonBase<GaudiTupleTool> ( type, name, parent ) { }

};

#endif // RICHKERNEL_RICHMONITOOLBASE_H
