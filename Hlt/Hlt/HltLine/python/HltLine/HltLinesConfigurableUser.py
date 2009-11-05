## @package HltConf
#  Base configurables for the Hlt lines
#  @author Gerhard Raven  (Gerhard.Raven@nikhef.nl)
#  @date   25/03/2009

__version__ = "$Id: HltLinesConfigurableUser.py,v 1.7 2009-11-05 12:54:31 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from LHCbKernel.Configuration import LHCbConfigurableUser

# ----------------------------------------------------------------------------------

## @class HltLinesConfigurableUser
#  Base configurable for HltLines
#  @author Gerhard Raven  (Gerhard.Raven@nikhef.nl)
#  @date   25/03/2009
class HltLinesConfigurableUser(LHCbConfigurableUser):

    ## Default options, common to all specializations of  HltLinesConfigurable
    __slots__ = { 'Prescale'  : {} 
                , 'Postscale' : {}
                }
    # what if derived class overrules this? Try to hook in __init__ instead...

    #def initialize(self) :
    #    self.Prescale = {}
    #    self.Postscale = {}
    #def __init__(self, *args, **kwargs )
    #    self.Prescale = {}
    #    self.Postscale = {}
    #    apply(super(HltLinesConfigurableUser,self).__init__,(),args,kwargs)
        

    def _scale(self,line,arg) :
        import re
        for (expr,value) in self.getProp(arg).iteritems() :
            m =  re.match(expr,line) 
            #  do not accept partial matches
            if m and m.span()[-1] == len(line) : return value
        return 1
    def postscale(self,line) : return self._scale(line,'Postscale')
    def prescale(self,line)  : return self._scale(line,'Prescale')

    def getProps(self) :
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
            
            
