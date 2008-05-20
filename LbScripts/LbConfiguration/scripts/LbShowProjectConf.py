from LbUtils.Script import Script
from LbConfiguration.Project import project_list, getProject

class LbShowProjectConfScript(Script):
    _version = "fake version"
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
  