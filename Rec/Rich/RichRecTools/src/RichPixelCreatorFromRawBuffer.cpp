
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.cpp
 *
 *  Implementation file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.cpp,v 1.13 2006-06-14 22:20:15 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromRawBuffer.h"

//---------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromRawBuffer>          s_factory ;
const        IToolFactory& RichPixelCreatorFromRawBufferFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromRawBuffer::
RichPixelCreatorFromRawBuffer( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : RichPixelCreatorBase ( type, name, parent ) { }
