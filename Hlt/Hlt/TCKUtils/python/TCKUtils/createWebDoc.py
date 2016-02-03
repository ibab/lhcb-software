import os
import sys
from utils import *

header = '''<html>
<head>
<style>
table, td, th, tr {
border-collapse:collapse;
border:1px solid black; 
padding:2px; 
}
tr.alt1 { background-color:#E0E0E0; }
tr.alt2 { background-color:#FFFFFF; }
.prop { margin-left: 30px; margin-top: 2px; margin-bottom: 2px; }
.alg { margin-left: 60px; margin-top: 2px; margin-bottom: 2px;}
.tool { margin-left: 90px; margin-top: 2px; margin-bottom: 2px;}
.toolprop { margin-left: 120px; margin-top: 2px; margin-bottom: 2px;}
a:link,a:visited {text-decoration:none}
a:hover {text-decoration:underline}
.hidden { display: none; }
.unhidden { display: block; }
</style>

<script type="text/javascript">
 function showhide(divID) {
   var item = document.getElementById(divID);
   if (item) {
     item.className=(item.className=='hidden')?'unhidden':'hidden';
   }
 }

 function showhide2(divID) {
   var item = document.getElementById(divID);
   if (item) {
     item.className=(item.className=='hidden')?'unhidden':'hidden';
   }
   item = document.getElementById('un' + divID);
   if (item) {
     item.className=(item.className=='hidden')?'unhidden':'hidden';
   }
 }
</script>
</head>
<body>

'''

def list_streams(tree):
    return ['Hlt1', 'Hlt2']

# List HLT lines (for a given stream, "Hlt1" or "Hlt2", or for all streams if stream=None)
# Hlt lines are algorithms of type "Hlt::Line"

def list_lines(tree, stream=None):
    stream_lines = []
    for i in tree:
        if hasattr(i, 'leaf') and i.leaf:
            if i.leaf.type == 'Hlt::Line':
                if (not stream or i.leaf.name.startswith(stream)) and i.leaf.name not in stream_lines:
                    s = i.leaf.name
                    stream_lines.append(i.leaf.name)
    return stream_lines

# List main properties of the Hlt line configurable


def list_line_properties(tree, linename=''):
    matchleaf = None

    # Search for the line by its name
    # The name should be unique, so exit
    # immediately after it is found
    for i in tree:
        if i.leaf and i.leaf.type == 'Hlt::Line':
            if i.leaf.name == linename:
                matchleaf = i.leaf

    if matchleaf:
        # Prescale fraction
        prescaler = matchleaf.properties()['Prescale']
        # Postscale fraction
        postscaler = matchleaf.properties()['Postscale']
        # Sequence of line filters
        filter1seq = matchleaf.properties()['Filter1']
        # L0 decision filter (if any)
        l0dufilt = matchleaf.properties()['L0DU']
        # ODIN filter (if any)
        odinfilt = matchleaf.properties()['ODIN']
        # HLT filter (if any)
        hltfilt = matchleaf.properties()['HLT']

        prescale = None
        postscale = None
        filter1members = None

        for i in tree:
            if i.leaf and i.leaf.fullyQualifiedName == prescaler:
                if 'AcceptFraction' in i.leaf.properties():
                    prescale = i.leaf.properties()['AcceptFraction']
                else:
                    prescale = ''
        for i in tree:
            if i.leaf and i.leaf.fullyQualifiedName == postscaler:
                if 'AcceptFraction' in i.leaf.properties():
                    postscale = i.leaf.properties()['AcceptFraction']
                else:
                    postscale = ''
        for i in tree:
            if i.leaf and i.leaf.fullyQualifiedName == filter1seq:
                if 'Members' in i.leaf.properties():
                    filter1members = i.leaf.properties()['Members']
                else:
                    filter1members = '[]'
        for i in tree:
            if i.leaf and i.leaf.fullyQualifiedName == l0dufilt:
                if 'Code' in i.leaf.properties():
                    l0dufilt = i.leaf.properties()['Code']
                else:
                    l0dufilt = ''
                break
        for i in tree:
            if i.leaf and i.leaf.fullyQualifiedName == odinfilt:
                if 'Code' in i.leaf.properties():
                    odinfilt = i.leaf.properties()['Code']
                else:
                    odinfilt = ''
                break
        for i in tree:
            if i.leaf and i.leaf.fullyQualifiedName == hltfilt:
                if 'Code' in i.leaf.properties():
                    hltfilt = i.leaf.properties()['Code']
                else:
                    hltfilt = ''
                break

        # Print the properties
        line_dict = {}
        line_dict['Prescale'] = prescale
        line_dict['ODIN'] = odinfilt
        line_dict['L0DU'] = l0dufilt
        line_dict['HLT'] = hltfilt
        if type(filter1members) == str:
            line_dict['Filters1'] = eval(filter1members)
        elif type(filter1members) == list:
            line_dict['Filters1'] = filter1members
        elif filter1members:
            line_dict['Filters1'] = [filter1members]
        else:
            line_dict['Filters1'] = []

        line_dict['Postscale'] = postscale

        return line_dict

