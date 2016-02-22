// $Id: TimeStamps.h,v 1.1 2009-09-08 16:22:39 krinnert Exp $
#ifndef TIMESTAMPS_H 
#define TIMESTAMPS_H 1

#include <map>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "Tell1Kernel/ITimeStampProvider.h" 


namespace Velo
{
  /** @class TimeStamps TimeStamps.h
   *
   *  Implementation of TimeStampProvider interface.
   *
   *  Provides time stamps in the format
   *
   *    YYYY-MM-DD_HH.MM.SS
   *
   *  @author Kurt Rinnert
   *  @date   2009-08-21
   */
  class TimeStamps : public GaudiTool, virtual public Velo::ITimeStampProvider {

    public: 
      /// Standard constructor
      TimeStamps( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

      virtual ~TimeStamps( ); ///< Destructor

      virtual StatusCode initialize(); ///< Sets init time stamp
      virtual StatusCode finalize(); 

      virtual std::string initTime(); ///< time of initialisation

      virtual std::string now(); ///< current time

    private:

      // time stamps
      std::string m_initTime;

  };
}
#endif // TIMESTAMPS_H 
