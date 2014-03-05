from Gaudi.Configuration import *

def upANN():
    from Configurables import HltANNSvc
    if "PatForwardTool/PestiForward" in HltANNSvc().InfoID:
        print "It is certainly there in InfoID!!", HltANNSvc().InfoID["PatForwardTool/PestiForward"]
    if "PatForwardTool/PestiForward" in HltANNSvc().Hlt1SelectionID:
        print "It is certainly there in SelectionID!!",  HltANNSvc().Hlt1SelectionID["PatForwardTool/PestiForward"]
    HltANNSvc().Hlt1SelectionID["PatForwardTool/PestiForward"]= 106302
    HltANNSvc().InfoID["PatForwardTool/PestiForward"]= 6302

appendPostConfigAction(upANN)
