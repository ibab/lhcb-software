

//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************



//Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

#include <string>

#include "RichGlobalPIDSummary_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichGlobalPIDSummary_dict
//------------------------------------------------------------------------------
{
public:
  RichGlobalPIDSummary_dict();
};

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_addToEventLL_0(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->addToEventLL(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedTracks_1(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedTracks_2(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedVeloTracks_3(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedVeloTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedVeloTracks_4(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedVeloTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedSeedTracks_5(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedSeedTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedSeedTracks_6(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedSeedTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedMatchTracks_7(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedMatchTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedMatchTracks_8(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedMatchTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedForwardTracks_9(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedForwardTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedForwardTracks_10(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedForwardTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedUpstreamTracks_11(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedUpstreamTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedUpstreamTracks_12(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedUpstreamTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_usedVeloTTTracks_13(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->usedVeloTTTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setUsedVeloTTTracks_14(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setUsedVeloTTTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalTracks_15(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalTracks_16(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalVeloTracks_17(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalVeloTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalVeloTracks_18(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalVeloTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalSeedTracks_19(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalSeedTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalSeedTracks_20(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalSeedTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalMatchTracks_21(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalMatchTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalMatchTracks_22(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalMatchTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalForwardTracks_23(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalForwardTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalForwardTracks_24(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalForwardTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalUpstreamTracks_25(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalUpstreamTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalUpstreamTracks_26(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalUpstreamTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_totalVeloTTTracks_27(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->totalVeloTTTracks();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setTotalVeloTTTracks_28(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setTotalVeloTTTracks(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_nRich1HitPixels_29(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->nRich1HitPixels();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setNRich1HitPixels_30(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setNRich1HitPixels(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_nRich2HitPixels_31(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->nRich2HitPixels();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setNRich2HitPixels_32(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setNRich2HitPixels(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_eventLL_33(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichGlobalPIDSummary*)v)->eventLL();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setEventLL_34(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setEventLL(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_eventProcessedOK_35(void* v)
//------------------------------------------------------------------------------
{
  static bool ret;
  ret =  ((RichGlobalPIDSummary*)v)->eventProcessedOK();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setEventProcessedOK_36(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setEventProcessedOK(*(bool*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_eventIteration_37(void* v)
//------------------------------------------------------------------------------
{
  static int ret;
  ret =  ((RichGlobalPIDSummary*)v)->eventIteration();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setEventIteration_38(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setEventIteration(*(int*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_rich1BckEstimate_39(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret =  ((RichGlobalPIDSummary*)v)->rich1BckEstimate();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setRich1BckEstimate_40(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setRich1BckEstimate(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_rich2BckEstimate_41(void* v)
//------------------------------------------------------------------------------
{
  static double ret;
  ret =  ((RichGlobalPIDSummary*)v)->rich2BckEstimate();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichGlobalPIDSummary_setRich2BckEstimate_42(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichGlobalPIDSummary*)v)->setRich2BckEstimate(*(double*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichGlobalPIDSummary_constructor_1()
//------------------------------------------------------------------------------
{
  static RichGlobalPIDSummary* ret = new RichGlobalPIDSummary();
  return ret;
}

//------------------------------------------------------------------------------
static RichGlobalPIDSummary_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichGlobalPIDSummary_dict::RichGlobalPIDSummary_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichGlobalPIDSummary",
                "Summary object for RICH Global PID analysis",
                0);

  RichGlobalPIDSummary* cl = new RichGlobalPIDSummary();
  metaC->addSuperClass("DataObject",
                (((int)cl)-((int)((DataObject*)cl))));
  delete cl;

  metaC->addConstructor("default constructor",
                RichGlobalPIDSummary_constructor_1);

  metaC->addField("usedTracks",
                "int",
                "Total number of used tracks",
                &((RichGlobalPIDSummary*)0)->m_usedTracks,
                MetaModifier::setPrivate());

  metaC->addField("usedVeloTracks",
                "int",
                "Number of velo tracks used in global PID analysis",
                &((RichGlobalPIDSummary*)0)->m_usedVeloTracks,
                MetaModifier::setPrivate());

  metaC->addField("usedSeedTracks",
                "int",
                "Number of seed tracks used in global PID analysis",
                &((RichGlobalPIDSummary*)0)->m_usedSeedTracks,
                MetaModifier::setPrivate());

  metaC->addField("usedMatchTracks",
                "int",
                "Number of match tracks used in global PID analysis",
                &((RichGlobalPIDSummary*)0)->m_usedMatchTracks,
                MetaModifier::setPrivate());

  metaC->addField("usedForwardTracks",
                "int",
                "Number of forward tracks used in global PID analysis",
                &((RichGlobalPIDSummary*)0)->m_usedForwardTracks,
                MetaModifier::setPrivate());

  metaC->addField("usedUpstreamTracks",
                "int",
                "Number of upstream tracks used in global PID analysis",
                &((RichGlobalPIDSummary*)0)->m_usedUpstreamTracks,
                MetaModifier::setPrivate());

  metaC->addField("usedVeloTTTracks",
                "int",
                "Number of Velo-TT tracks used in global PID analysis",
                &((RichGlobalPIDSummary*)0)->m_usedVeloTTTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalTracks",
                "int",
                "Total number of tracks in the event",
                &((RichGlobalPIDSummary*)0)->m_totalTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalVeloTracks",
                "int",
                "Total number of velo tracks in event",
                &((RichGlobalPIDSummary*)0)->m_totalVeloTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalSeedTracks",
                "int",
                "Total number of seed tracks in event",
                &((RichGlobalPIDSummary*)0)->m_totalSeedTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalMatchTracks",
                "int",
                "Total number of match tracks in event",
                &((RichGlobalPIDSummary*)0)->m_totalMatchTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalForwardTracks",
                "int",
                "Total number of forward tracks in event",
                &((RichGlobalPIDSummary*)0)->m_totalForwardTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalUpstreamTracks",
                "int",
                "Total number of upstream tracks in event",
                &((RichGlobalPIDSummary*)0)->m_totalUpstreamTracks,
                MetaModifier::setPrivate());

  metaC->addField("totalVeloTTTracks",
                "int",
                "Total number of velo-TT tracks in event",
                &((RichGlobalPIDSummary*)0)->m_totalVeloTTTracks,
                MetaModifier::setPrivate());

  metaC->addField("nRich1HitPixels",
                "int",
                "Number of hit pixels in Rich1",
                &((RichGlobalPIDSummary*)0)->m_nRich1HitPixels,
                MetaModifier::setPrivate());

  metaC->addField("nRich2HitPixels",
                "int",
                "Number of hit pixels in Rich2",
                &((RichGlobalPIDSummary*)0)->m_nRich2HitPixels,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("eventLL",
                "std::vector<float>",
                "History of overall event LogLikelihood values",
                &((RichGlobalPIDSummary*)0)->m_eventLL,
                MetaModifier::setPrivate());

  metaC->addField("eventProcessedOK",
                "bool",
                "Boolean to indicate if the event was successfully processed",
                &((RichGlobalPIDSummary*)0)->m_eventProcessedOK,
                MetaModifier::setPrivate());

  metaC->addField("eventIteration",
                "int",
                "Current event iteration",
                &((RichGlobalPIDSummary*)0)->m_eventIteration,
                MetaModifier::setPrivate());

  metaC->addField("rich1BckEstimate",
                "double",
                "Esitamte of overall background in Rich1",
                &((RichGlobalPIDSummary*)0)->m_rich1BckEstimate,
                MetaModifier::setPrivate());

  metaC->addField("rich2BckEstimate",
                "double",
                "Esitamte of overall background in Rich2",
                &((RichGlobalPIDSummary*)0)->m_rich2BckEstimate,
                MetaModifier::setPrivate());

  argTypes.clear();
  argTypes.push_back("double");
  metaC->addMethod("addToEventLL",
                "Push back this value in event LL vector",
                argTypes,
                RichGlobalPIDSummary_addToEventLL_0);

  metaC->addMethod("usedTracks",
                "Total number of used tracks",
                "int",
                RichGlobalPIDSummary_usedTracks_1);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedTracks",
                "Total number of used tracks",
                argTypes,
                RichGlobalPIDSummary_setUsedTracks_2);

  metaC->addMethod("usedVeloTracks",
                "Number of velo tracks used in global PID analysis",
                "int",
                RichGlobalPIDSummary_usedVeloTracks_3);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedVeloTracks",
                "Number of velo tracks used in global PID analysis",
                argTypes,
                RichGlobalPIDSummary_setUsedVeloTracks_4);

  metaC->addMethod("usedSeedTracks",
                "Number of seed tracks used in global PID analysis",
                "int",
                RichGlobalPIDSummary_usedSeedTracks_5);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedSeedTracks",
                "Number of seed tracks used in global PID analysis",
                argTypes,
                RichGlobalPIDSummary_setUsedSeedTracks_6);

  metaC->addMethod("usedMatchTracks",
                "Number of match tracks used in global PID analysis",
                "int",
                RichGlobalPIDSummary_usedMatchTracks_7);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedMatchTracks",
                "Number of match tracks used in global PID analysis",
                argTypes,
                RichGlobalPIDSummary_setUsedMatchTracks_8);

  metaC->addMethod("usedForwardTracks",
                "Number of forward tracks used in global PID analysis",
                "int",
                RichGlobalPIDSummary_usedForwardTracks_9);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedForwardTracks",
                "Number of forward tracks used in global PID analysis",
                argTypes,
                RichGlobalPIDSummary_setUsedForwardTracks_10);

  metaC->addMethod("usedUpstreamTracks",
                "Number of upstream tracks used in global PID analysis",
                "int",
                RichGlobalPIDSummary_usedUpstreamTracks_11);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedUpstreamTracks",
                "Number of upstream tracks used in global PID analysis",
                argTypes,
                RichGlobalPIDSummary_setUsedUpstreamTracks_12);

  metaC->addMethod("usedVeloTTTracks",
                "Number of Velo-TT tracks used in global PID analysis",
                "int",
                RichGlobalPIDSummary_usedVeloTTTracks_13);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setUsedVeloTTTracks",
                "Number of Velo-TT tracks used in global PID analysis",
                argTypes,
                RichGlobalPIDSummary_setUsedVeloTTTracks_14);

  metaC->addMethod("totalTracks",
                "Total number of tracks in the event",
                "int",
                RichGlobalPIDSummary_totalTracks_15);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalTracks",
                "Total number of tracks in the event",
                argTypes,
                RichGlobalPIDSummary_setTotalTracks_16);

  metaC->addMethod("totalVeloTracks",
                "Total number of velo tracks in event",
                "int",
                RichGlobalPIDSummary_totalVeloTracks_17);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalVeloTracks",
                "Total number of velo tracks in event",
                argTypes,
                RichGlobalPIDSummary_setTotalVeloTracks_18);

  metaC->addMethod("totalSeedTracks",
                "Total number of seed tracks in event",
                "int",
                RichGlobalPIDSummary_totalSeedTracks_19);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalSeedTracks",
                "Total number of seed tracks in event",
                argTypes,
                RichGlobalPIDSummary_setTotalSeedTracks_20);

  metaC->addMethod("totalMatchTracks",
                "Total number of match tracks in event",
                "int",
                RichGlobalPIDSummary_totalMatchTracks_21);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalMatchTracks",
                "Total number of match tracks in event",
                argTypes,
                RichGlobalPIDSummary_setTotalMatchTracks_22);

  metaC->addMethod("totalForwardTracks",
                "Total number of forward tracks in event",
                "int",
                RichGlobalPIDSummary_totalForwardTracks_23);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalForwardTracks",
                "Total number of forward tracks in event",
                argTypes,
                RichGlobalPIDSummary_setTotalForwardTracks_24);

  metaC->addMethod("totalUpstreamTracks",
                "Total number of upstream tracks in event",
                "int",
                RichGlobalPIDSummary_totalUpstreamTracks_25);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalUpstreamTracks",
                "Total number of upstream tracks in event",
                argTypes,
                RichGlobalPIDSummary_setTotalUpstreamTracks_26);

  metaC->addMethod("totalVeloTTTracks",
                "Total number of velo-TT tracks in event",
                "int",
                RichGlobalPIDSummary_totalVeloTTTracks_27);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setTotalVeloTTTracks",
                "Total number of velo-TT tracks in event",
                argTypes,
                RichGlobalPIDSummary_setTotalVeloTTTracks_28);

  metaC->addMethod("nRich1HitPixels",
                "Number of hit pixels in Rich1",
                "int",
                RichGlobalPIDSummary_nRich1HitPixels_29);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setNRich1HitPixels",
                "Number of hit pixels in Rich1",
                argTypes,
                RichGlobalPIDSummary_setNRich1HitPixels_30);

  metaC->addMethod("nRich2HitPixels",
                "Number of hit pixels in Rich2",
                "int",
                RichGlobalPIDSummary_nRich2HitPixels_31);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setNRich2HitPixels",
                "Number of hit pixels in Rich2",
                argTypes,
                RichGlobalPIDSummary_setNRich2HitPixels_32);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("eventLL",
                "History of overall event LogLikelihood values",
                "std::vector<float>",
                RichGlobalPIDSummary_eventLL_33);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setEventLL",
                "History of overall event LogLikelihood values",
                argTypes,
                RichGlobalPIDSummary_setEventLL_34);

  metaC->addMethod("eventProcessedOK",
                "Boolean to indicate if the event was successfully processed",
                "bool",
                RichGlobalPIDSummary_eventProcessedOK_35);

  argTypes.clear();
  argTypes.push_back("bool");
  metaC->addMethod("setEventProcessedOK",
                "Boolean to indicate if the event was successfully processed",
                argTypes,
                RichGlobalPIDSummary_setEventProcessedOK_36);

  metaC->addMethod("eventIteration",
                "Current event iteration",
                "int",
                RichGlobalPIDSummary_eventIteration_37);

  argTypes.clear();
  argTypes.push_back("int");
  metaC->addMethod("setEventIteration",
                "Current event iteration",
                argTypes,
                RichGlobalPIDSummary_setEventIteration_38);

  metaC->addMethod("rich1BckEstimate",
                "Esitamte of overall background in Rich1",
                "double",
                RichGlobalPIDSummary_rich1BckEstimate_39);

  argTypes.clear();
  argTypes.push_back("double");
  metaC->addMethod("setRich1BckEstimate",
                "Esitamte of overall background in Rich1",
                argTypes,
                RichGlobalPIDSummary_setRich1BckEstimate_40);

  metaC->addMethod("rich2BckEstimate",
                "Esitamte of overall background in Rich2",
                "double",
                RichGlobalPIDSummary_rich2BckEstimate_41);

  argTypes.clear();
  argTypes.push_back("double");
  metaC->addMethod("setRich2BckEstimate",
                "Esitamte of overall background in Rich2",
                argTypes,
                RichGlobalPIDSummary_setRich2BckEstimate_42);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones    (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12431");
  metaC->setPropertyList(pl);
}


