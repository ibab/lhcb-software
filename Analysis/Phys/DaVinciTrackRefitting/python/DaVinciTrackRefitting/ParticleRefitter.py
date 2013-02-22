# @brief: provides a GaudiSequencer with a track momentum scaler and an update algorithm for the candidate particles on TES.
# @author: Paul Seyfert
# @date: 2013-02-21
# idea mainly by V. Belyaev

def ParticleRefitterSeq(inputs = [],rootInTES = "/Event",scale=True):
  from Configurables import GaudiSequencer
  from Configurables import TrackScaleState as SCALER
  from Configurables import ParticleRefitter
  scaler = SCALER("Scaler", RootInTES = rootInTES)
  seq = GaudiSequencer("ParticleRefitterSeq")
  if scale:
     seq.Members = [ scaler ]
  else:
     seq.Members = [  ]
  refitter = ParticleRefitter()
  if isinstance(inputs, basestring) :
    refitter.Inputs = [inputs]
  else :
    refitter.Inputs = inputs
  refitter.RootInTES = rootInTES
  print "ParticleRefitterSeq is applied to the following inputs:"
  for i in refitter.Inputs:
    print "   - on of the inputs is ", i
  seq.Members += [refitter]
  return seq

