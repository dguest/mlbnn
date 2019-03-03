// Minimal xAOD dumper
//
// This is a _very_ basic executable to demonstrate how to access an
// xAOD. Note that a more robust program should be checking the status
// codes with `.isSuccess()`, where here we're just using `.ignore()`
// to suppress warnings.


// EDM things
#include "xAODJet/JetContainer.h"

// AnalysisBase tool include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

// 3rd party includes
#include "TFile.h"

// stl includes
#include <stdexcept>
#include <string>
#include <iostream>
#include <memory>

void usage(const char* name) {
  std::cout << "usage: " << name << " <AOD>" << std::endl;
}

//////////////////
// main routine //
//////////////////
int main (int argc, char *argv[])
{
  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }
  std::string file_name = argv[1];

  xAOD::Init().ignore(); // <-- some global magic
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);
  std::unique_ptr<TFile> ifile(TFile::Open(file_name.c_str(), "READ"));
  event.readFrom(ifile.get()).ignore();
  event.getEntry(0);
  const xAOD::JetContainer *jets = 0;
  event.retrieve(jets, "AntiKt4EMTopoJets").ignore();
  std::cout << "jets in entry: " << jets->size() << std::endl;

  return 0;
}

