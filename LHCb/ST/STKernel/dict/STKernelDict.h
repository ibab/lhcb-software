// $Id: STKernelDict.h,v 1.11 2009-02-17 10:08:17 mneedham Exp $
#ifndef DICT_STKERNELDICT_H 
#define DICT_STKERNELDICT_H 1

#include "Kernel/ISTClusterSelector.h"
#include "Kernel/ISTChannelIDSelector.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/ISTSignalToNoiseTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STBitsPolicy.h"
#include "Kernel/STFun.h"
#include "Kernel/BeetleRepresentation.h"
#include "Kernel/PPRepresentation.h"
#include "Kernel/StripRepresentation.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"

struct _Instantiations{

  STDAQ::BeetleRepresentation m_beetleRep;
  STDAQ::PPRepresentation m_ppRep;
  STDAQ::StripRepresentation m_stripRep;


}; 

#endif // DICT_STKERNELDICT_H
