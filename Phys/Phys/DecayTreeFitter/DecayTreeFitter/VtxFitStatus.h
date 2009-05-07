#ifndef VTXFITSTATUS_H
#define VTXFITSTATUS_H

namespace Vtx
{
  class FitStatus
  {
  public:
    enum EFitStatus { UnFitted, Success, Failed, BadInput, NonConverged } ;
  } ;
}


#endif