# List properties of the algorithm
# If verbose=False, list only the most important ones
# If tools=True, also list the properties of all the tools
#   used by this algorithm


def list_alg_properties(tree, algname=''):

    # Each algorithm can appear several times in the tree.
    # (because of the lines that are "OR" of all the lines in the stream).
    # This is the list to store all the algorithms that are already printed
    # to avoid printing the same algorithm several times.
    printed_algs = []

    alg_dict = {'__Tools': {}}

    for i in tree:
        if not i.leaf:
            continue

        tool = False
        if i.leaf.fullyQualifiedName == algname:
            tool = False
        else:
            stralgname = algname.split('/')[1]
            if i.leaf.fullyQualifiedName.find('/' + stralgname + '.') >= 0:
                tool = True
                toolname = i.leaf.fullyQualifiedName.split('.')[1]
            else:
                continue

        if i.leaf.fullyQualifiedName in printed_algs:
            continue
        printed_algs += [i.leaf.fullyQualifiedName]

#       print '  ', i.leaf.fullyQualifiedName, tool

        if not tool:
            for (k, v) in i.leaf.properties().iteritems():
                alg_dict[k] = v
        else:
            if toolname not in alg_dict['__Tools'].keys():
                alg_dict['__Tools'][toolname] = {}
            for (k, v) in i.leaf.properties().iteritems():
                alg_dict['__Tools'][toolname][k] = v

    return alg_dict


def make_html_summary(db, tck, summary):

    dirname = 'html/%s' % tck
    filename = dirname + '/index.html'

    if not os.path.isdir(dirname):
        os.makedirs(dirname)

    htmlfile = open(filename, 'w')
    htmlfile.write(header)

    htmlfile.write(
        '<p><a href=http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/hlt/>[HLT Project homepage]</a></p>')

    htmlfile.write('<h1> TCK %s : %s </h1>\n' % (tck, summary))
    htmlfile.write('<p><b>Streams:</b></p>\n')
    htmlfile.write('<ul>\n')

    for stream in sorted(db.keys()):
        htmlfile.write('<li><a href="#%s">%s</a>\n' % (stream.lower(), stream))

    htmlfile.write('</ul><hr>\n')

    for stream in sorted(db.keys()):
        htmlfile.write('<h2><a name="%s"> %s </a></h2>\n' %
                       (stream.lower(), stream))
        htmlfile.write('<table>')
        for line in sorted(db[stream].keys()):
            prescale = db[stream][line]['Properties']['Prescale']
            try:
                if float(prescale) < 1.:
                    prescale = '<font color="red"> %5.3f </font>' % float(
                        prescale)
                else:
                    prescale = '1.0'
            except:
                prescale = '<font color="red"> %s </font>' % prescale
            htmlfile.write('<tr><td><a href="%s/%s.html"> %s </a></td><td> %s </td></tr>\n'
                           % (stream.lower(), line.lower(), line, prescale))
        htmlfile.write('</table>')

    htmlfile.write('</body>')

    htmlfile.close()


def indent(n):
    s = 30
    for i in str(n):
        if i == '_':
            s += 30

    return 'style="margin-left: ' + str(s) + 'px; margin-top: 2px; margin-bottom: 2px;"'


