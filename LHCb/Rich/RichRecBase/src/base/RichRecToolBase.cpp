// $Id: RichRecToolBase.cpp,v 1.8 2003-11-02 21:46:03 jonrob Exp $

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
  : RichToolBase ( type, name, parent ) { }

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
