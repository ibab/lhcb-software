// $Id: RichRecToolBase.cpp,v 1.9 2004-02-02 14:23:06 jonesc Exp $

// local
#include "RichRecBase/RichRecToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecToolBase
//
// 2002-07-26 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichRecToolBase::RichRecToolBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_pixTool  (0),
    m_tkTool   (0),
    m_statTool (0),
    m_segTool  (0),
    m_photTool (0) { }

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
