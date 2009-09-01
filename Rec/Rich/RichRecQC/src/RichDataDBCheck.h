
//-----------------------------------------------------------------------------
/** @file RichDataDBCheck.h
 *
 *  Header file for tool : Rich::DAQ::DataDBCheck
 *
 *  CVS Log :-
 *  $Id: RichDataDBCheck.h,v 1.3 2009-09-01 16:09:32 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RICHDATADBCHECK_H
#define RICHRECQC_RICHDATADBCHECK_H 1

// STL
#include <sstream>

// RichKernel
#include "RichKernel/RichHistoAlgBase.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class DataDBCheck RichDataDBCheck.h
     *
     *  Simple monitor to check for consistency errors in the data and the database.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2008-10-14
     */
    //-----------------------------------------------------------------------------
    class DataDBCheck : public Rich::HistoAlgBase
    {

    public:

      /// Standard constructor
      DataDBCheck( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~DataDBCheck( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution

    private:

      /// Compare two pieces of information
      template < class TYPE >
      inline void compare( const std::string & type,
                           const Rich::DAQ::Level0ID l0id,
                           const TYPE & t1,
                           const TYPE & t2 ) const
      {
        if ( t1 != t2 )
        {
          std::ostringstream mess;
          mess << "HPD L0ID " << l0id;
          mess << " " << type << " mismatch : data=" << t1 << " DB=" << t2;
          Error( mess.str(), StatusCode::FAILURE, m_nErrorMess );
        }
      }

    private:

      /// L0ID information
      class L0IDInfo
      {
      public:
        /// Default Constructor
        L0IDInfo() { }
        /// Constructor from data values
        L0IDInfo( const Rich::DAQ::Level1HardwareID _l1HardID,
                  const Rich::DAQ::Level1Input      _l1Input )
          : l1HardID(_l1HardID), l1Input(_l1Input) { }
      public:
        Rich::DAQ::Level1HardwareID l1HardID;
        Rich::DAQ::Level1Input      l1Input;
      };

      /// Information per L0ID
      typedef Rich::Map< Rich::DAQ::Level0ID, L0IDInfo > L0IDInfoCount;

    private:

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_RichSys;

      std::vector<std::string> m_taeEvents; ///< The TAE location(s) to monitor

      /// Number of Error instances to print before supression
      unsigned int m_nErrorMess;

    };

  }
}

#endif // RICHRECQC_RICHDATADBCHECK_H
