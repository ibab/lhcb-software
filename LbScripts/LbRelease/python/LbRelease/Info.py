'''
Created on Dec 6, 2011

@author: hmd
'''


from LbRelease.CMT import LbCMTWhich

import json

def getReleaseInfo(project, version=None, with_html=True):
    output = {
              "action"       : "create_project_release",
              "name"         : project,
              "version"      : version,
              "dependencies" : [],
              "packages"     : [],
              "notes"        : ""
              }

    p = LbCMTWhich(project=project, version=version)

    output["version"] = p.version()
    output["name"] = p.name()

    for b in  p.base() :
        output["dependencies"].append({"name":b.name(), "version": b.version()})

    for pak in p.packages() :
        output["packages"].append({"name":pak.name(),
                                   "version": pak.version(),
                                   "notes": pak.releaseNotes()})

    output["notes"] = p.releaseNotes()

    return json.dumps(output, sort_keys=True, indent=4)