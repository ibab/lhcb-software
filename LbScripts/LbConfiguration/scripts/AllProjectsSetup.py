from LbUtils.Script import Script
from LbUtils.Env import Environment

from LbConfiguration.Project import project_list, getProject


class AllProjectsSetupScript(Script):
    _version = "fake version"
    def defineOpts(self):
        parser = self.parser
        parser.set_defaults(targetshell="csh")
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          dest = "targetshell",
                          choices = ['csh','sh','bat'],
                          help="(internal) select the type of shell to use")

    def main(self):
        opts = self.options
        ev = Environment(orig=dict())
        for p in project_list :
            key = "%s_release_area" % p.Name()
            value = p.ReleaseArea()
            ev[key] = value
        print ev.gen_script(opts.targetshell)

if __name__ == '__main__':
    s = AllProjectsSetupScript(usage="%prog [options]")
    s.run()

  