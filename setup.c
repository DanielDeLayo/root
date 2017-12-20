#include "histanalyze.c"
void setup(){
	TBrowser *b = new TBrowser();
	TFile *f1 = new TFile("combined_local_10M_plots.root");
	TFile *f2 = new TFile("combined_local2_10M_plots.root");
	TFile *fw = new TFile("compared_file");
	//TFile *f2 = new TFile("combined_empty_10M_plots.root");
	//TH2D *h1 = (TH2D*)f1->Get("Histo_RadDet_1_2_1");
	//TH2D *h1 = (TH2D*)f1->Get("HistoVertex_shld_RadDet_v10_p2_l1_0");
	//TH2D *h2 = (TH2D*)f2->Get("Histo_RadDet_1_2_1");
	//TH2D *h2 = (TH2D*)f2->Get("Histo_RadDet_1_2_2");
	//TH2D *h2 = (TH2D*)f2->Get("HistoVertex_shld_RadDet_v10_p2_l1_0");
	gROOT->ProcessLine(".L rootcode/histanalyze.C");
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
		TCanvas *tcw = new TCanvas();
		int counter = 0;
		TList *primList = tc->GetListOfPrimitives();	
		TIter piter(primList);
		TObject *prim;
		while ((prim = piter.Next()) != 0)
		{
			string delme;
			cout << prim->ClassName() << endl;
			//cout << prim->Print() << endl;
			if (!prim->InheritsFrom("TPad")) continue;
			TPad *primC = (TPad*) prim;
			TObject *prim2;
			TIter piter2(primC->GetListOfPrimitives());
			
			while ((prim2 = piter2.Next()) != 0)
			{
				cout << prim2->ClassName() << endl;
				if (!prim2->InheritsFrom("TH1")) continue;
				cout << "CALLING" << endl;
				cin >> delme;
				tcw->cd(counter++);
				histanalyze((TH1*)prim2, (TH1*)(tc2->FindObject(primC->GetTitle()))->FindObject(prim2->GetTitle()))->Draw();	
				
				cout <<"end" << endl;
			}	
		}	
	
	}		

}
