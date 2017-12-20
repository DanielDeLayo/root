// Daniel DeLayo's Graph Comparing Tool
#include <cassert>
#include <cmath>

#include "TH1.h"
#include "TCanvas.h"

int diffMax = 500;
int scaleMax = 2;
int qScaled = 0; //number of bin 1/4s (neg quality) for divided
int qDiff = 0; //number of bin 1/4s (neg quality) for subtracted
void Scale2(TH1* toScale, double fac)
{
	//int bins = toScale->GetSize();
	int bins = toScale-> GetNbinsX() * toScale->GetNbinsY();
	cout << bins << endl;
	for (int i = 1; i < bins-1; i++)
	{	
		toScale->SetBinContent(i, toScale->GetBinContent(i)*fac);
	}
	cout << "Scaled!" << endl;
	return;
}

TH1* getAdd(TH1* orig, TH1* other)
{
	string delme;
	cout << "getAdd" << endl;
	TH1* negChange = (TH1*)other->Clone();
	cout << "check" << endl;
	cout << negChange << endl << orig << endl;
	TCanvas *c = new TCanvas();	
	negChange->Draw();
	TCanvas *cc = new TCanvas();
	orig->Draw();
	cout << negChange->ClassName() << endl;
	
	cin >> delme;	
	//negChange->Add(orig, -1); 
	cout << "check" << endl;	
	cin >> delme;
	negChange->SetMaximum(diffMax);
	cout << "check" << endl;
	cin >> delme;
	negChange->SetMinimum(-diffMax);	
	cout << "added" << endl;
	cin >> delme;
	return negChange;
	//return other->Clone()->Add(orig, -1)->SetMaximum(diffMax)->SetMinimum(-diffMax);	
}

TH1* getScaled(TH1* orig, TH1* other)
{
	cout << "getScaled" << endl;
	//return other->Clone()->Divide(orig)->SetMaximum(scaleMax);
	TH1* energyScale = (TH1*)other->Clone();
	energyScale->Divide(orig);           	
	energyScale->SetMaximum(scaleMax);
	return energyScale;
}

TH1* multAnalyze(TH1* orig, TH1* other)
{	
	TH1* scaled = getScaled(orig, other);
	for (int i = 0; i < qScaled; i++)
	{	
		if (scaled->InheritsFrom("TH2")) {((TH2*)scaled)->Rebin2D(2,2);} else {scaled->Rebin(2);}
		
		Scale2(scaled, .25);	
	}	
	return scaled;
}
TH1* diffAnalyze(TH1* orig, TH1* other)
{	
	for (int i = 0; i < qDiff; i++)
	{
		if (change->InheritsFrom("TH2")) {((TH2*)change)->Rebin2D(2,2);} else {change->Rebin(2);}
		change->SetMaximum(diffMax*4*(i+1));
		change->SetMinimum(-diffMax*4*(i+1));
	}	
	return change;
}

void histanalyze(){
	TBrowser *b = new TBrowser();
	TFile *f1 = new TFile("combined_local_10M_plots.root");
	TFile *f2 = new TFile("combined_local2_10M_plots.root");
	TFile *fw = new TFile("compared_file");
	
	
	List *branchList = f1->GetListOfKeys();
	TIter iter(branchList);
	TKey *key;
	while ((key = (TKey*)(iter.Next())) != 0)
	{
		//string keyStr(key->GetTitle());	
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TCanvas")) continue;
	
		//cout <<"keys:" << endl;	
		//cout << key << endl;	
		//cout << key->ReadObj() << endl;	
		TCanvas *tc = (TCanvas*)key->ReadObj();		
		TCanvas *tc2 = (TCanvas*)f2->Get(key->GetTitle());	
		TCanvas *tcw = new TCanvas();
		
		tcw->Divide(6, 3, 0, 0);
		int counter = 0;
		
		TList *primList = tc->GetListOfPrimitives();	
		TIter piter(primList);
		TObject *prim;
		while ((prim = piter.Next()) != 0)
		{
			cout << prim->ClassName() << endl;
			if (!prim->InheritsFrom("TPad")) continue;
			
			TPad *primC = (TPad*) prim;
			TObject *prim2;
			TIter piter2(primC->GetListOfPrimitives());
			
			while ((prim2 = piter2.Next()) != 0)
			{
				cout << prim2->ClassName() << endl;
				if (!prim2->InheritsFrom("TH1")) continue;
				
				cout << "CALLING" << endl;
				tcw->cd(counter++);
				diffAnalyze((TH1*)prim2, (TH1*)(tc2->FindObject(primC->GetTitle()))->FindObject(prim2->GetTitle()))->Draw();	
				tcw->cd(counter++);
				multAnalyze((TH1*)prim2, (TH1*)(tc2->FindObject(primC->GetTitle()))->FindObject(prim2->GetTitle()))->Draw();	
				cout <<"end" << endl;
			}	
		}	
	
	}		

}

 
