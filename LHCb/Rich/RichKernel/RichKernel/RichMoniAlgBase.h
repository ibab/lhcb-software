
//-----------------------------------------------------------------------------
/** @file RichMoniAlgBase.h
 *
 *  Header file for algorithm base class : RichMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichMoniAlgBase.h,v 1.7 2005-10-13 15:03:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMONIALGBASE_H
#define RICHKERNEL_RICHMONIALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "RichKernel/RichCommonBase.h"

//-----------------------------------------------------------------------------
/** @class RichMoniAlgBase RichMoniAlgBase.h RichKernel/RichMoniAlgBase.h
 *
 *  Abstract base class for RICH algorithms providing
 *  some basic functionality. In addition, uses the histogramming and ntupling
 *  functionality from the base class GaudiTupleAlg.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichMoniAlgBase : public RichCommonBase<GaudiTupleAlg>
{

public:

  /// Standard constructor
  RichMoniAlgBase( const std::string& name,
                   ISvcLocator* pSvcLocator )
    : RichCommonBase<GaudiTupleAlg> ( name, pSvcLocator ) { }

};

#endif // RICHKERNEL_RICHMONIALGBASE_H
