
/** @file RichZSHitTriplet.h
 *
 *  Header file for RICH DAQ utility class : RichZSHitTriplet
 *
 *  CVS Log :-
 *  $Id: RichZSHitTriplet.h,v 1.15 2007-04-23 12:58:44 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */

#ifndef RICHDAQ_RICHZSHITTRIPLET_H
#define RICHDAQ_RICHZSHITTRIPLET_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  namespace DAQ
  {

    /** @class RichZSHitTriplet RichZSHitTriplet.h
     *
     *  Utility class representing three hits packed into a single word
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-06
     */

    class RichZSHitTriplet
    {

    public: // definitions

      /// Number of bits for each row or column number
      static const Rich::DAQ::ShortType BitsHit    = 5;

      // shift registers
      static const Rich::DAQ::ShortType ShiftRow0  = 0;
      static const Rich::DAQ::ShortType ShiftCol0  = ShiftRow0 + BitsHit;
      static const Rich::DAQ::ShortType ShiftRow1  = ShiftCol0 + BitsHit;
      static const Rich::DAQ::ShortType ShiftCol1  = ShiftRow1 + BitsHit;
      static const Rich::DAQ::ShortType ShiftRow2  = ShiftCol1 + BitsHit;
      static const Rich::DAQ::ShortType ShiftCol2  = ShiftRow2 + BitsHit;

      // The masks
      static const Rich::DAQ::LongType  MaskRow0   = ((1 << BitsHit)-1) << ShiftRow0;
      static const Rich::DAQ::LongType  MaskCol0   = ((1 << BitsHit)-1) << ShiftCol0;
      static const Rich::DAQ::LongType  MaskRow1   = ((1 << BitsHit)-1) << ShiftRow1;
      static const Rich::DAQ::LongType  MaskCol1   = ((1 << BitsHit)-1) << ShiftCol1;
      static const Rich::DAQ::LongType  MaskRow2   = ((1 << BitsHit)-1) << ShiftRow2;
      static const Rich::DAQ::LongType  MaskCol2   = ((1 << BitsHit)-1) << ShiftCol2;

      // the max row/column values storable
      static const Rich::DAQ::ShortType MaxRowCol  = ( 1 << BitsHit ) - 1;

    public: // methods

      /// Copy Constructor
      RichZSHitTriplet( const RichZSHitTriplet & hits ) : m_data( hits.data() ) {}

      /// Constructor from Rich::DAQ::LongType
      RichZSHitTriplet( const Rich::DAQ::LongType data = 0 ) : m_data( data ) { }

      /// Constructor from three MCRichDigits
      RichZSHitTriplet( const LHCb::RichSmartID * digOne   ,  ///< Pointer to first LHCb::RichSmartID to store
                        const LHCb::RichSmartID * digTwo   ,  ///< Pointer to second LHCb::RichSmartID to store
                        const LHCb::RichSmartID * digThree    ///< Pointer to third LHCb::RichSmartID to store
                        )
        : m_data ( 0 )
      {
        if ( digOne ) {
          setRow0 ( digOne->pixelRow() );
          setCol0 ( digOne->pixelCol() );
        }
        if ( digTwo ) {
          setRow1 ( digTwo->pixelRow() );
          setCol1 ( digTwo->pixelCol() );
        }
        if ( digThree ) {
          setRow2 ( digThree->pixelRow() );
          setCol2 ( digThree->pixelCol() );
        }
      }

      /// Destructor
      ~RichZSHitTriplet( ) { }

      /// Retrieve the full value
      inline Rich::DAQ::LongType data() const { return m_data; }

      /// operator to convert to Rich::DAQ::LongType
      inline operator Rich::DAQ::LongType() const { return data(); }

      /// Set the first hit row number
      inline void setRow0( const Rich::DAQ::ShortType row )
      {
        dataInRange(row,MaxRowCol);
        set( row, ShiftRow0, MaskRow0 );
      }

      /// Set the first hit column number
      inline void setCol0( const Rich::DAQ::ShortType col )
      {
        dataInRange(col,MaxRowCol);
        set( col, ShiftCol0,MaskCol0 );
      }

      /// Set the second hit row number
      inline void setRow1( const Rich::DAQ::ShortType row )
      {
        dataInRange(row,MaxRowCol);
        set( row, ShiftRow1,MaskRow1 );
      }

      /// Set the second hit column number
      inline void setCol1( const Rich::DAQ::ShortType col )
      {
        dataInRange(col,MaxRowCol);
        set( col, ShiftCol1,MaskCol1 );
      }

      /// Set the third hit row number
      inline void setRow2( const Rich::DAQ::ShortType row )
      {
        dataInRange(row,MaxRowCol);
        set( row, ShiftRow2, MaskRow2 );
      }

      /// Set the third hit column number
      inline void setCol2( const Rich::DAQ::ShortType col )
      {
        dataInRange(col,MaxRowCol);
        set( col, ShiftCol2,MaskCol2 );
      }

      /// Retrieve the first hit row number
      inline Rich::DAQ::ShortType row0() const
      {
        return ( (data() & MaskRow0) >> ShiftRow0 );
      }

      /// Retrieve the second hit row number
      inline Rich::DAQ::ShortType row1() const
      {
        return ( (data() & MaskRow1) >> ShiftRow1 );
      }

      /// Retrieve the third hit row number
      inline Rich::DAQ::ShortType row2() const
      {
        return ( (data() & MaskRow2) >> ShiftRow2 );
      }

      /// Retrieve the first hit column number
      inline Rich::DAQ::ShortType col0() const
      {
        return ( (data() & MaskCol0) >> ShiftCol0 );
      }

      /// Retrieve the second hit column number
      inline Rich::DAQ::ShortType col1() const
      {
        return ( (data() & MaskCol1) >> ShiftCol1 );
      }

      /// Retrieve the third hit column number
      inline Rich::DAQ::ShortType col2() const
      {
        return ( (data() & MaskCol2) >> ShiftCol2 );
      }

    private: // methods

      /// Update the internal data
      inline void setData( const Rich::DAQ::LongType data ) { m_data = data;  }

      /// Set the data value for a given mask and shift value
      inline void set( const Rich::DAQ::ShortType value,
                       const Rich::DAQ::ShortType shift,
                       const Rich::DAQ::LongType  mask )
      {
        setData( ((value << shift) & mask) | (data() & ~mask) );
      }

      /// tests whether a given value is in range for a given data field
      inline void dataInRange( const Rich::DAQ::ShortType value,
                               const Rich::DAQ::ShortType max ) const
      {
        if ( value > max ) 
        { 
          throw GaudiException( "Data out of range", "*RichZSHitTriplet*", StatusCode::FAILURE );
        }
      }

    private: // data

      /// The data word
      Rich::DAQ::LongType m_data = 0;

    };

  }
}

#endif // RICHDAQ_RICHZSHITTRIPLET_H
