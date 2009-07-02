
from Configurables import DataOnDemandSvc
from GaudiKernel.ProcessJobOptions import importOptions
from Gaudi.Configuration import allConfigurables
from HltLine.HltLine import bindMembers
importOptions('$STDOPTS/DecodeRawEvent.py')
_map = DataOnDemandSvc().AlgMap

def _convert(x) :
    a = x.split('/')
    if len(a) == 2 :
        (t,n) = a
    elif len(a) == 1 :
        (t,n) = (a[0],a[0])
    exec 'from Configurables import %s' % t
    exec 'c = %s("%s")'%(t,n)
    return bindMembers( None, [ c ]).ignoreOutputSelection()

DecodeODIN   = _convert( _map.pop("DAQ/ODIN") )
DecodeL0MUON = _convert( _map.pop('Trig/L0/MuonCtrl') ) 
DecodeL0CALO = _convert(_map.pop('Trig/L0/Calo') ) 
DecodeL0DU =  _convert( _map.pop('Trig/L0/L0DUReport') )
DecodeVELO =  _convert( _map.pop("Raw/Velo/LiteClusters") )
DecodeTT   =  _convert( _map.pop("Raw/TT/LiteClusters") )
DecodeIT   =  _convert( _map.pop("Raw/IT/LiteClusters") )
DecodeOT   =  _convert( _map.pop("Raw/OT/Times") )
#decodeMUON = _map.pop("Raw/Muon/Coords")
#decodeRICH = _map.pop("Raw/Rich/Digits")
DecodeECAL =  _convert( _map.pop("Raw/Ecal/Digits") )

# disable everything
for i in _map.keys() : _map.pop(i)

#and export explicit (configured!) decoders:
__all__ = ( 'DecodeL0DU', 'DecodeL0MUON', 'DecodeL0CALO', 'DecodeVELO', 'DecodeTT', 'DecodeIT', 'DecodeOT', 'DecodeECAL' )
