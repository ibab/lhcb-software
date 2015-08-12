
//---------------------------------------------------------------------------
/** @file RichRecPixelBackgroundAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::PixelBackgroundAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichRecPixelBackgroundAlg_H
#define RICHRECALGORITHMS_RichRecPixelBackgroundAlg_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// interfaces
#include "RichRecBase/IRichPixelBackgroundEsti.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class PixelBackgroundAlg RichRecPixelBackgroundAlg.h
     *
     *  Algorithm to run the pixel background algorithm for all segments
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class PixelBackgroundAlg : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      PixelBackgroundAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~PixelBackgroundAlg();   ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution

    private:

      /// background tool
      const IPixelBackgroundEsti * m_bkgTool;

    };

  }
}

#endif // RICHRECALGORITHMS_RichRecPixelBackgroundAlg_H
