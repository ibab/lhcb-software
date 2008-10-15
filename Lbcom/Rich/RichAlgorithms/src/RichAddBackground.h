
//---------------------------------------------------------------------------------
/** @file RichAddBackground.h
 *
 * Header file for class : Rich::AddBackground
 *
 * CVS Log :-
 * $Id: RichAddBackground.h,v 1.1.1.1 2008-10-15 12:34:57 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2008-10-10
 */
//---------------------------------------------------------------------------------

#ifndef RICHALGORITHMS_RICHADDBACKGROUND_H
#define RICHALGORITHMS_RICHADDBACKGROUND_H 1

// RichKernel
#include "RichKernel/RichAlgBase.h"
#include "RichKernel/IRichAddBackground.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{

  /** @class AddBackground RichAddBackground.h
   *
   *  Adds random photon detector backgrounds to the decoded raw data
   *
   *  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
   *  @date   2008-10-10
   */

  class AddBackground : public Rich::AlgBase
  {

  public:

    /// Standard constructor
    AddBackground( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~AddBackground( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution

  private:

    /// Rich System detector element
    const DeRichSystem * m_richSys;

    // The background tool
    const Rich::IAddBackground * m_background;

    /// Raw Buffer Decoding tool
    const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

  };

}

#endif // RICHALGORITHMS_RICHADDBACKGROUND_H
