// Daniel DeLayo's Graph Comparing Tool
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TPad.h"
#include "TKey.h"
#include "TClass.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TROOT.h"


int diffMax = 500;
int scaleMax = 2;
int qScaled = 1; //number of bin 1/4s (neg quality) for divided
int qDiff = 1; //number of bin 1/4s (neg quality) for subtracted

int sourceX = 3; //source pads in X direction (used for output)
int sourceY = 3; //source pads in Y direction  (used for output)

int canvasX = 3000;
int canvasY = 1000;
int screenX = 1800;
int screenY = 900;
bool silent = true;

int canvasCount = 0;
int currCanvas = 0;

void updateProgress()
{
	std::cout << "\r" << currCanvas << "/" << canvasCount << std::flush;
}

void enableShowCanvas()
{
	if (silent)
		gROOT->SetBatch(kFALSE);
}

void disableShowCanvas()
{
	if (silent)
		gROOT->SetBatch(kTRUE);
}
void Scale2(TH1* toScale, double fac)
{
	//int bins = toScale->GetSize();
	int bins = toScale-> GetNbinsX() * toScale->GetNbinsY();
	for (int i = 1; i < bins-1; i++)
	{	
		toScale->SetBinContent(i, toScale->GetBinContent(i)*fac);
	}
	return;
}

TH1* getAdd(TH1* orig, TH1* other)
{
	TH1* negChange = (TH1*)other->Clone();
	//TCanvas *c = new TCanvas();	
	//negChange->Draw();
	//TCanvas *cc = new TCanvas();
	//orig->Draw();
	
	negChange->Add(orig, -1); 
	negChange->SetMaximum(diffMax);
	negChange->SetMinimum(-diffMax);
	return negChange;
	//return other->Clone()->Add(orig, -1)->SetMaximum(diffMax)->SetMinimum(-diffMax);	
}

TH1* getScaled(TH1* orig, TH1* other)
{
	//return other->Clone()->Divide(orig)->SetMaximum(scaleMax);
	TH1* energyScale = (TH1*)other->Clone();
	energyScale->Divide(orig);           	
	energyScale->SetMaximum(scaleMax);
	return energyScale;
}

TH1* multAnalyze(TH1* orig, TH1* other)
{	
	TH1* scaled = getScaled(orig, other);
	if (scaled->InheritsFrom("TH2")) {((TH2*)scaled)->Rebin2D(2*qScaled,2*qScaled);} else {scaled->Rebin(2*qScaled);}
	Scale2(scaled, .25 * (1/qScaled));		
	return scaled;
}
TH1* diffAnalyze(TH1* orig, TH1* other)
{	
	TH1* change = getAdd(orig, other);
		if (change->InheritsFrom("TH2")) {((TH2*)change)->Rebin2D(2*qDiff,2*qDiff);} else {change->Rebin(2*qDiff);}
	change->SetMaximum(diffMax*4*qDiff);
	change->SetMinimum(-diffMax*4*qDiff);
	return change;
}

void histanalyze(/*string fString1, string fString2*/){ 
	disableShowCanvas();
	//TFile *f1 = new TFile(fString1);
	//TFile *f2 = new TFile(fString2);
	TFile *f1 = new TFile("combined_local_10M_plots.root");
	TFile *f2 = new TFile("combined_local2_10M_plots.root");
	TFile *fw = new TFile("compared_file.root", "RECREATE");
	TFile *fws = new TFile("compared_file_summary.root", "RECREATE");
			
	TList *branchList = f1->GetListOfKeys();
	for (int i = 0; i < 5; i++) {branchList->Remove(branchList->At(32));} //ahhh
	TIter iter(branchList);
	TKey *key;
	
	while ((key = (TKey*)(iter.Next())) != 0) {
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TCanvas")) continue;
		canvasCount++;
	}
	iter.Reset();
	while ((key = (TKey*)(iter.Next())) != 0)
	{
		//string keyStr(key->GetTitle());	
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TCanvas")) continue;
		currCanvas++;
		updateProgress();
		TCanvas *tc = (TCanvas*)f1->Get(key->GetTitle());		
		TCanvas *tc2 = (TCanvas*)f2->Get(key->GetTitle());	
		bool toDraw = false;
			
		TCanvas *tcw = new TCanvas();
		tcw->SetCanvasSize(canvasX, canvasY);
		tcw->SetWindowSize(screenX, screenY);
		
		tcw->Divide(sourceX*2, sourceY, 0.001, 0.01);
		int counter = 1;
		
		TList *primList = tc->GetListOfPrimitives();	
		TIter piter(primList);
		TObject *prim;
		while ((prim = piter.Next()) != 0)
		{
			if (!prim->InheritsFrom("TPad")) continue;	
			TPad *primC = (TPad*) prim;
			TObject *prim2;
			TIter piter2(primC->GetListOfPrimitives());
			while ((prim2 = piter2.Next()) != 0)
			{
				if (!prim2->InheritsFrom("TH1")) {continue;}
				TObject *primMatch = (tc2->FindObject(prim2->GetName()));
				if (!primMatch->InheritsFrom("TH1")) {continue;}
				std::string searchMe(((TH1*)prim2)->GetTitle());
				if (searchMe.find("All Area") != std::string::npos) toDraw = true;
				tcw->cd(counter++);
				diffAnalyze((TH1*)prim2, (TH1*)(primMatch))->Draw("COLZ1");
				tcw->cd(counter++);
				multAnalyze((TH1*)prim2, (TH1*)(primMatch))->Draw("COLZ1");	
				//cout << counter << ": " <<prim2->GetName() << endl;
				//cout << primMatch->GetName() << endl;
				//cout << (TH1*) prim2 << endl;
				//cout << (TH1*) primMatch << endl;	
			}	
		}
		if (toDraw) {enableShowCanvas(); tcw->SetBatch(kFALSE); tcw->DrawClone(); disableShowCanvas();
				fws->WriteTObject(tcw, tc->GetName()); fws->Flush();							}
		fw->WriteTObject(tcw, tc->GetName());
		fw->Flush();
		if(!toDraw) {delete tcw;}
	}		
	enableShowCanvas();
	delete fw; delete f1; delete f2; 
}

int main(int argc, char** argv)
{
	TApplication app("ROOT Application", &argc, argv);
	histanalyze(/*app.Argc(), app.Argv()*/);
	app.Run();
	return 0;
}



