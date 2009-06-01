## @package HltConf
#  Base configurables for the Hlt lines
#  @author Gerhard Raven  (Gerhard.Raven@nikhef.nl)
#  @date   25/03/2009

__version__ = "$Id: HltLinesConfigurableUser.py,v 1.4 2009-06-01 20:34:18 graven Exp $"
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

    def _scale(self,line,arg) :
        import re
        for (expr,value) in self.getProp(arg).iteritems() :
            if re.match(expr,line) : return value
        return 1
    def postscale(self,line) : return self._scale(line,'Postscale')
    def prescale(self,line)  : return self._scale(line,'Prescale')
