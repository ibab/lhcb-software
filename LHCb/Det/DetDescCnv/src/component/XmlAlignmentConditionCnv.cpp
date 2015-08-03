// $Id: XmlAlignmentConditionCnv.cpp,v 1.2 2006-12-14 13:14:09 ranjard Exp $
// Include files 
#ifndef XMLALIGNMENTCONDITION_H 
#define XMLALIGNMENTCONDITION_H 1

#ifdef __INTEL_COMPILER         // Disable ICC remark from Math headers
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif

// Include files
#include "DetDesc/AlignmentCondition.h"
#include "DetDescCnv/XmlUserConditionCnv.h"

typedef  XmlUserConditionCnv<AlignmentCondition>  XmlAlignmentConditionCnv;
DECLARE_CONVERTER_FACTORY(XmlAlignmentConditionCnv)

#endif // XMLALIGNMENTCONDITION_H
