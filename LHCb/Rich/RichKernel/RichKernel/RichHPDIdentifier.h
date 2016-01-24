
//-----------------------------------------------------------------------------
/** @file RichHPDIdentifier.h
 *
 *  Header file for Rich::DAQ::HPDIdentifier
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   27/04/2007
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHPDIDENTIFIER_H
#define RICHKERNEL_RICHHPDIDENTIFIER_H 1

// Kernel
#include "Kernel/RichSmartID.h"

namespace Rich
{
  namespace DAQ
  {

    /** @class HPDIdentifier RichHPDIdentifier.h RichKernel/RichHPDIdentifier.h
     *
     *  Simple class to convert a human readable HPD number into and
     *  from a RichSmartID
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   27/04/2007
     */
    class HPDIdentifier
    {

    public:

      /** Constructor from human readable number
       *  @param data The HPD information in a human readable form.
       *              The format is ABCCDD where A is the Rich number (1,2),
       *              B is the panel number (0,1), CC is the column number
       *              and DD is the number in column.
       *              For the PMT case RPMMMNN is Rich, Panel, Module and Number
       */
      explicit HPDIdentifier( const int data = -1 ) : m_data(data) { }

      /// Constructor from a RichSmartID
      explicit HPDIdentifier( const LHCb::RichSmartID id )
      {
        if ( LHCb::RichSmartID::HPDID == id.idType() )
        {
          m_data = ( id.isValid() ?
                     (int)(100000*(1+(int)id.rich()) + 10000*((int)id.panel()) +
                           100*id.pdCol() + id.pdNumInCol()) : -1 );
        }
        else  // MaPMT
        {
          m_data = ( id.isValid() ?
                     (int)(1000000*(1+(int)id.rich()) + 100000*((int)id.panel()) +
                           100*id.pdCol() + id.pdNumInCol()) : -1 );
        }
      }

    public:

      /// Return a RichSmartID
      inline LHCb::RichSmartID smartID() const
      {
        if ( m_data > 999999 ) // MaPMT
        {
          return LHCb::RichSmartID( (Rich::DetectorType)((m_data/1000000)-1),
                                    (Rich::Side)((m_data/100000)%10),
                                    m_data%100,
                                    (m_data/100)%1000,
                                    LHCb::RichSmartID::MaPMTID);
        }
        else
        {
          return ( m_data == -1 ?
                   LHCb::RichSmartID() :
                   LHCb::RichSmartID( (Rich::DetectorType)((m_data/100000)-1),
                                      (Rich::Side)((m_data/10000)%10),
                                      m_data%100,
                                      (m_data/100)%100 ) );
        }
      }

      /// Implicit conversion into a RichSmartID
      inline operator LHCb::RichSmartID() const
      {
        return this->smartID();
      }

      /// Access the human readable HPD number
      inline int number() const noexcept { return m_data; }

    private:

      int m_data{-1}; ///< The internal data

    };

  }
}

#endif // RICHKERNEL_RICHHPDIDENTIFIER_H
