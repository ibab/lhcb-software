
//-----------------------------------------------------------------------------
/** @file RichTupleAlgBase.h
 *
 *  Header file for algorithm base class : RichTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: RichTupleAlgBase.h,v 1.2 2006-08-28 10:59:31 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHTUPLEALGBASE_H
#define RICHKERNEL_RICHTUPLEALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "RichKernel/RichCommonBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

//-----------------------------------------------------------------------------
/** @class RichTupleAlgBase RichTupleAlgBase.h RichKernel/RichTupleAlgBase.h
 *
 *  Abstract base class for RICH algorithms providing some basic functionality. 
 *
 *  In addition, uses the histogramming and ntupling
 *  functionality from the base class GaudiTupleAlg.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichTupleAlgBase : public RichCommonBase<GaudiTupleAlg>
{

public:

  /// Standard constructor
  RichTupleAlgBase( const std::string& name,
                    ISvcLocator* pSvcLocator )
    : RichCommonBase<GaudiTupleAlg> ( name, pSvcLocator )
  {
    setHistoTopDir  ( "RICH/" );
    //setNTupleTopDir ( "RICH/" );
  }

};

#endif // RICHKERNEL_RICHTUPLEALGBASE_H
