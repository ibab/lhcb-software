// $Id: TfKernelDict.h,v 1.3 2007-09-10 08:54:31 wouter Exp $
#ifndef DICT_TFKERNELDICT_H 
#define DICT_TFKERNELDICT_H 1

#include "TfKernel/TfIDTypes.h"

#include "TfKernel/OTHit.h"
#include "TfKernel/STHit.h"
#include "TfKernel/VeloRHit.h"
#include "TfKernel/VeloPhiHit.h"

#include "TfKernel/Region.h"
#include "TfKernel/RegionID.h"

#include "TfKernel/ITTHitCreator.h"
#include "TfKernel/IITHitCreator.h"
#include "TfKernel/IOTHitCreator.h"
#include "TfKernel/ISTHitCleaner.h"
#include "TfKernel/IOTHitCleaner.h"

// instantiate some templated classes, to get them into the dictionary
namespace 
{
  struct _Instantiations 
  {
    Tf::Envelope<Tf::STHit::DetectorElementType> tf_envelope_sthit;
    Tf::Envelope<Tf::OTHit::DetectorElementType> tf_envelope_othit;
  };
}

#endif // DICT_TFKERNELDICT_H
