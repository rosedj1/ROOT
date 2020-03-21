/*
This is an example skimmer that skims a root file to produce another root file.

Specifically: 
    Opens up a root file which already has a TDirectoryFile ("Ana"),  
    inside of which is a TTree ("passedEvents"). The TTree then has 
    many branches, but only one is used ("lep_id") in this template.

To run this script (suppose the name is MySkim.c), in your terminal do:
    root -l MySkim.c\(\"2mu\",\"2016\",true,0,1000,\"testname\"\)
This script won't work out of the box for you because it is pointing to 
a root file while you don't have. So change "infile_name".

Notes:
- You must escape all quote marks and parentheses. 
- Do not include any whitespaces in the 'MySkim.c(...)' part. 
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <map>
#include <utility>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLorentzVector.h"

using namespace std;

void MySkim_template(TString fs, bool run_over_all_evts, int n_evt_start, int n_evt_end, TString extra_title){

//-----------------------//
//--- User Parameters ---//
//-----------------------//
TString infile_name = "/cms/data/store/user/t2/users/ferrico/Full_RunII/d0Studies/DY/2016.root";
TString outfile_name = "MySkim_simple";

//------------------------//
//--- Define variables ---//
//------------------------//
std::vector<int> *lep_id;  // Values get stored in idL1 and idL2.
// std::vector<int> *lep_id = 0;  // Values get stored in idL1 and idL2.
int Id1, Id2;

//-----------------------------------//
//--- Check that everything is OK ---//
//-----------------------------------//
if(fs!="2e" && fs!="2mu"){
  cout<<"fs has to be 2e, or 2mu"<<endl;
  return;
}
cout<<"---- fs is "<<fs<<endl;
cout<<"---- read file"<<endl;

std::cout << "Using file: " << infile_name << std::endl;
std::cout << "Will write to file: " << outfile_name << std::endl;

TFile* infile = TFile::Open(infile_name, "READ");

TTree* tree; 
if(infile){ 
  std::cout<<"File trovato.OK"<<std::endl;
  infile->cd("Ana");
  tree = (TTree*)gDirectory->Get("passedEvents");
}
else{ std::cout<<"ERROR could not find the file"<<std::endl; return -1;}
if(!tree) { cout<<"ERROR could not find the tree"<<endl; return -1;}

// Create the new root file.
// NOTE: For some goofy-ass reason, this must happen BEFORE you create
// the new TTree which will be stored in this file. 
TFile* tmpFile =  new TFile(outfile_name + extra_title + ".root", "RECREATE");

//-------------------------//
//--- Set Branch Status ---//
//-------------------------//
cout << "--- setting branch status ---" << endl;
tree->SetBranchStatus("*",0);//1);        
tree->SetBranchStatus("lep_id",1);                  


//--------------------------//
//--- Set Branch Address ---//
//--------------------------//
cout << "--- setting branch address ---" << endl;
tree->SetBranchAddress("lep_id", &lep_id);                


//-------------------------//
//--- Make new branches ---//
//-------------------------//
// NOTE: These branches must be made before the event loop happens.
TTree* newtree = new TTree("passedEvents","passedEvents");
cout << "start setting tree " << endl;
newtree->Branch("Id1",&Id1,"Id1/I");
newtree->Branch("Id2",&Id2,"Id2/I");


//----------------------//
//--- Begin the skim ---//
//----------------------//
Long64_t n_evts_in_tree = tree->GetEntries();
std::cout << "Tree has been set." << std::endl;

int i_max;
if (run_over_all_evts)  i_max = n_evts_in_tree;
else i_max = n_evt_end;
std::cout << "Running over " << i_max << " events." << std::endl;

// Loop over each event.
for(int mcfmEvt_HZZ = n_evt_start; mcfmEvt_HZZ < i_max; mcfmEvt_HZZ++) { 

    tree->GetEntry(mcfmEvt_HZZ);
    if(mcfmEvt_HZZ % 1000 == 0)
    std::cout << mcfmEvt_HZZ << " --- Dentro il tree --- " << std::endl;

    // Put analysis here.

    // Example analysis:
    if ((*lep_id).size() != 2) continue;

    Id1 = (*lep_id)[0];
    Id2 = (*lep_id)[1];

    newtree->Fill();
}               
cout << "End reading tree." << endl;                       

// Save the newtree into the new file:      
tmpFile->cd();                                        
                                                            
newtree->Write("passedEvents", TObject::kOverwrite);   
                                                                
tmpFile->Write();                                     
tmpFile->Close();   
}