def ind(lvl):
    s = '<br>'
    for n in range(0, lvl):
        s += '&nbsp;&nbsp;&nbsp;'
    return s


def lokiparse(eq):
    l = ""
    lvl = 0
    last = ""
    for s in eq:
        if s == "(":
            lvl += 1
            if ((last == "|") or (last == "&")):
                l += ind(lvl - 1) + s
            else:
                l += s
        elif s == ")":
            lvl -= 1
            l += s
        else:
            l += s
        if s != ' ':
            last = s
    return l

_tab = 3


def prettyPrintStreamer(code):
    code.translate(None, '\n')
    return ('<br>\n' + (_tab) * '&nbsp;' + '>> ').join([i.strip() for i in code.split('>>')])


def prettyPrintDict(code, trItem):
    try:
        cuts = eval(code)  # should be a dict
        return "{<br>\n" + (_tab + 2) * '&nbsp;' + ('<br>\n' + (_tab) * '&nbsp;' + ', ').join([trItem(k, v) for (k, v) in cuts.iteritems()]) + '<br>\n' + "}"
    except:
        return code


def prettyPrintList(code, trItem=None, skipEmpty=True):
    try:
        l = eval(code)
        if skipEmpty and not l:
            return ''
        if len(l) < 2:
            return code
        if trItem:
            l = [trItem(i) for i in l]
        return "[<br>\n" + (_tab + 2) * '&nbsp;' + ('<br>\n' + (_tab) * '&nbsp;' + ', ').join(l) + '<br>\n' + ']'
    except:
        return code


def make_members_line(line, summary, streamname, linename, htmlfile, alg, n):

    # List of useful properties
    properties_whitelist = [
                            'RoutingBits', 'AcceptFraction', 'FilterDescriptor', 'Code', 
                            'InputLocations', 'Inputs', 'DaughtersCuts', 'CombinationCut', 
                            'MotherCut', 'DecayDescriptor', 'DecayDescriptors',
                            'OutputSelection', 'Output', 'MinPVs', 'MaxPVs', 'MaxMM', 'MinMM', 
                            'PIDs', 'TisTosSpecs', 'ModeOR', 'IgnoreFilterPassed', 
                            'Members', 'Substitutions', "InfoLocations", "Tool", "MaxLevel"
    ]

    properties_blacklist = [
        "StatTableHeader", "AuditExecute", "AuditReinitialize", "VetoObjects", "AuditRestart",
        "MonitorService", "Enable", "RootOnTES", "RequireObjects", "RootInTES", "AuditFinalize",
        "AuditEndRun", "ErrorsPrint", "AuditBeginRun", "ErrorMax", "StatEntityList", "AuditInitialize",
        "RegularRowFormat", "TypePrint", "StatPrint", "AuditStop", "Context", "PropertiesPrint",
        "GlobalTimeOffset", "UseEfficiencyRowFormat", "OutputLevel", "AuditStart", "RegisterForContextService",
        "AuditAlgorithms", "EfficiencyRowFormat", "ErrorCounter", "CounterList", "FormatFor1DHistoTable",
        "HeaderFor1DHistoTable", "ShortFormatFor1DHistoTable", "MonitorHistograms",
        "HistoCountersPrint", "HistoTopDir", "HistoPrint", "HistoSplitDir", "HistoCheckForNaN",
        "UseSequencialNumericAutoIDs", "HistoDir", "InputPlotsPath", "Monitor", "Factory", "InputPlotsTool",
        "HistoProduce", "OutputPlotsTool", "PreMonitor", "PreloadTools", "OutputPlotsPath", "PostMonitor",
        "DaughtersPlotsPath", "CombinationPlots", "CombinationPlotsPath", "DaughtersPlots", "CombinationMonitor",
        "DaughtersMonitors", "WriteP2PVRelations", "ParticleCombiners", "PVReFitters", "VertexFitters",
        "ParticleFilters", "ParticleReFitters", "DecayTreeFitters", "MassFitters", "DistanceCalculators",
        "LifetimeFitters", "DirectionFitters", "CheckOverlapTool", "MotherMonitor", "MotherPlots",
        "MotherPlotsPath", "ForceP2PVBuild", "HistoDescriptor", "HistoOffSet", "HistogramUpdatePeriod",
    ]

    htmlfile.write(
        '''<p %s><a href="javascript:showhide('div%s');"> %s </a></p>\n''' % (indent(n), n, alg))
    htmlfile.write('<div id="div%s" class="hidden">\n' % n)

    r = 0

    if 'Members' in line['Algs'][alg].keys():
        algs = eval(line['Algs'][alg]['Members'])
        n2 = 0

        htmlfile.write('''<p %s><b>Members:</b></p>\n''' %
                       (indent(str(n) + "_")))
        for alg2 in algs:
            make_members_line(line, summary, streamname,
                              linename, htmlfile, alg2, "%s_%d" % (n, n2))

            n2 += 1

    htmlfile.write('''<p %s><b>Main properties:</b></p>\n''' %
                   (indent(str(n) + "_")))
    htmlfile.write('<p %s><table>\n' % indent(str(n) + "_"))

    nn = 0

    r = 0
    for k, v in line['Algs'][alg].iteritems():
        if k in properties_blacklist:
            continue
        if k == '__Tools':
            continue

        if k == 'CombinationCut':
            v = prettyPrintStreamer(v)
        if k == 'MotherCut':
            v = prettyPrintStreamer(v)
        if k == 'Code':
            v = prettyPrintStreamer(v)
        if k == 'Members':
            v = prettyPrintList(v)
        if k == 'DaughtersCuts':
            v = prettyPrintDict(v, lambda k2, v2: "'%s' : '%s'" % (k2, v2))
        if k == 'Inputs':
            v = prettyPrintList(v)
        if k == 'InputLocations':
            v = prettyPrintList(v)
        if k == 'Preambulo':
            v = prettyPrintList(v)
        if k == 'FilterDescriptor':
            v = prettyPrintList(v, lambda y: "'%s'" % y, True)
        if k == 'RoutingBits':
            v = prettyPrintDict(v, lambda k2, v2: "%2d : \"%s\"" % (k2, v2))

