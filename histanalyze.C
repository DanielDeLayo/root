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

TCanvas* histanalyze(TH1* orig, TH1* other)
{	
	cout << orig << endl;
	cout << other << endl;
	
	
	
	cout << "called" << endl;	
	//TCanvas* c1 = new TCanvas();
	//orig->Draw();
	//TCanvas* c2 = new TCanvas();
	//other->Draw();
		
	TCanvas* c3 = new TCanvas();
	c3->Divide(1, 2);
	TH1* change = getAdd(orig, other);
	c3->cd(1);
	for (int i = 0; i < qDiff; i++)
	{
		string checkName(change->ClassName());
		cout << checkName << endl;

		if (checkName.find("TH2")==0) {((TH2*)change)->Rebin2D(2,2);} else {change->Rebin(2);}
		change->SetMaximum(diffMax*4*(i+1));
		change->SetMinimum(-diffMax*4*(i+1));
	}
	change->DrawCopy("colz");
	TH1* scaled = getScaled(orig, other);
	c3->cd(2);
	for (int i = 0; i < qScaled; i++)
	{	
		string checkName(scaled->ClassName());
		cout << checkName << endl;
		if (checkName.find("TH2")==0) {((TH2*)scaled)->Rebin2D(2,2);} else {scaled->Rebin(2);}
		
		Scale2(scaled, .25.);	
	}	
	scaled->DrawCopy("colz");	
	return c3;
}
