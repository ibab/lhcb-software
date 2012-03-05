'''
Created on Dec 6, 2011

@author: hmd
'''


from LbRelease.CMT import LbCMTWhich, LbProject
from LbConfiguration.Version import getVersionsFromDir
from LbConfiguration.Project import getProject, ProjectConfException

import json

import os

_afs_doc_base = "/afs/cern.ch/lhcb/software/releases/DOC"

def getHTMLReleaseInfo(project, version):
    output = {
              "tags" : [],
              "description" : "",
              "date" : "",
              "archdate" : ""
              }

    prj_doc_releases = os.path.join(_afs_doc_base, project.lower(), "releases")
    prj_doc_ar_releases = os.path.join(prj_doc_releases, "archive")

    release_doc_dir = os.path.join(prj_doc_releases, version)
    release_doc_ar_dir = os.path.join(prj_doc_ar_releases, version)
    release_doc_desc = os.path.join(release_doc_dir, "description.html")
    release_doc_desc_ar = os.path.join(release_doc_ar_dir,"description.html")
    if os.path.exists(release_doc_desc) :
        output["description"] = open(release_doc_desc).read()
        vers_list = [ v.name() for v in getVersionsFromDir(prj_doc_releases)]
        vers_list.append("archive")
        for d in os.listdir(prj_doc_releases) :
            if d not in vers_list :
                if os.path.exists(os.path.join(prj_doc_releases, d, version)) :
                    output["tags"].append(d)

    elif os.path.exists(release_doc_desc_ar) :
        output["description"] = open(release_doc_desc_ar).read()
        output["tags"].append("archived")
        output["date"] = open(os.path.join(release_doc_ar_dir, "reldate.txt")).read().strip()
        output["archdate"] = open(os.path.join(release_doc_ar_dir, "arcdate.txt")).read().strip()


    return output

def getReleaseInfo(project, version=None, with_html=True):
    output = {
              "action"       : "create_project_release",
              "name"         : project,
              "version"      : version,
              "date"         : "",
              "archdate"     : "",
              "dependencies" : [],
              "packages"     : [],
              "notes"        : ""
              }

    if version:
        # speed up project lookup if the version is specified
        if not "CMTPROJECTPATH" in os.environ:
            os.environ["CMTPROJECTPATH"] = os.environ.get("LHCBPROJECTPATH", "")
        searchpath = os.environ.get("CMTPROJECTPATH").split(os.pathsep)
        pv = os.path.join(project.upper(), "%s_%s" % (project.upper(), version))
        for d in searchpath:
            if os.path.exists(os.path.join(d, pv, "cmt", "project.cmt")):
                p = LbProject(os.path.join(d, pv))
                break
    else:
        p = LbCMTWhich(project=project, version=version)

    if p :
        output["version"] = p.version()
        output["name"] = p.name()

        for b in  p.base(cmtprojectpath=os.environ["CMTPROJECTPATH"]) :
            try:
                output["dependencies"].append({"name":b.name(), "version": b.version()})
            except ProjectConfException:
                # ignore non-versioned projects
                pass
        for pak in p.packages() :
            output["packages"].append({"name":pak.name(),
                                       "version": pak.version(),
                                       "notes": pak.releaseNotes().strip()})

        output["notes"] = p.releaseNotes().strip()
        output["date"] = p.releaseDate()

    if with_html and version :
        html_info = getHTMLReleaseInfo(project, version )
        output["description"] = html_info["description"]
        output["tags"] = ",".join(html_info["tags"])
        if "archived" in html_info["tags"] :
            output["date"] = html_info["date"]
            output["archdate"] = html_info["archdate"]

    return json.dumps(output, sort_keys=True, indent=4)