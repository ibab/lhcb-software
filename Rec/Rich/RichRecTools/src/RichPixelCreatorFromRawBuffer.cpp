
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.cpp,v 1.14 2006-12-01 17:05:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRawBuffer.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

//---------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichPixelCreatorFromRawBuffer );

// Standard constructor
RichPixelCreatorFromRawBuffer::
RichPixelCreatorFromRawBuffer( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : RichPixelCreatorBase ( type, name, parent ) { }
