"""
Module to parse files used by or created by CMT
"""

def parseReq(requirements, token='use', default_version="*"):
    '''Takes a requirements file or project.cmt-like string,
       and returns a dictionary of {project:version}
       of the use statements in the file

       options:
       token: steer which cmt token to look for, usually "use"
       default_version:  steer what to have as the default version
       '''
    requirements=requirements.strip().split('\n')
    projver={}
    for req in requirements:
        aproj=''
        aver=default_version
        req=req.strip()
        #ignore lines
        if 'use' not in req:
            continue
        #remove comments
        if '#' in req:
            if req.find('#')<req.find('use'):
                continue
            req=req.split('#')[0]
        #remove repeated whitespace
        while '\t' in req or '  ' in req:
            req=req.replace('\t',' ')
            req=req.replace('  ',' ')
        #tokenize
        req=req.strip()
        tokens=req.split(' ')
        if tokens[0]!=token:
            continue
        if len(tokens)<2:
            raise TypeError, ('The requirements file has "use" with no package')
        aproj=tokens[1]
        if len(tokens)>2:
            #if ('*') not in tokens[2]:
            aver=tokens[2]
        if len(tokens)>3:
            aproj=tokens[3]+'/'+aproj
        projver[aproj]=aver
    return projver
