#!/usr/bin/env python

from LbUtils.Script import Script
from LbUtils.Env import Environment, Aliases
import LbConfiguration.Project as Project
from tempfile import mkstemp
import sys, os

def _check_output_options_cb(option, opt_str, value, parser):
    if opt_str == "--mktemp":
        if parser.values.output:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.mktemp = True
    elif opt_str == "--output":
        if parser.values.mktemp:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.output = value



class AllProjectsSetupScript(Script):
    _version = "fake version"
    def _write_script(self, env):
        """ select the ouput stream according to the cmd line options """
        close_output = False
        if self.options.output:
            self.output_file = open(self.options.output,"w")
            self.options.output = None # reset the option value to avoid to reuse it
            close_output = True
        elif self.options.mktemp:
            fd, outname = mkstemp()
            self.output_file = os.fdopen(fd,"w")
            print outname
            self.options.mktemp = None # reset the option value to avoid to reuse it
            close_output = True
        else :
            self.output_file = sys.stdout
            close_output = False
        # write the data
        self.output_file.write(env)
        self.output_file.write("\n") # @todo: this may be avoided
        if close_output: 
            self.output_file.close()
    def defineOpts(self):
        """ define commandline options """
        parser = self.parser
        parser.set_defaults(targetshell="csh")
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          dest = "targetshell",
                          choices = ['csh','sh','bat'],
                          help="(internal) select the type of shell to use")
        parser.set_defaults(mktemp=False)
        parser.add_option("--mktemp", action="callback",
                          callback = _check_output_options_cb,
                          help="(internal) send the output to a temporary file and print on stdout the file name (like mktemp)")
        parser.set_defaults(output=None)
        parser.add_option("--output", action="callback", metavar="FILE",
                          type = "string", callback = _check_output_options_cb,
                          help="(internal) output the command to set up the environment ot the given file instead of stdout")

    def getEnv(self):
        ev = Environment(orig=dict())
        al = Aliases()
        for p in Project.project_list :
            key = "%s_release_area" % p.Name()
            value = p.ReleaseArea()
            ev[key] = value
        for p in Project.project_list :
            alist = p.Aliases()
            for a in alist.keys():
                al[a] = alist[a]
        return ev, al

    def main(self):
        opts = self.options
        ev, al = self.getEnv()
        self._write_script(ev.gen_script(opts.targetshell)+al.gen_script(opts.targetshell))
        return 0

if __name__ == '__main__':
    sys.exit(AllProjectsSetupScript(usage="%prog [options]").run())


  