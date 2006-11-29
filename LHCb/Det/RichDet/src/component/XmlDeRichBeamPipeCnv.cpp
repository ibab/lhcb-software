#ifndef XMLDERICHBEAMPIPECNV_H 
#define XMLDERICHBEAMPIPECNV_H 1

// Include files

/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"

#include "RichDet/DeRichBeamPipe.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichBeamPipe>  XmlDeRichBeamPipeCnv;
static CnvFactory<XmlDeRichBeamPipeCnv> s_DeRichBeamPipeFactory;
const ICnvFactory& XmlDeRichBeamPipeCnvFactory = s_DeRichBeamPipeFactory;

#endif // XMLDERICHBEAMPIPECNV_H
