
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecToolBase.cpp,v 1.11 2005-04-20 16:21:20 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecToolBase.h"

// Standard constructor, initializes variables
RichRecToolBase::RichRecToolBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_pixTool  (0),
    m_tkTool   (0),
    m_segTool  (0),
    m_photTool (0),
    m_statTool (0) { }

StatusCode RichRecToolBase::initialize()
{
  // base class initilize
  return RichToolBase::initialize();
}

StatusCode RichRecToolBase::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}
