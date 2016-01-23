
//-----------------------------------------------------------------------------
/** @file RichRandomPixelBackgroundTool.h
 *
 *  Header file for tool : Rich::RandomPixelBackgroundTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-10
 */
//-----------------------------------------------------------------------------

#ifndef RICHRANDOMPIXELBACKGROUNDTOOL_H
#define RICHRANDOMPIXELBACKGROUNDTOOL_H 1

// STL
#include <algorithm>

// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// RichKernel
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichAddBackground.h"
#include "RichKernel/RichDAQDefinitions.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{

  /** @class RandomPixelBackgroundTool RichRandomPixelBackgroundTool.h
   *
   *  Implementation of Rich::IAddbackground that adds simple random background
   *  given a percentage dark count rate per pixel
   *
   *  @author Chris Jones
   *  @date   2008-10-10
   */
  class RandomPixelBackgroundTool : public Rich::ToolBase,
                                    virtual public Rich::IAddBackground
  {

  public:

    /// Standard constructor
    RandomPixelBackgroundTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

    /// Destructor
    virtual ~RandomPixelBackgroundTool( );

    /// Initialise
    virtual StatusCode initialize();

  public:

    /// Compute a set of background hits
    virtual StatusCode createBackgrounds( HPDBackgrounds & backgrounds,
                                          const bool aliceMode = false ) const;

  private:

    /// Rich System detector element
    const DeRichSystem * m_richSys = nullptr;

    /// The pixel background (dark rate) probability
    double m_pixBackgroundProb;

    // random numbers service
    IRndmGenSvc * m_randSvc = nullptr;

  };

}

#endif // RICHRANDOMPIXELBACKGROUNDTOOL_H
