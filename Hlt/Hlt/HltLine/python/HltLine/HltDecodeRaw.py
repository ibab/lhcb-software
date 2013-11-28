"""
Python module to handle bringing in the Decoders as Hlt members

Ideally this should not be a module, because that fixes *all* the decoders as soon as you ask for any one of them... really it should be a function which returns the correct thing...
"""
from Configurables import DataOnDemandSvc
from GaudiKernel.ProcessJobOptions import importOptions
from HltLine import bindMembers
from DAQSys import Decoders, DecoderClass
from DAQSys.Decoders import DecoderDB
from DAQSys.DecoderClass import decoderToLocation, decodersForBank
#importOptions('$STDOPTS/DecodeRawEvent.py')

def _convert(db,bank) :
    algs=[d.setup() for d in decodersForBank(db,bank)]
    return bindMembers( None, algs).ignoreOutputSelection()

def _convertName(db,name) :
    algs=[db[name].setup()]
    return bindMembers( None, algs).ignoreOutputSelection()

# bind __convert to _convert
__convert = lambda x : _convert( DecoderDB,x) 
__convertName = lambda x : _convertName( DecoderDB,x) 

# __convert = lambda x : bindMembers( None, [] ).ignoreOutputSelection()


DecodeODIN       = __convert( 'ODIN' )
DecodeL0MUON     = __convert( 'L0Muon' )
DecodeL0CALO     = __convert( 'L0Calo' )
DecodeL0FullCALO = __convert( 'L0CaloFull')
#... ^^ nominally returns the same algorithm for both L0CaloFull and L0Calo...
DecodeL0DU       = __convert( 'L0DU' )
DecodeVELO       = __convertName("DecodeVeloRawBuffer/createVeloLiteClusters")
#... ^^ only do the VeloLite clusters, all that's needed for Moore
DecodeTT         = __convert( 'TT' )
DecodeIT         = __convert( 'IT' )
DecodeOT         = __convert( 'OT' )
DecodeMUON       = __convert( 'Muon' )
DecodeRICH       = __convert( 'Rich' )
DecodeECAL       = __convert( 'Ecal' )
DecodeSPD        = __convertName("CaloDigitsFromRaw/SpdFromRaw")
DecodePRS        = __convertName("CaloDigitsFromRaw/PrsFromRaw")
DecodeHCAL       = __convert( 'Hcal' )


#and export explicit (configured!) decoders:
__all__ = ( 'DecodeL0DU', 'DecodeL0MUON', 'DecodeL0CALO'
          , 'DecodeVELO', 'DecodeTT', 'DecodeIT', 'DecodeOT'
          , 'DecodeECAL', 'DecodeSPD', 'DecodePRS', 'DecodeHCAL'
          , 'DecodeMUON', 'DecodeRICH' )

# ==============================================================================
# Register symbols for streamer framework:
# ==============================================================================
from Configurables import LoKi__Hybrid__CoreFactory as Hlt1Factory
_factory = Hlt1Factory ( "Hlt1Factory" )
_names          = lambda obj : [ m.getFullName()  for m in obj.members() ]
#
## register the symbols for streamer framework
#  they can be used both as "gate" and "cause"
#
_factory.Lines += [
    "decodeL0MUON = %s " % _names ( DecodeL0MUON ) ,
    "decodeL0CALO = %s " % _names ( DecodeL0CALO ) ,
    "decodeVELO   = %s " % _names ( DecodeVELO   ) ,
    "decodeTT     = %s " % _names ( DecodeTT     ) ,
    "decodeIT     = %s " % _names ( DecodeIT     ) ,
    "decodeOT     = %s " % _names ( DecodeOT     ) ,
    "decodeECAL   = %s " % _names ( DecodeECAL   ) ,
    "decodeHCAL   = %s " % _names ( DecodeHCAL   ) ,
    "decodeSPD    = %s " % _names ( DecodeSPD    ) ,
    "decodePRS    = %s " % _names ( DecodePRS    ) ,
    "decodeMUON   = %s " % _names ( DecodeMUON   ) ,
    "decodeRICH   = %s " % _names ( DecodeRICH   )
    ]

# ==============================================================================
# The END 
# ==============================================================================
