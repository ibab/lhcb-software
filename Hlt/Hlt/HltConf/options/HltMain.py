from Gaudi.Configuration import *
from Configurables import ApplicationMgr
from Configurables import GaudiSequencer
from Configurables import CreateHltVzero
#/**
# * Common configuration for Hlt1 and Hlt2. Will be included only once.
# *
# * Take care when changing. Make sure changes work for both trigger levels.
# */
#/**
# * defines online reconstruction
# */
importOptions('$HLTCONFROOT/options/HltRecoSequence.py')
#/**
# * Add Ks 
# * @todo Make ProtoParticlea and articles with these Ks
# * @todo See what to do with downstream Ks
# */
#
#/**
# * HltKshort moved to HltProtoparticles to profit from clone killing
# *
#importOptions('$HLTKSHORTROOT/options/HltV0LL.opts')

## Kzero from two long tracks
#GaudiSequencer('HltRecoSequence').Members.append( CreateHltVzero('HltV0LL')  )


#/**
# * main HLT sequencer
# */
Hlt = GaudiSequencer('Hlt', ModeOR= True, ShortCircuit = False
                    , Members = [ GaudiSequencer('Hlt1') 
                                , GaudiSequencer('Hlt2') # NOTE: Hlt2 checks itself whether Hlt1 passed or not
                                , GaudiSequencer('HltEndSequence') 
                                ] )


ApplicationMgr().TopAlg.append( Hlt )

