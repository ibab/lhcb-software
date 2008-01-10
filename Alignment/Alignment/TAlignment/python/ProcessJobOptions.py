import os, sys, re

def _find_file(f):
    f = os.path.expandvars(f)
    if os.path.isfile(f):
        return os.path.realpath(f)
    
    path = os.environ.get('JOBOPTSEARCHPATH','').split(os.pathsep)
    # find the full path to the option file
    candidates = [d for d in path if os.path.isfile(os.path.join(d,f))]
    if not candidates:
        raise "Cannot find '%s' in %s" % (f,path)
    return os.path.realpath(os.path.join(candidates[0],f))

_included_files = set()
def _to_be_included(f):
    if f in _included_files:
        print "# Warning: file '%s' already included, ignored."%f
        return False
    _included_files.add(f)
    return True

printing_level = 0
def _print(msg):
    if printing_level <= 0:
        print msg
    #print msg


class JobOptsParser:
    comment = re.compile(r'(//.*)$')
    directive = re.compile(r'^\s*#\s*(\w+)\s+(.*)\s*$')
    comment_ml = ( re.compile(r'/\*'), re.compile(r'\*/') )
    statement_sep = ";"
    
    def __init__(self):
        # parser level states
        self.units = {}
        self.defines = {}
        if sys.platform != 'win32':
            self.defines[ "WIN32" ] = True
        
    def _include(self,file,function):
        file = _find_file(file)
        if _to_be_included(file):
            _print("# --> Including file '%s'"%file)
            function(file)
            _print("# <-- End of file '%s'"%file)
        
    def parse(self,file):
        global printing_level
        
        # states for the "translation unit"
        statement = ""
        
        ifdef_level = 0
        ifdef_skipping = False
        ifdef_skipping_level = 0
        
        f = open(_find_file(file))
        l = f.readline()
        while l:
            l = l.rstrip()+'\n' # normalize EOL chars (to avoid problems with DOS new-line on Unix)
            
            # single line comment
            m = self.comment.search(l)
            if m:
                l = l[:m.start()]+l[m.end():]
        
            # process directives
            m = self.directive.search(l)
            if m:
                directive_name = m.group(1)
                directive_arg = m.group(2).strip()
                if directive_name == "include":
                    included_file = directive_arg.strip("'\"")
                    importOptions(included_file)
                elif directive_name == "units":
                    units_file = directive_arg.strip("'\"")
                    self._include(units_file,self._parse_units)
                elif directive_name in [ "ifdef", "ifndef"]:
                    ifdef_skipping_level = ifdef_level
                    ifdef_level += 1
                    if directive_arg in self.defines:
                        ifdef_skipping = directive_name == "ifndef"
                    else:
                        ifdef_skipping = directive_name == "ifdef"
                elif directive_name == "else":
                    ifdef_skipping = not ifdef_skipping
                elif directive_name == "endif":
                    ifdef_level -= 1
                    if ifdef_skipping and ifdef_skipping_level == ifdef_level:
                        ifdef_skipping = False
                elif directive_name == "pragma":
                    if not directive_arg:
                        l = f.readline()
                        continue
                    pragma = directive_arg.split()
                    if pragma[0] == "print":
                        if len(pragma) > 1:
                            if pragma[1].upper() in [ "ON", "TRUE", "1" ]:
                                if printing_level > 0:
                                    printing_level -= 1
                            else:
                                printing_level += 1
                else:
                    print "# // Warning: unknown directive '%s'"%directive_name
                l = f.readline()
                continue
            
            if ifdef_skipping:
                l = f.readline()
                continue
        
            # multi-line comment
            m = self.comment_ml[0].search(l)
            if m:
                l,l1 = l[:m.start()],l[m.end():]
                m = self.comment_ml[1].search(l1)
                while not m:
                    l1 = f.readline()
                    if not l1:
                        break # EOF
                    m = self.comment_ml[1].search(l1)
                if not l1 and not m:
                    raise "End Of File reached before end of multi-line comment"
                l += l1[m.end():]
            
            if self.statement_sep in l:
                i = l.index(self.statement_sep)
                statement += l[:i]
                self._eval_statement(statement.replace("\n","").strip())
                statement = l[i+1:]
            else:
                statement += l
            
            l = f.readline()
            
    def _parse_units(self,file):
        for line in open(file):
            if '//' in line:
                line = line[:line.index('//')]
            line = line.strip()
            if not line:
                continue
            nunit, value = line.split('=')
            factor, unit = nunit.split()
            value = eval(value)/eval(factor)
            self.units[unit] = value

    def _eval_statement(self,statement):
        from GaudiKernel.Proxy.Configurable import ConfigurableGeneric, Configurable
        #statement = statement.replace("\n","").strip()
        _print("# %s%s"%(statement,self.statement_sep))
        
        property,value = statement.split("=",1)
        
        inc = None
        if property[-1] in [ "+", "-" ]:
            inc = property[-1]
            property = property[:-1]
        
        property = property.strip()
        value = value.strip()
        
        ## find the configurable to apply the property to
        #parent_cfg = None
        #while '.' in property:
        #    component, property = property.split('.',1)
        #    if parent_cfg:
        #        if hasattr(parent_cfg,component):
        #            cfg = getattr(parent_cfg,component)
        #        else:
        #            cfg = ConfigurableGeneric(component)
        #            setattr(parent_cfg,component,cfg)
        #    else:
        #        cfg = ConfigurableGeneric(component)
        #    parent_cfg = cfg
        
        component, property = property.rsplit('.',1)
        if component in Configurable.allConfigurables:
            cfg = Configurable.allConfigurables[component]
        else:
            cfg = ConfigurableGeneric(component)
        
        value = os.path.expandvars(value)
        value = value.replace('true','True').replace('false','False')
        if value[0] == '{' :
            # Try to guess if the values looks like a dictionary
            if ':' in value and not ( value[:value.index(':')].count('"')%2 or value[:value.index(':')].count("'")%2 ) :
                # for dictionaries, keep the surrounding {}
                value = '{'+value[1:-1].replace('{','[').replace('}',']')+'}'
            else : # otherwise replace all {} with []
                value = value.replace('{','[').replace('}',']')
        
        # We must escape '\' because eval tends to interpret them 
        value = value.replace('\\','\\\\')
        value = eval(value,self.units)
        
        #if type(value) is str    : value = os.path.expandvars(value)
        #elif type(value) is list : value = [ type(item) is str and os.path.expandvars(item) or item for item in value ]
    
        # consider the += and -=
        if inc == "+":
            if hasattr(cfg,property):
                prop = getattr(cfg,property)
                if type(prop) == dict:
                    for k in value:
                        prop[k] = value[k]
                else:
                    prop += value
            else:
                setattr(cfg,property,value)
        elif inc == "-":
            if hasattr(cfg,property):
                prop = getattr(cfg,property)
                if type(prop) is dict:
                    for k in value:
                        if k in prop:
                            del prop[k]
                        else:
                            _print("# Warning: key '%s' not in %s.%s"%(k,cfg.name(),property))
                else:
                    for k in value:
                        if k in prop:
                            prop.remove(k)
                        else:
                            _print("# Warning: value '%s' not in %s.%s"%(k,cfg.name(),property))
        else:
            setattr(cfg,property,value)

