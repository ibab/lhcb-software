import commands

try:
    from Gaudi.Configuration import DataObjectDescriptorCollection
except ImportError:
    # DataObjectDescriptorCollection comes with Gaudi v27r0
    pass

#extra locations known to be added
known_extras=['Raw/Prs/Adcs', 'Hlt1/DecReports', 'Hlt2/DecReports',
              'Hlt/SelReports', 'Raw/Velo/PUClustersNZS', 'Hlt1/VertexReports',
              'Raw/Hcal/Adcs', 'Raw/Ecal/Adcs', 'Hlt/VertexReports',
              'Raw/Spd/Adcs', 'Hlt2/SelReports', 'Hlt1/Track/Velo',
              'Hlt/DecReports', 'Trig/L0/L0DUData', 'Hlt1/SelReports',
              'Raw/Velo/PUClusters', 'Hlt2/VertexReports', 'Hlt/LumiSummary']

#known differences, none :)
known_diffs={}

#options which are known to differ, very very few right now :)
default_opts_diffs={'Raw/Ecal/Digits' : { 'CaloZSupAlg/EcalZSup': {'Context': ['Offline', '-->', ''],
                                                                   'OutputDigitData' : ['', '-->', 'Raw/Ecal/Digits'],
                                                                   'OutputADCData' : ['', '-->', 'Raw/Ecal/Adcs']}},
                    'Raw/Prs/Digits' : { 'CaloDigitsFromRaw/PrsFromRaw': {'Context': ['Offline', '-->', ''],
                                                                          'DigitsContainer' : ['Raw/Ecal/Digits', '-->', 'Raw/Prs/Digits'],
                                                                          'AdcsContainer' : ['Raw/Ecal/Adcs', '-->', 'Raw/Prs/Adcs']}},
                    'Raw/Hcal/Digits' : { 'CaloZSupAlg/HcalZSup': {'Context': ['Offline', '-->', ''],
                                                                   'OutputDigitData' : ['', '-->', 'Raw/Hcal/Digits'],
                                                                   'OutputADCData' : ['', '-->', 'Raw/Hcal/Adcs']}},
                    'Raw/Spd/Digits' : { 'CaloDigitsFromRaw/SpdFromRaw': {'Context': ['Offline', '-->', ''],
                                                                          'DigitsContainer' : ['Raw/Ecal/Digits', '-->', 'Raw/Spd/Digits'],
                                                                          'AdcsContainer' : ['Raw/Ecal/Adcs', '-->', 'Raw/Spd/Adcs']}},
                    'Trig/L0/MuonBCSU' : { 'L0MuonCandidatesFromRaw/L0MuonFromRaw' : { 'DAQMode': [0, '-->', 1]}},
                    'Trig/L0/MuonCtrl' : { 'L0MuonCandidatesFromRaw/L0MuonFromRaw' : { 'DAQMode': [0, '-->', 1]}},
                    'Trig/L0/MuonData' : { 'L0MuonCandidatesFromRaw/L0MuonFromRaw' : { 'DAQMode': [0, '-->', 1]}}
                    }


def parse(stdout):
    stdout='\n'.join([ s for s in stdout.split('\n') if not s.startswith("# INFO:") and not s.startswith("# WARNING:")])
    blocks=stdout.split("====================\n")
    if len(blocks)<2:
        return None
    result={}
    for block in blocks:
        #print block
        block=block.replace("====================\n",'').replace("--------------------\n",'')
        if not len(block):
            continue
        lines=[l.lstrip().rstrip() for l in block.split("\n") if len(l)>1]
        if len(lines)<3:
            print block
            continue
        dest=lines[0]
        algname=lines[1]
        #print lines[2]
        exec("algprops="+lines[2].replace("PublicToolHandle","'PublicToolHandle'+"))
        #algprops=dict(lines[2])
        result[dest]={algname: algprops}
    return result

def compare(d1,d2):
    """
    d1 is the reference, d2 is the one to check
    """
    diff={}
    for dest,v in d1.iteritems():
        if dest not in d2:
            print d1.keys(),d2.keys()
            raise KeyError(dest+" is not configured")
        for alg,props in v.iteritems():
            if alg not in d2[dest]:
                print v.keys(),d2[dest].keys()
                raise KeyError(dest+" uses different algorithm, expected "+alg)
            for prop,val in props.iteritems():
                if prop not in d2[dest][alg]:
                    print props.keys(),d2[dest][alg].keys()
                    raise KeyError(alg+" somehow has two different property lists? ")
                if val!=d2[dest][alg][prop]:
                    if dest not in diff:
                        diff[dest]={}
                    if alg not in diff[dest]:
                        diff[dest][alg]={}
                    diff[dest][alg][prop]=[val,"-->",d2[dest][alg][prop]]
                    #print dest,alg,prop, "differs", diff[dest][alg][prop]
    extras={}
    for dest,v in d2.iteritems():
        if dest not in d1:
            extras[dest]=v
    return diff,extras

ref=commands.getoutput("python ../options/dump-decoderawevent.py")
comp=commands.getoutput("python ../options/dump-confrawevent.py")
#print ref
#print comp

ref=parse(ref)
comp=parse(comp)

diff,extras=compare(ref,comp)

found=[]
known=[]
default=[]
bad=[]
unknown=[]

for dest,algs in diff.iteritems():
    if dest in known_diffs:
        for alg,props in algs.iteritems():
            if alg in known_diffs[dest]:
                #print props
                for prop,val in props.iteritems():
                    if prop in known_diffs[dest][alg]:
                        found.append([dest,alg,prop])
                        if known_diffs[dest][alg][prop]==val:
                            known.append([dest,alg,prop])
                        else:
                            bad.append([dest,alg,prop])
    if dest in default_opts_diffs:
        for alg,props in algs.iteritems():
            if alg in default_opts_diffs[dest]:
                for prop,val in props.iteritems():
                    if prop in default_opts_diffs[dest][alg].keys():
                        found.append([dest,alg,prop])
                        if default_opts_diffs[dest][alg][prop]==val:
                            default.append([dest,alg,prop])
                        else:
                            bad.append([dest,alg,prop])

    for alg,props in algs.iteritems():
        for prop,val in props.iteritems():
            # note: DataInputs and DataOutputs are new in Gaudi v27r0
            if [dest,alg,prop] in found or prop in ('DataInputs', 'DataOutputs'):
                continue
            unknown.append([dest,alg,prop])


print len(known), "expected differences found"
print len(default), "expected overwrites of default empty options found"
print len(bad), "expected modifications but with unexpected results"
print len(unknown), "unexpected modifications"
unknown_extras=[k for k in extras if k not in known_extras]
print "Extra Locations:", extras.keys()
print len(unknown_extras), "unknown new locations"

for key in bad+unknown:
    print "ERROR:", key, diff[key[0]][key[1]][key[2]]

for extra in unknown_extras:
    print "ERROR:",extra,diff[extra]

if len(bad+unknown):
    raise ValueError("Unknown or bad differences found.")

if len(unknown_extras):
    raise ValueError("Unknown extra output.")

print "Pass"
