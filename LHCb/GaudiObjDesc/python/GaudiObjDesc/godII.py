import xparser, sys, os
import tools
import genPackage, genClasses, genClassDicts, genNamespaces, genAssocDicts

from optparse import OptionParser, OptionGroup
import logging

def envOption(value, variable, option):
    """
    Helper function to treat the special value 'env' for some options.

    If the value of the option is 'env', return the value of the environment
    variable, otherwise return the value.
    In case the environment variable is not set, raises a KeyError exception
    initialized with (variable, option) as arguments.

    >>> os.environ['OPTVALUE'] = 'option value'
    >>> envOption('value a', 'OPTVALUE', '-o')
    'value a'
    >>> envOption('env', 'OPTVALUE', '-o')
    'option value'
    >>> del os.environ['OPTVALUE']
    >>> envOption('env', 'OPTVALUE', '-o')
    Traceback (most recent call last):
        ...
    KeyError: ('OPTVALUE', '-o')
    """
    if value == 'env':
        if variable in os.environ:
            return os.environ[variable]
        else:
            raise KeyError(variable, option)
    return value

class godII(object):
    """
    Class implementing the script logic of the GaudiObjDesc tool.
    """
    def __init__(self, args=None):
        """
        Initialize the script.

        Note that for backward compatibility the list of arguments is the full
        content of sys.argv (which is not what optparse expects).
        """
        # FIXME: backward compatibility, to be removed
        if 'GAUDIOBJDESCROOT' not in os.environ:
            print >>sys.stderr, "Environment variable GAUDIOBJDESCROOT not defined."
            sys.exit(1)

        self._parser = None # to make pylint happy
        self._setupOptParser()
        # FIXME: backward compatibility, sys.args[0] should not be in args
        if args:
            self._parser.prog = args[0]
            args = args[1:]
        self.opts, self.args = self._parser.parse_args(args)
        self._log = logging.getLogger(self._parser.get_prog_name())

        logging.basicConfig(level=logging.INFO)

        # check options
        if self.opts.dtd == '':
            self._parser.error("invalid value for option -t (--dtd)")

        if self.opts.namespace == '':
            self._parser.error("invalid value for option -n (--namespace)")

        # fix options
        if self.opts.classdb is None:
            self.opts.classdb = os.path.join(self.opts.root, 'xml_files', 'GODsClassDB.xml')

        ####
        # Backward compatibility
        ####
        self.fullCommand = ' '.join([self._parser.get_prog_name()] + args)
        self.tools = tools.tools()
        self.version = self._parser.version
        self.xmlSources = self.args
        self.xmlDBFileExtra = self.opts.info_file
        self.argv0 = self._parser.get_prog_name()
        self.godRoot = self.opts.root
        # if self.opts.generate is not set (None), all the options are selected
        self.gClasses = self.opts.generate in ('src', None)
        self.gClassDicts = self.opts.generate in ('dct', None)
        self.gNamespaces = self.opts.generate in ('src', None)
        self.gAssocDicts = self.opts.generate in ('dct', None)
        self.allocatorType = self.opts.allocator
        self.default_namespace = self.opts.namespace
        self.dtdPath = self.opts.dtd
        try:
            self.xmlDBFile = envOption(self.opts.classdb, 'GODXMLDB', '-x')
            self.srcOutput = envOption(self.opts.src_output, 'GODDOTHOUT', '-s')
            self.dictOutput = envOption(self.opts.dict_output, 'GODDICTOUT', '-d')
        except KeyError, x:
            msg = 'Option "{1} env" used without environment variable {0} declared'.format(*x.args)
            self._parser.error(msg)

        # FIXME: backward compatibility, to remove
        # ensure that self.godRoot ends with '/'
        if not self.godRoot.endswith(os.sep):
            self.godRoot += os.sep

        # deprecated options
        if self.opts.deprecated_o:
            self._log.warning('deprecated option -o, use -s or -d instead')
            if self.opts.generate == 'src':
                self.srcOutput = self.opts.deprecated_o
            elif self.opts.generate == 'dct':
                self.dictOutput = self.opts.deprecated_o
        if self.opts.deprecated_l:
            self._log.warning('deprecated option -l, ignored')

    def _setupOptParser(self):
        p = OptionParser(usage="%prog [options] xml_source(s)",
                         version="v11r16",
                         description="Produce c++ source files and dictionary "
                         "files from xml descriptions.",
                         epilog="'xml_source(s)' can be either one or more "
                         "directories where all xml files will be parsed or "
                         "one or more xml-files which must have the extension "
                         ".xml")
        p.remove_option('--version')
        p.add_option('-v', '--version', action='version',
                     help='show version and exit')
        p.add_option("-g", "--generate", action="store", type="choice",
                     metavar='WHAT',
                     choices=('src', 'dct'),
                     help="produce only sources ('src') or only dictionaries ('dct')")
        p.add_option("-i", "--info-file", action="append",
                     help="add additional file-package-information from './AddImports.txt'")
        p.add_option('-s', '--src-output', action='store', metavar="DIR",
                     help='define possible output destination of source code. '
                     'Use the special value "env" to use the content of the environment '
                     'variable GODDOTHOUT. [default: %default]')
        p.add_option('-d', '--dict-output', action='store', metavar="DIR",
                     help='define possible output destination of source code. '
                     'Use the special value "env" to use the content of the environment '
                     'variable GODDICTOUT. [default: %default]')
        p.add_option('-x', '--classdb', action='store', metavar="CLASSDB",
                     help='define location of "GODsClassDB.xml", which contains a '
                     'dictionary of classes and their corresponding include files. '
                     'Use the special value "env" to use the content of the environment '
                     'variable GODXMLDB. [default: ${GAUDIOBJDESCROOT}/xml_files/GODsClassDB.xml]')
        p.add_option('-r', '--root', action='store',
                     metavar='ROOT_DIR',
                     help='define the root path to the GOD tools. '
                     '[default: ${GAUDIOBJDESCROOT}]')
        p.add_option('-n', '--namespace', action='store',
                     help='define the default namespace to use if not given in XML. '
                     '[default: %default]')
        p.add_option('-t', '--dtd', action='store',
                     help='path to the DTD file (gdd.dtd). '
                     '[default: use the value of GODDTDPATH if defined, or find it in the '
                     'same directory as the xml file]')
        p.add_option('--allocator', action='store', type='choice',
                     choices=('FROMXML', 'NO', 'BOOST', 'BOOST2', 'DEBUG', 'ORDERED', 'DEFAULT'),
                     help='chose the type of allocator to use. Allowed values are: '
                     'FROMXML (use what is specified in the XML, default), '
                     'NO (never overload operators "new" and "delete"),'
                     'BOOST (always use Boost singleton pool), '
                     'BOOST2 (always use Boost singleton pool with a check on delete, slower), '
                     'DEBUG (same as BOOST2 plus debug print-outs on std::cout), '
                     'ORDERED (use "ordered_" malloc and free instead of plain ones '
                     'and generate the static method to release the memory),'
                     'DEFAULT (alias for BOOST)')

        deprecated = OptionGroup(p, "Deprecated Options",
                                 "These options are deprecated and will be removed "
                                 "in a future version.")
        deprecated.add_option('-l', action='store',
                              dest='deprecated_l', metavar="IGNORED_VALUE",
                              help='ignored')
        deprecated.add_option('-o', action='store',
                              dest='deprecated_o', metavar="OUTPUT_DIR",
                              help='output directory, use -s or -d instead')

        p.add_option_group(deprecated)

        root = os.environ['GAUDIOBJDESCROOT']
        p.set_defaults(info_file=[],
                       src_output=os.curdir,
                       dict_output=os.curdir,
                       classdb=None,
                       root=root,
                       namespace='LHCb',
                       allocator='FROMXML',
                       dtd=os.environ.get("GODDTDPATH")
                       )
        self._parser = p

    def findLongestName(self,godPackage):
        lname = 0
        if self.gAssocDicts and godPackage.has_key('assoc') :
            lname = len(godPackage['attrs']['name'])+22
        if godPackage.has_key('class') :
            classLName = self.tools.getLongestName(godPackage['class'])
            if self.gClasses     : lname = max(lname, classLName+2)
            if self.gClassDicts : lname = max(lname, classLName+9)
        if self.gNamespaces and godPackage.has_key('namespace') :
            lname = max(lname, self.tools.getLongestName(godPackage['namespace'])+2)
        return lname

    def doit(self):

        #print 'GODII %s' % self.version
        #print self.fullCommand

        x = xparser.xparser(dtdPath=self.dtdPath)

        srcFiles = []
        for src in self.xmlSources:
            if os.path.isdir(src):
                for file in src:
                    if file.split('.')[-1] == 'xml':
                        srcFiles.append(file)
            elif os.path.isfile(src) and src.split('.')[-1] == 'xml':
                srcFiles.append(src)
            else :
                print '%s: ERROR: %s passed as source location is neither directory nor a .xml file' % (self.argv0, src)
                sys.exit(1)

        if self.gClasses : gClasses = genClasses.genClasses(self.godRoot)
        if self.gNamespaces : gNamespaces = genNamespaces.genNamespaces(self.godRoot)
        if self.gAssocDicts : gAssocDicts = genAssocDicts.genAssocDicts(self.godRoot, self.dictOutput, self.srcOutput)

        for srcFile in srcFiles:

            #--->PM<--- Needs to be reset each time! Otherwise it doubles the contents for 'instantiations' and 'exclusions'
            if self.gClassDicts :
                gClassDicts = genClassDicts.genClassDicts(self.godRoot, self.dictOutput, self.srcOutput)
                if not self.gClasses : gClasses = genClasses.genClasses(self.godRoot)

            gdd = x.parseSource(srcFile)
            godPackage = gdd['package'][0]

            # Set default namespace if not set in <package>
            try:
                ns = godPackage['attrs']['namespace']
            except KeyError:
                ns = self.default_namespace
                godPackage['attrs']['namespace'] = ns


            package = genPackage.genPackage(godPackage)

            print 'Processing package ' + package.dict['packagename']

            lname = self.findLongestName(godPackage)

            if godPackage.has_key('assoc') and self.gAssocDicts :
                print '  Generating Dictionaries for Associations'
                gAssocDicts.doit(godPackage)
                print '  - Done'

            if godPackage.has_key('namespace') and self.gNamespaces :
                print '  Generating Namespaces'
                gNamespaces.doit(package,godPackage['namespace'],self.srcOutput,lname)
                print '  - Done'

            if godPackage.has_key('class'):
                if self.gClasses :
                    print '  Generating Header Files'
                    gClasses.doit(package,godPackage['class'],self.srcOutput,lname,self.allocatorType)
                    print '  - Done'

                if self.gClassDicts :
                    print '  Generating Dictionaries'
                    gClassDicts.doit(godPackage)
                    print '  - Done'

            print '- Done '

if __name__ == '__main__':
    g = godII(sys.argv)
    g.doit()
