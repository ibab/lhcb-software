// $Id: ClonePageTree.cpp,v 1.1 2008-05-14 15:00:56 ggiacomo Exp $
#include <iostream>
#include <OnlineHistDB/OnlineHistDB.h>
#include <OnlineHistDB/OnlineHistogram.h>
#include <OnlineHistDB/OnlineHistoOnPage.h>
#include <OnlineHistDB/OnlineHistPage.h>
using namespace std;
OnlineHistDB *HistDB=NULL;

void usage() {
  cout << "Usage:" <<endl;
  cout << "ClonePageTree [-d] [-t newTask] sourceTree newTree" <<endl;
  cout << "   -d for dry run (only list pages to be cloned" <<endl;
  cout << "   -t newTask forces all histograms in newTree to be from newTask, they are created in DB if needed" <<endl;
}

int addPages(std::string& folder,  std::vector<std::string>& pages) {
  int npages = HistDB->getPageNamesByFolder(folder,pages);
  std::vector<std::string> subFolders;
  HistDB->getPageFolderNames(subFolders, folder);
  for (std::vector<std::string>::iterator isf= subFolders.begin(); isf!= subFolders.end(); isf++) {
    npages += addPages(*isf, pages);
  }
  return npages;
}


OnlineHistogram* changeHistTaskName(OnlineHistogram* h, std::string nTask) {
  OnlineHistogram* hout=NULL;
  if (!HistDB) return hout;
  if (!h) return hout;
  if ( h->isAnaHist() && nTask.find("ANALYSIS") == std::string::npos )
    nTask = nTask + "_ANALYSIS";
  if (h->task() == nTask) return h;
  std::string newhname=nTask+"/" +h->algorithm()+"/" +h->hname();      
  hout= HistDB->getHistogram(newhname);
  if (!hout) { // declare it
    if ( h->isAnaHist() ) {
      cout << "Declaring Virtual histogram "<< newhname <<endl;
      std::string Algorithm;
      std::vector<std::string> sources;  
      std::vector<OnlineHistogram*> newsources; 
      std::vector<float> parameters;
      if( h->getCreationDirections(Algorithm, sources, parameters) ) {
        // change task of inputs
        for (std::vector<std::string>::iterator ish = sources.begin(); ish != sources.end(); ish++) {
          OnlineHistogram* hsou=HistDB->getHistogram(*ish);
          hsou=changeHistTaskName(hsou, nTask);
          newsources.push_back(hsou);
        }
        hout = HistDB->declareAnalysisHistogram(Algorithm, h->hname(), newsources, &parameters);
      }
      else 
        return hout;
      
    }
    else { 
        cout << "Declaring  histogram "<< newhname <<endl;
        HistDB->declareHistogram(nTask, h->algorithm(), h->hname(),  h->type());
        hout=HistDB->getHistogram(newhname);
    }
  }
  return hout;
}

bool copyOptions(OnlineHistoOnPage* hpfrom, OnlineHistogram* hto) {
  bool ok=true;
  ok &= hto->copyDisplayOptions(hpfrom); 
  if(ok)
    ok &= hto->saveDisplayOptions();
  if(ok)
    ok &= hto->setDoc(hpfrom->histo->doc());
  if(ok) 
    ok &= hto->setDescr(hpfrom->histo->descr());
  if(ok && hpfrom->histo->nbinlabels()) {
    std::vector<std::string> Xlabels;
    std::vector<std::string> Ylabels;
    for (unsigned int i=0; i< hpfrom->histo->nXbinlabels(); i++)
      Xlabels.push_back(hpfrom->histo->binlabel(i,0));
    for (unsigned int i=0; i< hpfrom->histo->nYbinlabels(); i++)
      Ylabels.push_back(hpfrom->histo->binlabel(i,1));
    ok &= hto->setBinLabels(&Xlabels, &Ylabels);
  }
  return ok;
}


int main(int argc, char* argv[]) {
  string passwd;
  bool changeTask=false;
  string newTask;
  bool dryRun=false;
  int iarg=1;
  for (int xarg=1; xarg<argc; xarg++) {
     if(string(argv[xarg]) == "-d") {
       dryRun = true;
       cout << "---- Only listing pages to be copied, no changes will be done to HistDB ----"<<endl;
       iarg++;
     }
     if(string(argv[xarg]) == "-t" && (xarg+1)<argc) {
       changeTask=true;
       newTask=argv[++xarg];
       iarg += 2;
       cout << "---- Will set all histograms on new pages to task "<<newTask <<" ----"<<endl;
     }
  }
  if( (argc-iarg)<2) {
    usage();
    return 0;
  }

  string oldName=argv[iarg++];
  string newName=argv[iarg++];

  cout << "Enter the HIST_WRITER account's password: ";
  cin >> passwd;
  cout<<endl;


  HistDB = new OnlineHistDB(passwd,"HIST_WRITER",OnlineHistDBEnv_constants::DB);
  if(changeTask) {
    OnlineHistTask* DBNewTask= HistDB->getTask(newTask);
    if (! DBNewTask) {
      cout << "Warning: The new task is unknown to the database" <<endl;
    }
  }
  std::vector<std::string> pagenames;
  
  std::string curFolder=oldName;
  int npages =addPages(curFolder, pagenames);

  bool ok=true;

  for (int ip=0; ip<npages; ip++) {
    std::string newpagename = pagenames[ip];
    newpagename.replace(newpagename.find(oldName), oldName.length(), newName.data());
    cout << "Cloning Page \""<< pagenames[ip].data() <<"\" to \""<< newpagename.data() <<"\"" <<endl;
    if(dryRun) continue; // stop here if dry run

    if(newpagename ==  pagenames[ip]) {
      cout << "No change in page name, cannot proceed"<<endl;
      continue;
    }

    OnlineHistPage*  oldpage =  HistDB->getPage(pagenames[ip].data());

    OnlineHistPage*  newpage =  HistDB->getPage(newpagename.data());
    newpage->removeAllHistograms();
    
    std::vector<OnlineHistoOnPage*> hlist;
    oldpage->getHistogramList(&hlist);
    // start putting all histograms except overlap ones (so we're sure that the mother is declared before the overlapped histos)
    for (std::vector<OnlineHistoOnPage*>::iterator ih=hlist.begin(); ih!=hlist.end(); ih++) {
      OnlineHistoOnPage* hp=*ih;
      OnlineHistogram* h=hp->histo;
      if(!hp->isOverlap()) {
        if(changeTask) h=changeHistTaskName(h,newTask);
        ok &= (NULL != newpage->declareHistogram(h, hp->xmin, hp->ymin,  hp->xmax, hp->ymax)); 
        if(changeTask) copyOptions(hp, h);
      }
    }
    // and now add the overlap ones
    for (std::vector<OnlineHistoOnPage*>::iterator ih=hlist.begin(); ih!=hlist.end(); ih++) {
      OnlineHistoOnPage* hp=*ih;
      if(hp->isOverlap()) {
        OnlineHistogram* h=hp->histo;
        OnlineHistoOnPage* moth= hp->getOverlap();
        OnlineHistogram* mh=moth->histo;
        if(changeTask) {
          h=changeHistTaskName(h, newTask);
          mh=changeHistTaskName(mh, newTask);
        }
        ok &= (NULL != newpage->addOverlapHistogram(h,mh));
        if(changeTask) copyOptions(hp, h);
      }
    }
    newpage->save();


  }


  HistDB->commit();    
  
  delete HistDB;
}
