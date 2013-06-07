// $Id: ITsaStubExtender.h,v 1.3 2007-10-09 18:03:38 smenzeme Exp $
#ifndef _ITsaStubExtender_H
#define _ITsaStubExtender_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITsaStubExtender( "ITsaStubExtender", 0, 0 );

#include <vector>

namespace Tf
{
  namespace Tsa
  {

    class SeedStub;
    class SeedTrack;
    class SeedHit;

    /** @class ITsaStubExtender ITsaStubExtender.h TsaKernel/ITsaStubExtender.h
     *
     *  Interface to Tsa Stub extender
     *
     *  @author M.Needham
     *  @date   12/11/2006
     */

    class ITsaStubExtender: virtual public IAlgTool
    {
    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ITsaStubExtender ; }

      virtual StatusCode execute(int& sect, std::vector<SeedStub*> stubs[], std::vector<SeedHit*> hits[],
                                 std::vector<SeedHit*> sHits[], std::vector<SeedTrack*>& seeds )= 0;

    };

  }
}

#endif
