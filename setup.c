void setup(){
	TBrowser *b = new TBrowser();
	TFile *f1 = new TFile("combined_local_10M_plots.root");
	TFile *f2 = new TFile("combined_local2_10M_plots.root");
	//TFile *f2 = new TFile("combined_empty_10M_plots.root");
	TH2D *h1 = (TH2D*)f1->Get("Histo_RadDet_1_2_1");
	//TH2D *h1 = (TH2D*)f1->Get("HistoVertex_shld_RadDet_v10_p2_l1_0");
	TH2D *h2 = (TH2D*)f2->Get("Histo_RadDet_1_2_1");
	//TH2D *h2 = (TH2D*)f2->Get("Histo_RadDet_1_2_2");
	//TH2D *h2 = (TH2D*)f2->Get("HistoVertex_shld_RadDet_v10_p2_l1_0");
	gROOT->ProcessLine(".L macros/histanalyze.C");
	//histanalyze(h1, h2);	
	TList *branchList = f1->GetListOfKeys();
	TIter iter(branchList);
	TKey *key;
	while ((key = (TKey*)(iter.Next())) != 0)
	{
		//string keyStr(key->GetTitle());	
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TCanvas")) continue;
	
		cout <<"keys:" << endl;	
		cout << key << endl;	
		cout << key->ReadObj() << endl;	
		TCanvas *tc = (TCanvas*)key->ReadObj();
				
		TCanvas *tc2 = (TCanvas*)f2->Get(key->GetTitle());	
	
		TList *primList = tc->GetListOfPrimitives();	
		TIter piter(primList);
		while ((prim = piter.Next()) != 0)
		{
			string delme;
			cout << prim->ClassName() << endl;
			cout << prim->Print() << endl;
			if (!prim->InheritsFrom("TH1")) continue;
			cout << "CALLING" << endl;
			cin >> delme;
			histanalyze((TH1*)hist, (TH1*)(tc2->FindObject(prim->GetTitle()))->FindObject(hist->GetTitle()));	
			cout <<"end" << endl;
		
		}	
	
	}		
	//histanalyze((TH2D*)key->ReadObj(),(TH2D*)f2->Get(key->GetTitle()));

}
