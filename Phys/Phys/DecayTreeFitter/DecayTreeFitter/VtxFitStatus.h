#ifndef VTXFITSTATUS_H
#define VTXFITSTATUS_H

namespace LHCb
{
  class VtxFitStatus
  {
  public:
    enum EFitStatus { UnFitted, Success, Failed, BadInput, NonConverged } ;
  } ;
}


#endif

