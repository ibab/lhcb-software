import tools, os

#================================================================================
class importUtils:
#--------------------------------------------------------------------------------
    def __init__(self):
        self.tools = tools.tools()
        self.include = []
        self.stdIncludes = []
        self.forwardDeclLHCb = []
        self.forwardDeclGlob = {}
        self.forwardIncl = []
        self.verbatimLHCb = []
#--------------------------------------------------------------------------------
    def reset(self,godClass):
        self.typedefenums = []
        self.verbatimLHCb = []
#--------------------------------------------------------------------------------
    def addInclude(self, name, std=0):
        if std :
            if name not in self.stdIncludes : self.stdIncludes.append(name)
        elif name not in self.include     : self.include.append(name)
#--------------------------------------------------------------------------------
    def parseImport(self, godClass, include, stdIncludes, forwardDeclLHCb, forwardDeclGlob, forwardIncl):
        if godClass.has_key('import'):
            for imp in godClass['import']:
                impAtt = imp['attrs']
                impName = impAtt['name']
                impSoft = impAtt['soft']
                if impAtt['std'] == 'TRUE' and impName not in stdIncludes : stdIncludes.append(impName)
                else :
                    if impSoft == 'TRUE' or impSoft == 'FORWARDONLY':                        # do forward declaration stuff
                        impNS = impAtt.get('namespace')
                        if godClass['attrs'].has_key('scope'):
                            myNS = godClass['attrs']['scope']
                        elif godClass['attrs'].has_key('namespace'):
                            myNS = godClass['attrs']['namespace']
                        elif os.environ.has_key('GODSCOPE'):
                            myNS = os.environ['GODSCOPE']
                        else:
                            myNS = 'LHCb'

                        if impNS and impNS != myNS:
                            if not forwardDeclGlob.has_key(impNS) : forwardDeclGlob[impNS] = [impName]
                            elif impName not in forwardDeclGlob[impNS] : forwardDeclGlob[impNS].append(impName)
                        elif impName not in forwardDeclLHCb : forwardDeclLHCb.append(impName)
                        if impSoft == 'TRUE' and impName not in forwardIncl : forwardIncl.append(impName)
                    else :                                                                   # do include stuff
                        if impName not in include : include.append(impName)
#--------------------------------------------------------------------------------
    def genIncludes(self):
        s = ''
        for imp in self.include :
            if imp.find('GaudiKernel/boost_allocator.h') != -1 :
                # workaround for Windows max() macro problem, to be moved to GaudiKernel/boost_allocator.h in Gaudi v21r2
                s += '#ifdef _WIN32\n'
                s += '// Avoid conflict of Windows macro with std::max\n'
                s += '  #ifndef NOMINMAX\n'
                s += '    #define NOMINMAX\n'
                s += '  #endif\n'
                s += '#endif\n'
            if imp.find('HepMC/GenEvent') != -1 :
                # Suppress compiler warning from HepMC/Flow.h included by HepMC/GenEvent.h
                s += '#ifdef _WIN32\n'
                s += '// Disable warning C4800 forcing value to bool true or false (performance warning), caused by HepMC/Flow.h\n'
                s += '  #pragma warning ( disable : 4800 )\n'
                s += '#endif\n'
                # Suppress INTEL compiler remark from HepMC/SimpleVector.icc, HeavyIon.h, PdfInfo.h
                s += '#ifdef __INTEL_COMPILER\n'
                s += '  #pragma warning (disable:1572) // floating-point equality and inequality comparisons are unreliable\n'
                s += '  #pragma warning(push)\n'
                s += '#endif\n'
            if imp.find('.') != -1 : s += '#include "%s"\n' % imp
            else                   : s += '#include "%s.h"\n' % imp
            if imp.find('HepMC/GenEvent') != -1 :
                # End suppress INTEL compiler remark
                s += '#ifdef __INTEL_COMPILER\n'
                s += '  #pragma warning(pop)\n'
                s += '#endif\n'
        s += self.genStdIncludes()
        return s
#--------------------------------------------------------------------------------
    def genStdIncludes(self):
        s = ''
        for imp in self.stdIncludes : s += '#include <%s>\n' % imp
        return s
#--------------------------------------------------------------------------------
    def genForwardDeclsLHCb(self):
        s = ''
        for decl in self.forwardDeclLHCb :
            if decl not in self.include : s += 'class %s;\n' % decl.split('/')[-1]
        for imp in self.verbatimLHCb : s += '%s\n' % imp
        return s
#--------------------------------------------------------------------------------
    def genForwardDeclsGlob(self):
        s = ''
        for k in self.forwardDeclGlob.keys():
            ind = 0;
            for sk in k.split('::') :
                if sk :
                    s += '%snamespace %s {\n' % (' '*ind, sk )
                    ind += 2
            for ns in self.forwardDeclGlob[k] : s += '%sclass %s;\n' % (' '*ind, ns.split('/')[-1].split('.')[0])
            ind -= 2
            while (ind >= 0) :
                s += '%s}\n' % ( ' '*ind )
                ind -= 2
        return s
#--------------------------------------------------------------------------------
    def genForwardIncludes(self, clName):
        s = ''
        for imp in self.forwardIncl :
            if imp not in self.include and imp.split('/')[-1] != clName:
                if imp.find('.') != -1 : s += '#include "%s"\n' % imp
                else                   : s += '#include "%s.h"\n' % imp
        return s
