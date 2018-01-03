// Daniel DeLayo's Graph Comparing Tool
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TPad.h"
#include "TKey.h"
#include "TClass.h"
#include "TCanvas.h"
#include "TRint.h"
#include "TROOT.h"


int diffMax = 500;
int scaleMax = 2;
int qScaled = 1; //number of bin 1/4s (neg quality) for divided
int qDiff = 1; //number of bin 1/4s (neg quality) for subtracted

int sourceX = 3; //source pads in X direction (used for output)
int sourceY = 3; //source pads in Y direction  (used for output)

//int canvasX = 3000;
//int canvasY = 1000;
//int screenX = 1800;
//int screenY = 900;
bool silent = true;

int canvasCount = 0;
int currCanvas = 0;

void updateProgress()
{
	std::cout << "\r" << currCanvas << "/" << canvasCount << std::flush;
}

void enableShowCanvas()
{
	gROOT->SetBatch(kFALSE);
}

void disableShowCanvas()
{
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

void countCanvas(std::string fName)
{
	TFile *f1 = new TFile(fName.c_str());
	TList *branchList = f1->GetListOfKeys();
	
	for (int i = 0; i < 5; i++) {branchList->Remove(branchList->At(32));} //ahhh
	
	TIter iter(branchList);
	TKey* key;
	while ((key = (TKey*)(iter.Next())) != 0) {
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (!cl->InheritsFrom("TCanvas")) continue;
		canvasCount++;
	}
	delete f1;
}

void histanalyze(std::string fString1, std::string fString2){ 
	disableShowCanvas();
	TFile *f1 = new TFile(fString1.c_str());
	TFile *f2 = new TFile(fString2.c_str());
	
	fString1.erase(fString1.size()-5);
	fString2.erase(fString2.size()-5);
	std::string toWrite = fString1 + "_vs_" + fString2;

	TFile *fw = new TFile((toWrite + ".root").c_str(), "RECREATE");
	TFile *fws = new TFile((toWrite + "_summary.root").c_str(), "RECREATE");
			
	TList *branchList = f1->GetListOfKeys();
	for (int i = 0; i < 5; i++) {branchList->Remove(branchList->At(32));} //ahhh
	TIter iter(branchList);
	TKey *key;
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
		std::string title = tc->GetTitle();
		title.append("_compared");	
		TCanvas *tcw = new TCanvas();
		tcw->SetTitle(title.c_str());
		//tcw->SetCanvasSize(canvasX, canvasY);
		//tcw->SetWindowSize(screenX, screenY);		

		tcw->Divide(sourceX*2, sourceY, 0.001, 0.01);
		int counter = 1;
		
		TList *primList = tc->GetListOfPrimitives();	
		TIter piter(primList);
		TObject *prim;
		TH1** histos = new TH1*[sourceX * sourceY * 2];
		for (int i = 0; i < (sourceX * sourceY * 2); i++) {histos[i]=NULL;}
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
				histos[counter-2] = diffAnalyze((TH1*)prim2, (TH1*)(primMatch));
				histos[counter-2]->Draw("COLZ1");
				tcw->cd(counter++);
				histos[counter-2] = multAnalyze((TH1*)prim2, (TH1*)(primMatch));
				histos[counter-2]->Draw("COLZ1");
			}	
		}
		if (toDraw) {
			if (!silent) {enableShowCanvas(); tcw->DrawClone(); disableShowCanvas();}
			fws->WriteTObject(tcw, tc->GetName()); fws->Flush();
		}
		fw->WriteTObject(tcw, tc->GetName());
		fw->Flush();
		for (int i = 0; i < (sourceX * sourceY * 2); i++) { if (histos[i] != NULL) delete histos[i];}
		delete[] histos;
		delete tcw; delete tc; delete tc2;
	}		
	enableShowCanvas();
	delete fw; delete f1; delete f2; delete fws; 
}

void runner (int argc, char** argv)
{
	std::cout << "args: " << argc << std::endl;
	std::string f1(argv[1]);
	for (int i = 0; i < argc; i++)
	{
		std::cout << i << ":" << argv[i] << std::endl;
	}
	for (int i = 2; i < argc; i++)
	{
		std::string fi(argv[i]);
		countCanvas(f1);
		if (i != argc-1) countCanvas(fi);
	}

	for (int i = 2; i < argc; i++)
	{
		std::string fi(argv[i]);
		std::cout << "ahh" << std::endl;
		histanalyze(f1, fi);
	}
	for (int i = 2; i < argc-1; i++)
	{
		std::string fi(argv[i]);
		std::string fnext(argv[i+1]);
		histanalyze(fi, fnext);
	}
}

int main(int argc, char** argv)
{
	int pArgc = 0;
	char* pArgv[argc];
	for (int i = 0; i < argc; i++)
	{	
		if (argv[i][0] == '-') 
		{
			if (argv[i][1] == 'l') {silent = false;}
			continue;	
		}		
		pArgv[pArgc++] = argv[i];
	}
	TRint app("ROOT Application", &pArgc, pArgv);
	runner(app.Argc(), app.Argv());	
	app.Run();
	return 0;
}
