// EDM things
#include "xAODJet/JetContainer.h"

// AnalysisBase tool include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

// root includes
#include "TFile.h"

// stl includes
#include <stdexcept>
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

int main (int argc, char *argv[])
{

  // set up xAOD basics
  xAOD::TReturnCode::enableFailure();
  assert(xAOD::Init().isSuccess());
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // Loop over the specified files:
  for (int file_n = 1; file_n < argc; ++file_n) {
    std::string file_name = argv[file_n];
    // Open the file:
    std::unique_ptr<TFile> ifile(TFile::Open(file_name.c_str(), "READ"));
    if ( ! ifile.get() || ifile->IsZombie()) {
      throw std::logic_error("Couldn't open file: " + file_name);
      return 1;
    }
    std::cout << "Opened file: " << file_name << std::endl;

    // Connect the event object to it:
    assert(event.readFrom(ifile.get()).isSuccess());

    // Loop over its events:
    const unsigned long long entries = event.getEntries();
    for (unsigned long long entry = 0; entry < entries; ++entry) {

      // Load the event:
      assert(event.getEntry(entry) >= 0);

      // Print some status:
      if ( ! (entry % 500)) {
        std::cout << "Processing " << entry << "/" << entries << "\n";
      }
      const xAOD::JetContainer *jets = 0;
      assert(event.retrieve(jets, "AntiKt4EMTopoJets").isSuccess());

      for (const xAOD::Jet *jet : *jets) {
        std::cout << jet->pt() << std::endl;
      }

    } // end event loop
  } // end file loop


  return 0;
}
