// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"


#include "GaussRICH/RichSensDet.h"
#include "GaussRICH/RichG4TrackActionPhotOpt.h"
#include "GaussRICH/RichG4TrackAction.h"
#include "GaussRICH/RichG4TrackActionAerogelPhoton.h"
#include "GaussRICH/RichG4RunAction.h"
#include "GaussRICH/RichG4EventAction.h"
#include "GaussRICH/Rich1G4TrackActionUpstrPhoton.h"
#include "GaussRICH/GiGaPhysConstructorHpd.h"
#include "GaussRICH/GiGaPhysConstructorOp.h"
#include "GaussRICH/GetMCRichHitsAlg.h"
#include "GaussRICH/GetMCRichOpticalPhotonsAlg.h"
#include "GaussRICH/GetMCRichSegmentsAlg.h"
#include "GaussRICH/GetMCRichTracksAlg.h"
#include "GaussRICH/RichG4StepAnalysis3.h"
#include "GaussRICH/RichG4StepAnalysis4.h"
#include "GaussRICH/RichG4StepAnalysis5.h"
#include "GaussRICH/RichG4StepAnalysis6.h"



// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( RichSensDet );
DECLARE_TOOL_FACTORY( RichG4TrackActionPhotOpt );

DECLARE_TOOL_FACTORY( RichG4TrackAction );

DECLARE_TOOL_FACTORY( RichG4TrackActionAerogelPhoton );
DECLARE_TOOL_FACTORY( RichG4RunAction );
DECLARE_TOOL_FACTORY( RichG4EventAction );
DECLARE_TOOL_FACTORY( Rich1G4TrackActionUpstrPhoton );
DECLARE_TOOL_FACTORY( GiGaPhysConstructorHpd );
DECLARE_TOOL_FACTORY( GiGaPhysConstructorOp );
DECLARE_ALGORITHM_FACTORY( GetMCRichHitsAlg );
DECLARE_ALGORITHM_FACTORY( GetMCRichOpticalPhotonsAlg );
DECLARE_ALGORITHM_FACTORY( GetMCRichSegmentsAlg );
DECLARE_ALGORITHM_FACTORY( GetMCRichTracksAlg );

DECLARE_TOOL_FACTORY(RichG4StepAnalysis3);
DECLARE_TOOL_FACTORY(RichG4StepAnalysis4);
DECLARE_TOOL_FACTORY(RichG4StepAnalysis5);
DECLARE_TOOL_FACTORY(RichG4StepAnalysis6);


