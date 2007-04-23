
//=============================================================================================
/** @file RichDAQL0Header.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::RichDAQL0Header
 *
 *  CVS Log :-
 *  $Id: RichDAQL0Header.h,v 1.1 2007-04-23 12:44:04 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/01/2007
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQL0Header_H
#define RICHDAQ_RichDAQL0Header_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  namespace DAQ
  {

    //=============================================================================================
    /** @class L0Header RichDAQL0Header.h
     *
     *  The overall L0 header
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   23/01/2007
     */
    //=============================================================================================

    class L0Header
    {

    public: // Each word

      //=============================================================================================
      /** @class Word0 RichDAQL0Header.h
       *
       *  The first word of the RICH L0 header
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   23/01/2007
       */
      //=============================================================================================
      class Word0
      {
      private: // bit packing info

        // To Do : Add specific bit fileds here

      public:

        /// Constructor from raw LongType
        explicit Word0( const LongType data = 0 ) : m_data(data) { }

        /// Retrieve the full value
        inline LongType data() const { return m_data; }

      private:

        /// The data word
        LongType m_data;

      };

      //=============================================================================================
      /** @class Word1 RichDAQL0Header.h
       *
       *  The second word of the RICH L0 header
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   23/01/2007
       */
      //=============================================================================================
      class Word1
      {
      private: // bit packing info

        // To Do : Add specific bit files here

      public:

        /// Constructor from raw LongType
        explicit Word1( const LongType data = 0 ) : m_data(data) { }

        /// Retrieve the full value
        inline LongType data() const { return m_data; }

      private:

        /// The data word
        LongType m_data;

      };

    public:

      /// Constructor from raw LongType words
      explicit L0Header( const Word0 word0,
                         const Word1 word1 )
        : m_word0(word0),
          m_word1(word1) { }

      /// Destructor
      ~L0Header( ) { }

      /// Retrieve the first word
      inline Word0 word0() const { return m_word0; }

      /// Retrieve the second word
      inline Word1 word1() const { return m_word1; }

    public:

      /// Write this header to a RAWBank
      inline void fillRAWBank( RAWBank & bank ) const
      {
        bank.push_back( this->word0().data() );
        bank.push_back( this->word1().data() );
      }

    private:

      /// The first data word
      Word0 m_word0;

      /// The second data word
      Word1 m_word1;

    };

  }
}

#endif // RICHDAQ_RichDAQL0Header_H
