from LbUtils.Script import Script
from LbUtils.CVS import CVS2Version
from LbConfiguration.Project import project_list, getProject

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.2 $")

class LbShowProjectConfScript(Script):
    _version = __version__
    def defineOpts(self):
        pass 
    def main(self):
        projname = None
        if len(self.args) > 0:
            projname = self.args[0]
                
        options = self.options

        if projname == None :
            for p in project_list:
                print p
        else :
            print getProject(projname)


if __name__ == '__main__':
    s = LbShowProjectConfScript(usage="%prog [options] projectname")
    s.run()
  