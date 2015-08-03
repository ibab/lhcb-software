#ifndef XMLDERICHBEAMPIPECNV_H 
#define XMLDERICHBEAMPIPECNV_H 1

// Include files
#include "RichDet/DeRichBeamPipe.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichBeamPipe>  XmlDeRichBeamPipeCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichBeamPipeCnv )

#endif // XMLDERICHBEAMPIPECNV_H
