
//--------------------------------------------------------------------------------
/** @file RichSmartIDCnv.cpp
 *
 *  Implementation file for RichSmartID conversion utilities
 *
 *  CVS Log :-
 *  $Id: RichSmartIDCnv.cpp,v 1.3 2008-02-22 10:56:00 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   05/02/2008
 */
//--------------------------------------------------------------------------------

// local
#include "RichKernel/RichSmartIDCnv.h"

// Returns the number of pixel rows per HPD (depending on data mode)
unsigned int Rich::SmartIDGlobalOrdering::_nPixelRowsPerHPD() const
{
  return ( pixelMode() == ALICEMode           ? Rich::DAQ::MaxDataSizeALICE :
           pixelMode() == LHCbMode            ? Rich::DAQ::MaxDataSize      :
           smartID().pixelSubRowDataIsValid() ? Rich::DAQ::MaxDataSizeALICE :
           Rich::DAQ::MaxDataSize );
}

// Returns the HPD column offset number
unsigned int Rich::SmartIDGlobalOrdering::_panelHPDColOffset() const
{
  return ( Rich::Rich2 == smartID().rich() ?
           ( Rich::left == smartID().panel() ? _nHPDCols() : 0 ) :
           ( Rich::top  == smartID().panel() ? _nHPDCols() : 0 ) );
}

// Returns the 'global' HPD column number
int Rich::SmartIDGlobalOrdering::_hpdCol() const
{
  return ( Rich::Rich2 == smartID().rich() ?
           ( _panelHPDColOffset() + smartID().hpdCol() ) :
           ( _panelHPDColOffset() + _nHPDCols() - 1 - smartID().hpdCol() )
           );
}

// Returns the maximum 'global' pixel x-coordinate
int Rich::SmartIDGlobalOrdering::maxGlobalPixelX() const
{
  return ( Rich::Rich2 == smartID().rich() ?
           _nPixelColsPerHPD() * _nHPDCols() * 2 :
           (int)( _nPixelRowsPerHPD() * ( _nHPDsPerCol() + 0.5 ) ) );
}

// Returns the maximum 'global' pixel column number with a given HPD size.
int Rich::SmartIDGlobalOrdering::maxGlobalPixelX(int sizeX) const
{
  return ( Rich::Rich2 == smartID().rich() ?
           sizeX * _nHPDCols() * 2 :
           (int)( sizeX * ( _nHPDsPerCol() + 0.5 ) ) );
}

// Returns the maximum 'global' pixel y-coordinate
int Rich::SmartIDGlobalOrdering::maxGlobalPixelY() const
{
  return ( Rich::Rich1 == smartID().rich() ?
           _nPixelColsPerHPD() * _nHPDCols() * 2 :
           (int)( _nPixelRowsPerHPD() * ( _nHPDsPerCol() + 0.5 ) ) );
}

// Returns the maximum 'global' pixel y-coordinate with a given HPD size.
int Rich::SmartIDGlobalOrdering::maxGlobalPixelY(int sizeY) const
{
  return ( Rich::Rich1 == smartID().rich() ?
           sizeY * _nHPDCols() * 2 :
           (int)( sizeY * ( _nHPDsPerCol() + 0.5 ) ) );
}

// Returns the 'correct' HPD pixel row number (ALICE or LHCb mode)
int Rich::SmartIDGlobalOrdering::_pixelRowNum() const
{
  return ( smartID().pixelSubRowDataIsValid() ?
           (Rich::DAQ::NumAlicePixelsPerLHCbPixel*smartID().pixelRow()) + smartID().pixelSubRow() :
           smartID().pixelRow() );
}

// Returns a 'global' pixel column number
int Rich::SmartIDGlobalOrdering::globalPixelX() const
{
  if ( Rich::Rich1 == smartID().rich() )
  {
    return ( _pixelRowNum() +
             (_numInCol()*_nPixelRowsPerHPD()) +
             (int)(globalHpdNumInColOffset()*_nPixelRowsPerHPD()) );
  }
  else
  {
    return ( smartID().pixelCol() + (_hpdCol()*_nPixelColsPerHPD()) );
  }
}

// Returns a 'global' pixel row number
int Rich::SmartIDGlobalOrdering::globalPixelY() const
{
  if ( Rich::Rich1 == smartID().rich() )
  {
    return ( smartID().pixelCol() +
             (_hpdCol()*_nPixelColsPerHPD()) );
  }
  else
  {
    return ( _pixelRowNum() +
             (_numInCol()*_nPixelRowsPerHPD()) +
             (int)(globalHpdNumInColOffset()*_nPixelRowsPerHPD()) );
  }
}

// Returns a 'global' pixel X coordinate number from a given local coordinate and size of X HPD direction.
int Rich::SmartIDGlobalOrdering::globalPixelX(int localX, int sizeX) const
{
  if ( Rich::Rich1 == smartID().rich() )
  {
    return ( localX +
             (_numInCol()*sizeX) +
             (int)(globalHpdNumInColOffset()*sizeX) );
  }
  else
  {
    return ( localX + (_hpdCol()*sizeX) );
  }
}

// Returns a 'global' pixel y coordinate from a given local coordinate and size of the y HPD direction.
int Rich::SmartIDGlobalOrdering::globalPixelY(int localY, int sizeY) const
{
  if ( Rich::Rich1 == smartID().rich() )
  {
    return ( localY +
             (_hpdCol()*sizeY) );
  }
  else
  {
    return ( localY +
             (_numInCol()*sizeY) +
             (int)(globalHpdNumInColOffset()*sizeY) );
  }
}

// Returns a 'local' pixel column number
int Rich::SmartIDGlobalOrdering::localPixelX() const
{
  if ( Rich::Rich1 == smartID().rich() )
  {
    return _pixelRowNum();

  }
  else
  {
    return smartID().pixelCol();
  }
}

// Returns a 'local' pixel row number
int Rich::SmartIDGlobalOrdering::localPixelY() const
{
  if ( Rich::Rich1 == smartID().rich() )
  {
    return smartID().pixelCol();
  }
  else
  {
    return _pixelRowNum();
  }
}

MsgStream&
Rich::SmartIDGlobalOrdering::fillStream( MsgStream& os ) const
{
  return os << "["
            << " hpdX=" << globalHpdX()
            << " hpdY=" << globalHpdY()
            << " pixX=" << globalPixelX()
            << " pixY=" << globalPixelY()
            << " ]";
}
