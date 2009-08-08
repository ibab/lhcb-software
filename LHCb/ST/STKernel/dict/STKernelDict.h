// $Id: STKernelDict.h,v 1.13 2009-08-08 10:48:42 mneedham Exp $
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
#include "Kernel/ITDetectorPlot.h"
#include "Kernel/TTDetectorPlot.h"


struct _Instantiations{

  STDAQ::BeetleRepresentation m_beetleRep;
  STDAQ::PPRepresentation m_ppRep;
  STDAQ::StripRepresentation m_stripRep;

  STDAQ::chanPair m_chanPair;
  
  ST::ITDetectorPlot m_itdetPlot;
  ST::TTDetectorPlot m_ttdetPlot;


}; 

#endif // DICT_STKERNELDICT_H
