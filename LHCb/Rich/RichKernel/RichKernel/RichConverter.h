// $Id: RichConverter.h,v 1.2 2009-09-30 07:48:53 jonrob Exp $
#ifndef RICHKERNEL_RICHCONVERTER_H
#define RICHKERNEL_RICHCONVERTER_H 1

// local
#include "RichKernel/RichConverter_Imp.h"
#include "RichKernel/RichCommonBase.h"

namespace Rich
{

  /** @class Converter RichKernel/RichConverter.h
   *
   *  Base class for RICH converters
   *
   *  @author Chris Jones
   *  @date   2009-07-07
   */
  class Converter : public Rich::CommonBase<Rich::Converter_Imp>
  {

  public:

    /// Standard constructor
    Converter ( long storage_type,
                const CLID &class_type,
                ISvcLocator *svc = NULL )
      : Rich::CommonBase<Rich::Converter_Imp> ( storage_type, class_type, svc )
    { }

  public:

    /// Forward Print method
    inline StatusCode Print ( const std::string & Message                       ,
                              const MSG::Level    level   = MSG::INFO           ,
                              const StatusCode    Status  = StatusCode::SUCCESS ) const
    {
      return this->Rich::CommonBase<Rich::Converter_Imp>::Print(Message,level,Status);
    }

  };

}

#endif // RICHKERNEL_RICHCONVERTER_H
