#!/usr/bin/env python
from Gaudi.Configuration import *
from Configurables import ( DaVinci, MessageSvc, FilterDesktop )
from PhysSelPython.Wrappers import ( AutomaticData, Selection,
                                     SelectionSequence, MultiSelectionSequence )
from DSTWriters.__dev__.Configuration import SelDSTWriter, stripDSTElements, stripDSTStreamConf
from DSTWriters.__dev__.microdstelements import *

location = '/Event/Dimuon/Phys/BetaSBu2JpsiKDetachedLine'
input_data = AutomaticData( Location = location + '/Particles' )

dst_selection = 'Bu2JpsiKUnbiased'
from BetaSReaders import DSTSelections
offline = getattr( DSTSelections, dst_selection )()

dst_filter = FilterDesktop( 'OfflineFilter' )
dst_filter.Code = offline.cut() % offline.selection()
DSTSelection = Selection( "DSTSelection",
                          Algorithm = dst_filter,
                          RequiredSelections = [ input_data ] )

DSTSeq = SelectionSequence( "Bu2JpsiK", TopSelection = DSTSelection )

reco_leaves = [ '/Event/pRec#3',
                '/Event/Rec/Status#1',
                '/Event/Rec/Header#1',
                '/Event/DAQ/RawEvent#1' ]

dv_leaves = [ '/Event/Dimuon/Rec/Vertex/Primary#1',
              location + '#2',
              '/Event/Dimuon/Phys/Bu2JpsiKBetaS#2',
              '/Event/Dimuon/Phys/KaonsForBetaSBetaS#2',
              '/Event/Dimuon/Phys/StdLooseKaons#2',
              '/Event/Dimuon/Phys/StdMassConstrainedJpsi2MuMu#2',
              '/Event/Dimuon/Phys/StdLooseMuons#2' ]

from DSTWriters.__dev__.streamconf import OutputStreamConf
from Configurables import OutputStream
streamConf = OutputStreamConf( streamType = OutputStream,
                               fileExtension = '.dst',
                               extraItems = reco_leaves + dv_leaves )

SelDSTWriterElements = { 'default' : [] }
SelDSTWriterConf = { 'default' : streamConf }

dstWriter = SelDSTWriter( 'SelDST',
                          StreamConf = SelDSTWriterConf,
 JpsiKS_JpsiLambda_Stripping10.py                           MicroDSTElements = SelDSTWriterElements,
                          WriteFSR = False,
                          OutputFileSuffix = "Offline",
                          SelectionSequences = [ DSTSeq ] )

## More space for output
MessageSvc().Format = "% F%50W%S%15W%R%T %0W%M"

## DaVinci Configurable
dv = DaVinci (
  DataType   = '2010',
  Simulation = False,
  HistogramFile = 'histos.root',
  EvtMax = -1
  )

dv.appendToMainSequence( [ dstWriter.sequence() ] )
