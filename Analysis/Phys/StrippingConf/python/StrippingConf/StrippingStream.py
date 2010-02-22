#!/usr/bin/env python

#
# StrippingStream
#

__all__ = ( 
	    'StrippingStream'
	  )

from GaudiConf.Configuration import *
from Gaudi.Configuration import GaudiSequencer, Sequencer, Configurable
from Configurables import LHCbConfigurableUser


class StrippingStream ( LHCbConfigurableUser ) :

    __slots__ = { 
		  "Lines" : [], 
                  "LineConfigurables" : [] 
                }

    def __apply_configuration__ ( self ) :
        log.info("Configuring StrippingStream " + self.name() )
        

    def appendLines (self, lines) : 
	for line in lines : 
	    self.lineConfigurables().append(line)

	    
    def createConfigurables(self, TES = False, 
				  TESPrefix = 'Strip', 
				  HDRLocation = 'Phys/DecReports', 
				  Settings = None) :
        from Configurables import StrippingCheck

# First apply settings to configurable lines

	log.info("Applying settings to stream " + self.name())
        
        if Settings : 
    	    if hasattr(Settings,'Thresholds') : 
    		log.info("Applying thresholds")
    		for (lineconf,thr) in Settings.Thresholds().iteritems() : 
    		    if lineconf in self.lineConfigurables() : 
    			log.info("   to configurable line %s" % type(lineconf))
    			for (cut,value) in thr.iteritems() : 
    			    if hasattr(lineconf,cut) : 
    				log.info("      %s.%s = %s" % (type(lineconf),cut,value))
    				setattr(lineconf,cut,value)


# Append lines to stream 

	from StrippingLine import StrippingLine
	for line in self.lineConfigurables() : 
	    print type(line)
	    if StrippingLine is type(line) : 
		self.lines().append(line)
		line.declareAppended()
	    elif hasattr(line, 'lines') : 
		confLines = line.lines()
		for confLine in confLines :
		    if StrippingLine is type(confLine) : 
			self.lines().append(confLine)
			confLine.declareAppended()

# Create configurables
        
	for line in self.lines() : 
	    if TES :
		self.sequence().Members += [ StrippingCheck(line.name(), 
				     InputLocation = "/Event/" + TESPrefix + "/" + line.outputLocation() + "/Particles") ]
	    else :  
		line.createConfigurable( TESPrefix + "/" + HDRLocation )
		self.sequence().Members += [ line.configurable() ]


    def lines( self ) :
        return self.getProp('Lines')

    def lineConfigurables( self ) :
        return self.getProp('LineConfigurables')


    def sequence ( self ) : 
	return GaudiSequencer("StrippingStream"+self.name(),
                              ModeOR = True,
                              ShortCircuit = False)


    def outputLocations (self) : 
	outputList = []
	for i in self.lines() :
	    output = i.outputLocation()
	    if output : 
		outputList.append(output)
	return outputList
