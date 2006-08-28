
//-----------------------------------------------------------------------------
/** @file RichTupleToolBase.h
 *
 *  Header file for tool base class : RichTupleToolBase
 *
 *  CVS Log :-
 *  $Id: RichTupleToolBase.h,v 1.2 2006-08-28 10:59:31 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHTUPLETOOLBASE_H
#define RICHKERNEL_RICHTUPLETOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "RichKernel/RichCommonBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

//-----------------------------------------------------------------------------
/** @class RichTupleToolBase RichTupleToolBase.h RichKernel/RichTupleToolBase.h
 *
 *  Abstract base class for RICH tools providing some basic functionality. 
 *
 *  In addition, uses the histogramming and ntupling
 *  functionality from the base class GaudiTupleTool.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichTupleToolBase : public RichCommonBase<GaudiTupleTool>
{

public:

  /// Standard constructor
  RichTupleToolBase( const std::string& type,
                     const std::string& name,
                     const IInterface* parent )
    : RichCommonBase<GaudiTupleTool> ( type, name, parent )
  {
    setHistoTopDir  ( "RICH/" );
    //setNTupleTopDir ( "RICH/" );
  }

};

#endif // RICHKERNEL_RICHTUPLETOOLBASE_H
