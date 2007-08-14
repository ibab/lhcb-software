// $Id: TsaBaseAlg.h,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
#ifndef _TSABASEALG_H_
#define _TSABASEALG_H_

//#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
//#include "TsaKernel/stopwatch.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class BaseAlg
     *
     *  Base class for Tsa Initialization
     *
     *  @author M.Needham
     *  @date   07/03/2005
     */
    class BaseAlg: public GaudiAlgorithm
    {

    public:

      // Constructors and destructor
      BaseAlg(const std::string& name,
                 ISvcLocator* pSvcLocator);
      virtual ~BaseAlg();

      // IAlgorithm members
      virtual StatusCode initialize();
      virtual StatusCode finalize();

    };

  }
}

#endif // _TSABASEALG_H_
