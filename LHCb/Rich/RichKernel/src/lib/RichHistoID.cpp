
//-----------------------------------------------------------------------------
/** @file RichHistoID.cpp
 *
 *  Implementation file for RICH utility class : Rich::HistogramID
 *
 *  CVS Log :-
 *  $Id: RichHistoID.cpp,v 1.1 2010-02-12 16:55:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/02/2010
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/RichHistoID.h"

//-----------------------------------------------------------------------------

std::string 
Rich::HistogramID::fullid() const
{  
  std::string fid("");
  if      ( rad()  != Rich::InvalidRadiator ) { fid += Rich::text(rad())        + "/"; }
  else if ( det()  != Rich::InvalidDetector ) { fid += Rich::text(det())        + "/"; }
  if      ( side() != Rich::InvalidSide     ) { fid += Rich::text(det(),side()) + "/"; }
  if      ( pid()  != Rich::Unknown         ) { fid += Rich::text(pid())        + "/"; }
  return fid + id();
}

std::string 
Rich::HistogramID::fullTitle( const std::string & title ) const
{
  std::string ftitle("");
  if      ( rad()  != Rich::InvalidRadiator ) { ftitle += Rich::text(rad())        + " "; }
  else if ( det()  != Rich::InvalidDetector ) { ftitle += Rich::text(det())        + " "; }
  if      ( side() != Rich::InvalidSide     ) { ftitle += Rich::text(det(),side()) + " "; }
  if      ( pid()  != Rich::Unknown         ) { ftitle += Rich::text(pid())        + " "; }
  return ftitle + title;
}