class _TempSysPath:
    def __init__(self, new_path):
        self.old_path = sys.path
        sys.path = new_path
    def __del__(self):
        sys.path = self.old_path

_parser = JobOptsParser()

def _import_module(file):
    dirname, basname = os.path.split(file)
    basname, ext = os.path.splitext(basname) 
    guard = _TempSysPath([dirname] + sys.path)
    __import__(basname)

def _import_pickle(file):
    import pickle
    input = open(file, 'rb')
    catalog = pickle.load(input)
    print '# Unpickled ', len(catalog), ' configurables'

def _import_opts(file):
    _parser.parse(file)

_import_function_mapping = {
                             ".py"   : _import_module,
                             ".pkl"  : _import_pickle,
                             ".opts" : _import_opts,
                            }

def importOptions( optsfile ) :
    # check the file type (extension)
    dummy, ext = os.path.splitext(optsfile)
    if ext in _import_function_mapping:
        # check if the file has been already included
        optsfile = _find_file(optsfile)
        if _to_be_included(optsfile):
            _print("# --> Including file '%s'"%optsfile)
            # include the file
            _import_function_mapping[ext](optsfile)
            _print("# <-- End of file '%s'"%optsfile)
    else:
        raise "Unknown file type '%s' ('%s')" % (ext,optsfile)

