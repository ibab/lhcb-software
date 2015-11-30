#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Author: Elena Graverini
# @Date:   2015-11-30 16:18:57
# @Last Modified by:   Elena Graverini
# @Last Modified time: 2015-11-30 16:56:44

from Gaudi.Configuration import *

def ExcludeSTLayers(ExcludedLayers):
    print "Removing clusters from layers", ExcludedLayers

    from Configurables import STClusterKiller, STLiteClusterKiller, STSelectClustersByChannel, STLayerSelector
    
    ttLayerSelector = STLayerSelector("TTLayerSelector")
    ttLayerSelector.DetType = "TT"
    ttLayerSelector.IgnoredLayers = ExcludedLayers
    ttLayerSelector.OutputLevel = 3
    
    itLayerSelector = STLayerSelector("ITLayerSelector")
    itLayerSelector.DetType = "IT"
    itLayerSelector.IgnoredLayers = ExcludedLayers
    itLayerSelector.OutputLevel = 3
    
    ttLiteClusterKiller = STLiteClusterKiller("TTLiteClusterKiller")
    ttLiteClusterKiller.DetType = "TT"
    ttLiteClusterKiller.SelectorType = "STLayerSelector"
    ttLiteClusterKiller.SelectorName = "TTLayerSelector"
    
    itLiteClusterKiller = STLiteClusterKiller("ITLiteClusterKiller")
    itLiteClusterKiller.DetType = "IT"
    itLiteClusterKiller.SelectorType = "STLayerSelector"
    itLiteClusterKiller.SelectorName = "ITLayerSelector"
    
    ttClusterSelector = STSelectClustersByChannel("TTClusterSelector")
    ttClusterSelector.SelectorType = "STLayerSelector"
    ttClusterSelector.SelectorName = "TTLayerSelector"
    
    itClusterSelector = STSelectClustersByChannel("ITClusterSelector")
    itClusterSelector.SelectorType = "STLayerSelector"
    itClusterSelector.SelectorName = "ITLayerSelector"
    
    ttClusterKiller = STClusterKiller("TTClusterKiller")
    ttClusterKiller.DetType = "TT"
    ttClusterKiller.SelectorType = "STSelectClustersByChannel"
    ttClusterKiller.SelectorName = "TTClusterSelector"
    
    itClusterKiller = STClusterKiller("ITClusterKiller")
    itClusterKiller.DetType = "IT"
    itClusterKiller.SelectorType = "STSelectClustersByChannel"
    itClusterKiller.SelectorName = "ITClusterSelector"
    
    GaudiSequencer("RecoDecodingSeq").Members += [ ttClusterKiller, ttLiteClusterKiller,
                                                   itClusterKiller, itLiteClusterKiller ]