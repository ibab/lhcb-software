
//-----------------------------------------------------------------------------
/** @file RichSmartIDCnv.h
 *
 *  Header file for RichSmartID conversion utilities
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   05/02/2008
 *  @edits: Young Min Kim (s0679231@sms.ed.ac.uk) on 13/06/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RichSmartIDCnv_H
#define RICHKERNEL_RichSmartIDCnv_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{

  /** @class SmartIDGlobalOrdering RichSmartIDCnv.h RichKernel/RichSmartIDCnv.h
   *
   *  Simple class to provide global ordering information for RichSmartIDs.
   *
   *  Useful to enable all hits for a given RICH detector to be drawn on a single
   *  figure.
   *
   *  Example Usage is :-
   *  @code
   *
   *  // Get a RichSmartID from somewhere
   *  LHCb::RichSmartID id = ....;
   *  // Make a local ordering object
   *  const Rich::SmartIDGlobalOrdering order(id);
   *  // Which RICH ?
   *  const std::string RICH = Rich::text( id.rich() );
   *
   *  // Draw plots showing all hits in each RICH
   *  plot2D( order.globalPixelX(), order.globalPixelY(),
   *          RICH, "Hit Map for "+RICH,
   *          order.minGlobalPixelX()-0.5,  order.maxGlobalPixelX()+0.5,
   *          order.minGlobalPixelY()-0.5,  order.maxGlobalPixelY()+0.5,
   *          order.totalNumInGlobalX()+1,  order.totalNumInGlobalY()+1 );
   *
   *  // Can also get the global HPD (x,y) numbers
   *  const int globalHPDX = order.globalHpdX();
   *  const int globalHPDY = order.globalHpdY();
   *
   *  // The constructor above automatically determines on the fly if the
   *  // data is in ALICE or LHCb mode, and adjust the returned data accordingly.
   *  // This normally will do what is correct. However, if you want you can
   *  // force a given mode using :-
   *  const Rich::SmartIDGlobalOrdering order(id,Rich::SmartIDGlobalOrdering::ALICEMode);
   *  const Rich::SmartIDGlobalOrdering order(id,Rich::SmartIDGlobalOrdering::LHCbMode);
   *
   *  @endcode
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/02/2008
   */
  class SmartIDGlobalOrdering
  {

  public: // definitions and constants

    /// The mode in operation to determine ALICE or LHCb data
    enum PixelMode
      {
        AutomaticMode = 0, ///< Automatically determine ALICE or LHCb mode from the RichSmartID (default)
        ALICEMode,         ///< Force ALICE mode (i.e. 256 pixel rows per HPD)
        LHCbMode           ///< Force LHCb mode  (i.e. 32  pixel rows per HPD)
      };

  public:

    /// Constructor from a RichSmartID
    explicit
    SmartIDGlobalOrdering( const LHCb::RichSmartID id,
                           const Rich::SmartIDGlobalOrdering::PixelMode mode
                           = Rich::SmartIDGlobalOrdering::AutomaticMode )
      : m_id ( id ), m_mode ( mode ) { }

    /// Access the RichSmartID this object represents
    inline LHCb::RichSmartID smartID() const { return m_id; }

    /// Access the data mode
    inline Rich::SmartIDGlobalOrdering::PixelMode pixelMode() const { return m_mode; }

  private:

    /** Number of PD columns per PD panel.
     *
     *  Note here 'column' is used in the RICH sense. I.e. a column is vertical
     *  in RICH2 but horizontal in RICH1.
     */
    inline unsigned int _nPDCols() const
    {
      // Eventually these numbers should be in RichDAQDefinitions.h ??
      return ( Rich::Rich1 == smartID().rich() ? 7 : 9 );
    }

    /** Number of PDs per column
     *
     *  Note here 'column' is used in the RICH sense. I.e. a column is vertical
     *  in RICH2 but horizontal in RICH1.
     */
    inline unsigned int _nPDsPerCol() const
    {
      // Eventually these numbers should be in RichDAQDefinitions.h ??
      return ( Rich::Rich1 == smartID().rich() ? 14 : 16 );
    }

    /// Returns the number of pixel rows per PD (depending on data mode)
    unsigned int _nPixelRowsPerPD() const;

    /// Returns the number of pixel columns per PD
    inline unsigned int _nPixelColsPerPD() const
    {
      return ( Rich::DAQ::NumPixelColumns );
    }

    /// Returns the HPD column offset number
    unsigned int _panelPDColOffset() const;

    /// Returns the 'correct' PD pixel row number (ALICE or LHCb mode)
    int _pixelRowNum() const;

    /// Returns the 'global' PD number in a RICH column
    inline int _numInCol() const
    {
      return ( ( Rich::Rich1 == smartID().rich() &&
                 Rich::bottom == smartID().panel() ) ?
               ( smartID().pdNumInCol() ) :
               ( _nPDsPerCol() - 1 - smartID().pdNumInCol() ) );
    }

    /// Returns the 'global' PD column number
    int _pdCol() const;

  public:

    /// Returns the minimum PD x-coordinate on a panel.
    inline int minPDX() const
    {
      return ( 0 );
    }

    /// Returns the maximum PD x-coordinate on a panel.
    inline int maxPDX() const
    {
      return ( ( Rich::Rich1 == smartID().rich() && Rich::bottom == smartID().panel() ) ?
               ( _nPDsPerCol() ) :
               ( _nPDCols() ) );
    }

    /// Returns the total number of PDs on the x-axis.
    inline int totalPDX() const
    {
      return maxPDX() - minPDX();
    }

    /// Returns the minimum PD y-coordinate on a panel.
    inline int minPDY() const
    {
      return ( 0 );
    }

    /// Returns the minimum PD y-coordinate on a panel.
    inline int maxPDY() const
    {
      return ( ( Rich::Rich1 == smartID().rich() && Rich::bottom == smartID().panel() ) ?
               ( _nPDCols() ) :
               ( _nPDsPerCol() ) );
    }

    /// Returns the total number of PDs on the y-axis.
    inline int totalPDY() const
    {
      return maxPDY() - minPDY();
    }

    /// Returns the minimum 'local' pixel x-coordinate
    inline int minLocalPixelX() const
    {
      return ( 0 );
    }

    /// Returns the maximum 'local' pixel x-coordinate
    int maxLocalPixelX() const;

    /// Returns the minimum 'local' pixel y-coordinate
    inline int minLocalPixelY() const
    {
      return ( 0 );
    }

    /// Returns the maximum 'local' pixel y-coordinate
    int maxLocalPixelY() const;

    /// Returns the minimum 'global' pixel x-coordinate
    inline int minGlobalPixelX() const
    {
      return ( 0 );
    }

    /// Returns the maximum 'global' pixel x-coordinate
    int maxGlobalPixelX() const;

    /// Returns the maximum 'global' pixel x-coordinate with a variable PD size
    int maxGlobalPixelX(int sizeX) const;

    /// Returns the minimum 'global' pixel y-coordinate
    inline int minGlobalPixelY() const
    {
      return ( 0 );
    }

    /// Returns the maximum 'global' pixel y-coordinate
    int maxGlobalPixelY() const;

    /// Returns the maximum 'glocal' pixel y-coordinate with a variable PD size
    int maxGlobalPixelY(int sizeY) const;

    /// The total number of 'global' pixel columns
    inline int totalNumInGlobalX() const
    {
      return maxGlobalPixelX() - minGlobalPixelX();
    }

    /// The total number of 'global' pixel rows
    inline int totalNumInGlobalY() const
    {
      return maxGlobalPixelY() - minGlobalPixelY();
    }

  public:

    /** Returns the PD number in column 'offset'.
     *
     *  Used to get the stagger between neighbouring PD columns
     */
    inline double globalPdNumInColOffset() const
    {
      double Offset;
      if ( Rich::Rich1 == smartID().rich() && Rich::bottom == smartID().panel() )
      {
        Offset = ((smartID().pdCol()+1)%2) * 0.5;
      }
      else
      {
        Offset = (smartID().pdCol()%2) * 0.5;
      }

      return Offset;
    }

    /// Returns a 'global' PD column number
    inline int globalPdX() const
    {
      return ( Rich::Rich2 == smartID().rich() ? _pdCol() : _numInCol() );
    }

    /// Returns a 'global' PD number in column number
    inline int globalPdY() const
    {
      return ( Rich::Rich1 == smartID().rich() ? _pdCol() : _numInCol() );
    }

    /// Returns a 'global' pixel column number
    int globalPixelX() const;

    /// Returns a 'global' pixel row number
    int globalPixelY() const;

    /// Returns a 'global' pixel column number from a given local coordinate with a variable PD x size.
    int globalPixelX(const int localX, const int sizeX) const;

    /// Returns a 'global' pixel row number from a given local corrdinate with a variable PD y size.
    int globalPixelY(const int localY, const int sizeY) const;

    /// Returns a 'local' pixel column number
    int localPixelX() const;

    /// Returns a 'local' pixel row number
    int localPixelY() const;

  public:

    /// Overload output to MsgStream
    friend inline MsgStream& operator << ( MsgStream& os,
                                           const SmartIDGlobalOrdering & order )
    { return order.fillStream(os); }

  private:

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

  private:

    LHCb::RichSmartID m_id; ///< The RichSmartID
    PixelMode m_mode;       ///< The data mode (Forced ALICE or LHCb, or automatic based on the data)

  };

  // Returns the number of pixel rows per PD (depending on data mode)
  inline unsigned int SmartIDGlobalOrdering::_nPixelRowsPerPD() const
  {
    return ( pixelMode() == ALICEMode           ? Rich::DAQ::MaxDataSizeALICE :
             pixelMode() == LHCbMode            ? Rich::DAQ::MaxDataSize      :
             smartID().pixelSubRowDataIsValid() ? Rich::DAQ::MaxDataSizeALICE :
             Rich::DAQ::MaxDataSize );
  }

  // Returns the PD column offset number
  inline unsigned int SmartIDGlobalOrdering::_panelPDColOffset() const
  {
    return ( Rich::Rich2 == smartID().rich() ?
             ( Rich::left == smartID().panel() ? _nPDCols() : 0 ) :
             ( Rich::top  == smartID().panel() ? _nPDCols() : 0 ) );
  }

  // Returns the 'global' PD column number
  inline int SmartIDGlobalOrdering::_pdCol() const
  {
    return ( ( Rich::Rich1 == smartID().rich() && Rich::top == smartID().panel() ) ?
             ( _panelPDColOffset() + _nPDCols() - 1 - smartID().pdCol() ) :
             ( _panelPDColOffset() + smartID().pdCol() ) );
  }

  /// Returns the maximum 'local' pixel x-coordinate
  inline int SmartIDGlobalOrdering::maxLocalPixelX() const
  {
    return ( Rich::Rich2 == smartID().rich() ?
             _nPixelColsPerPD() :
             _nPixelRowsPerPD() );
  }

  /// Returns the maximum 'local' pixel y-coordinate
  inline int SmartIDGlobalOrdering::maxLocalPixelY() const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             _nPixelColsPerPD() :
             _nPixelRowsPerPD() );
  }

  // Returns the maximum 'global' pixel x-coordinate
  inline int SmartIDGlobalOrdering::maxGlobalPixelX() const
  {
    return ( Rich::Rich2 == smartID().rich() ?
             _nPixelColsPerPD() * _nPDCols() * 2 :
             (int)( _nPixelRowsPerPD() * ( _nPDsPerCol() + 0.5 ) ) );
  }

  // Returns the maximum 'global' pixel column number with a given PD size.
  inline int SmartIDGlobalOrdering::maxGlobalPixelX(int sizeX) const
  {
    return ( Rich::Rich2 == smartID().rich() ?
             sizeX * _nPDCols() * 2 :
             (int)( sizeX * ( _nPDsPerCol() + 0.5 ) ) );
  }

  // Returns the maximum 'global' pixel y-coordinate
  inline int SmartIDGlobalOrdering::maxGlobalPixelY() const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             _nPixelColsPerPD() * _nPDCols() * 2 :
             (int)( _nPixelRowsPerPD() * ( _nPDsPerCol() + 0.5 ) ) );
  }

  // Returns the maximum 'global' pixel y-coordinate with a given PD size.
  inline int SmartIDGlobalOrdering::maxGlobalPixelY(int sizeY) const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             sizeY * _nPDCols() * 2 :
             (int)( sizeY * ( _nPDsPerCol() + 0.5 ) ) );
  }

  // Returns the 'correct' PD pixel row number (ALICE or LHCb mode)
  inline int SmartIDGlobalOrdering::_pixelRowNum() const
  {
    return ( smartID().pixelSubRowDataIsValid() ?
             (Rich::DAQ::NumAlicePixelsPerLHCbPixel*smartID().pixelRow()) + smartID().pixelSubRow() :
             smartID().pixelRow() );
  }

  // Returns a 'global' pixel column number
  inline int SmartIDGlobalOrdering::globalPixelX() const
  {
    if ( Rich::Rich1 == smartID().rich() )
    {
      return ( (Rich::bottom  == smartID().panel() ? 
                (_nPixelRowsPerPD() - _pixelRowNum()) 
                : _pixelRowNum()) + (_numInCol()*_nPixelRowsPerPD()) +
               (int)(globalPdNumInColOffset()*_nPixelRowsPerPD()) );
    }
    else
    {
      return ( smartID().pixelCol() + (_pdCol()*_nPixelColsPerPD()) );
    }
  }

  // Returns a 'global' pixel row number
  inline int SmartIDGlobalOrdering::globalPixelY() const
  {
    if ( Rich::Rich1 == smartID().rich() )
    {
      return ( (Rich::bottom  == smartID().panel() ? 
                (_nPixelColsPerPD() - smartID().pixelCol()) : 
                smartID().pixelCol()) + (_pdCol()*_nPixelColsPerPD()) );
    }
    else
    {
      return ( _pixelRowNum() +
               (_numInCol()*_nPixelRowsPerPD()) +
               (int)(globalPdNumInColOffset()*_nPixelRowsPerPD()) );
    }
  }

  // Returns a 'global' pixel X coordinate number from a given local coordinate and size of X PD direction.
  inline int SmartIDGlobalOrdering::globalPixelX(const int localX, const int sizeX) const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             // RICH1
             ( localX +
               (_numInCol()*sizeX) +
               (int)(globalPdNumInColOffset()*sizeX) ) :
             // RICH2
             ( localX + (_pdCol()*sizeX) )
             );
  }

  // Returns a 'global' pixel y coordinate from a given local coordinate and size of the y PD direction.
  inline int SmartIDGlobalOrdering::globalPixelY(const int localY, const int sizeY) const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             // RICH1
             ( localY + (_pdCol()*sizeY) ) :
             // RICH2
             ( localY +
               (_numInCol()*sizeY) +
               (int)(globalPdNumInColOffset()*sizeY) )
             );
  }

  // Returns a 'local' pixel column number
  inline int SmartIDGlobalOrdering::localPixelX() const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             // RICH1
             _pixelRowNum() :
             // RICH2
             smartID().pixelCol()
             );
  }

  // Returns a 'local' pixel row number
  inline int SmartIDGlobalOrdering::localPixelY() const
  {
    return ( Rich::Rich1 == smartID().rich() ?
             // RICH1
             smartID().pixelCol() :
             // RICH2
             _pixelRowNum()
             );
  }

}

#endif // RICHKERNEL_RichSmartIDCnv_H