#       if k == 'CombinationCut' or k == 'MotherCut' or k == 'Code' :
#         v2 = lokiparse(v)
#
#         if v != v2 :
#           v = '<div id="indent%s_%d" class="hidden">%s</div><div id="unindent%s_%d" class="unhidden">%s</div>' % (n, nn, v2, n, nn, v)
#           k = k + '''<p align="right"><a href="javascript:showhide2('indent%s_%d');">Indent on/off</a></p>''' % (n, nn)

        if v == '':
            v = 'None'

        if r == 0:
            htmlfile.write(
                ' <tr class="alt1"><td>%s : </td><td>%s</td></tr>\n' % (k, v))
        else:
            htmlfile.write(
                ' <tr class="alt2"><td>%s : </td><td>%s</td></tr>\n' % (k, v))
        r = 1 - r

    htmlfile.write('</table></p>\n')

    htmlfile.write('''<p %s><a href="javascript:showhide('div%so');"><b>Other properties:</b></a></p>\n''' %
                   (indent(str(n) + "_"), n))
    htmlfile.write('<div id="div%so" class="hidden">\n' % n)
    htmlfile.write('<p %s><table>\n' % indent(str(n) + "_"))

    r = 0
    for k in properties_blacklist:
        nn += 1
        if k == 'Members':
            continue
        if k not in line['Algs'][alg].keys():
            continue
        v = line['Algs'][alg][k]

        if v == '':
            v = 'None'

        v = str(v)

        if r == 0:
            htmlfile.write(
                ' <tr class="alt1"><td>%s</td><td>%s</td></tr>\n' % (k, v))
        else:
            htmlfile.write(
                ' <tr class="alt2"><td>%s</td><td>%s</td></tr>\n' % (k, v))
        r = 1 - r

    htmlfile.write('</table></p>\n')
    htmlfile.write('</div>\n')

    if line['Algs'][alg]['__Tools'] != {}:
        htmlfile.write(
            '''<p %s><a href="javascript:showhide('div%st');"><b>Tools:</b></a></p>\n''' % (indent(str(n) + "_"), n))
        htmlfile.write('<div id="div%st" class="hidden">\n' % n)

        m = 1
        for k in line['Algs'][alg]['__Tools'].keys():
            htmlfile.write('''<p %s><a href="javascript:showhide('div%st%d');"> %s </a></p>\n''' %
                           (indent(str(n) + "_"), n, m, k))
            htmlfile.write('<div id="div%st%d" class="hidden">\n' % (n, m))
            htmlfile.write('''<p %s><b>Properties:</b></p>\n''' %
                           (indent(str(n) + "__")))
            htmlfile.write('<p %s><table>\n' % indent(str(n) + '__'))

            r = 0
            for tk, tv in line['Algs'][alg]['__Tools'][k].iteritems():
                if tv == '':
                    tv = 'None'
                if r == 0:
                    htmlfile.write(
                        ' <tr class="alt1"><td>%s : </td><td>%s</td></tr>\n' % (tk, tv))
                else:
                    htmlfile.write(
                        ' <tr class="alt2"><td>%s : </td><td>%s</td></tr>\n' % (tk, tv))
                r = 1 - r

            htmlfile.write('</table></p>\n')
            htmlfile.write('</div>\n')
            m += 1

