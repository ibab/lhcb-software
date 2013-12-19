{
TFile *f = TFile::Open("indexer_data.root");
TTree *t = 0;
f->GetObject("index", t);
t->Scan();
}
