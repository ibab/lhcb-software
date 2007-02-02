
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.cpp
 *
 *  Implementation file for tool : Rich::Rec::PixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.cpp,v 1.15 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRawBuffer.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//---------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromRawBuffer );

// Standard constructor
PixelCreatorFromRawBuffer::
PixelCreatorFromRawBuffer( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : PixelCreatorBase ( type, name, parent ) { }