#       htmlfile.write('</table></p>\n')
        htmlfile.write('</div>\n')

    htmlfile.write('</div>\n')


def make_html(line, tck, streamname, linename):

    dirname = ('html/%s' % tck) + '/' + streamname.lower()
    filename = dirname + '/' + linename.lower() + '.html'

    if not os.path.isdir(dirname):
        os.makedirs(dirname)

    htmlfile = open(filename, 'w')
    htmlfile.write(header)

    htmlfile.write(
        '<p><a href=http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/hlt/>[HLT Project homepage]</a></p>')
    htmlfile.write('<p><a href=../index.html>[%s lines]</a></p>' % tck)

    htmlfile.write('<h1> %s </h1>\n' % linename)

    htmlfile.write('<h2> Properties: </h2>\n')

    htmlfile.write('<p class="prop"><table>\n')
    r = 0
    for k, v in line['Properties'].iteritems():
        if v == '':
            v = 'None'
        if k != 'Filters1':
            if r == 0:
                htmlfile.write(
                    ' <tr class="alt1"><td>%s</td><td>%s</td></tr>\n' % (k, v))
            else:
                htmlfile.write(
                    ' <tr class="alt2"><td>%s</td><td>%s</td></tr>\n' % (k, v))
            r = 1 - r
    htmlfile.write('</table></p>\n')

    htmlfile.write('<h2> Filter1 sequence: </h2>\n')
    n = 1
    for alg in line['Properties']['Filters1']:
        make_members_line(line, tck, streamname, linename, htmlfile, alg, n)

        n += 1

    htmlfile.write('</body>')

    htmlfile.close()


def make_line_algorithms(root_tree, tck, outdb, stream, line, alg):
    print alg
    alg_prop = list_alg_properties(root_tree, alg)
    outdb[stream][line]['Algs'][alg] = alg_prop

    if 'Members' in alg_prop:
        for alg2 in eval(alg_prop['Members']):
            make_line_algorithms(root_tree, tck, outdb, stream, line, alg2)


def make_documentation(root_tree, tck, summary):

    outdb = {}

    for stream in sorted(list_streams(root_tree)):
        outdb[stream] = {}
        for line in sorted(list_lines(root_tree, stream)):
            print stream, line
            line_prop = list_line_properties(root_tree, line)
            outdb[stream][line] = {}
            outdb[stream][line]['Properties'] = line_prop
            outdb[stream][line]['Algs'] = {}
            print line_prop['Filters1']
            for alg in line_prop['Filters1']:
                make_line_algorithms(root_tree, summary,
                                     outdb, stream, line, alg)

            make_html(outdb[stream][line], tck, stream, line)

    make_html_summary(outdb, tck, summary)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        listConfigurations()
    else:
        stck = sys.argv[1]
        tck = eval(stck)
        label = None
        for i, j in getConfigurations().iteritems():
            if tck in j['TCK']:
                label = j['label']
        if label:
            print label
            db = getConfigTree(tck)
            make_documentation(db, stck, label)
        else:
            print 'TCK ', stck, ' not found'
