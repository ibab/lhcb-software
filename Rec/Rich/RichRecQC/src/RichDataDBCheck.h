
//-----------------------------------------------------------------------------
/** @file RichDataDBCheck.h
 *
 *  Header file for tool : Rich::DAQ::DataDBCheck
 *
 *  CVS Log :-
 *  $Id: RichDataDBCheck.h,v 1.2 2009-08-26 10:01:05 rogers Exp $
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
     *  Simple monitor to compare the decoded data against the information
     *  In the DB (DeRichSystem).
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
          mess << " " << type << " mismatch : " << t1 << " " << t2;
          Error( mess.str(), StatusCode::FAILURE, 999 );
        }
      }

    private:

      /// Raw Buffer Decoding tool
      const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

      /// Pointer to RICH system detector element
      const DeRichSystem * m_RichSys;

      std::vector<std::string> m_taeEvents; ///< The TAE location(s) to monitor

    };

  }
}

#endif // RICHRECQC_RICHDATADBCHECK_H
