
//-----------------------------------------------------------------------------
/** @file RichHPDIdentifier
 *
 *  Header file for Rich::DAQ::HPDIdentifier
 *
 *  CVS Log :-
 *  $Id: RichHPDIdentifier.h,v 1.5 2008-02-06 10:45:56 jonrob Exp $
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
       *              and DD is the number in column
       */
      explicit HPDIdentifier( const int data = -1 ) : m_data(data) { }

      /// Constructor from a RichSmartID
      explicit HPDIdentifier( const LHCb::RichSmartID id )
        : m_data ( id.isValid() ? 
                   (int)(100000*(1+(int)id.rich()) + 10000*((int)id.panel()) +
                         100*id.hpdCol() + id.hpdNumInCol()) : -1 )
      { }

    public:

      /// Implicit conversion into a RichSmartID
      inline operator LHCb::RichSmartID () const
      {
        return ( m_data == -1 ?
                 LHCb::RichSmartID() :
                 LHCb::RichSmartID( (Rich::DetectorType)((m_data/100000)-1),
                                    (Rich::Side)((m_data/10000)%10),
                                    m_data%100,
                                    (m_data/100)%100 ) );
      }

      /// Access the human readable HPD number
      inline int number() const { return m_data; }

    private:

      int m_data; ///< The internal data

    };

  }
}

#endif // RICHKERNEL_RICHHPDIDENTIFIER_H
