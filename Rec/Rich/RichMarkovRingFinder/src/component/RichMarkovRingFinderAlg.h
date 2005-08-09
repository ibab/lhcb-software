
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.h
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.h,v 1.18 2005-08-09 13:17:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

#ifndef RICHMARKOVRINGFINDER_RICHMARKOVRINGFINDERALG_H
#define RICHMARKOVRINGFINDER_RICHMARKOVRINGFINDERALG_H 1

// RichRecBase
#include "RichRecBase/RichRecMoniAlgBase.h"

//-----------------------------------------------------------------------
/** @class RichMarkovRingFinderAlg RichMarkovRingFinderAlg.h
 *  
 *  Tracklessm ring finder using a Markov Chaion Monte Carlo
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------

class RichMarkovRingFinderAlg : public RichRecMoniAlgBase
{

public: 

  /// Standard constructor
  RichMarkovRingFinderAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichMarkovRingFinderAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};

#endif // RICHMARKOVRINGFINDER_RICHMARKOVRINGFINDERALG_H
