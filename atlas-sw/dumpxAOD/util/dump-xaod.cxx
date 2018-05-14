// local tools
#include "Root/JetWriter.h"

// EDM things
#include "xAODJet/JetContainer.h"

// AnalysisBase tool include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

// 3rd party includes
#include "TFile.h"
#include "H5Cpp.h"

// stl includes
#include <stdexcept>
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

int main (int argc, char *argv[])
{
  const char* ALG = argv[0];
  // set up xAOD basics
  RETURN_CHECK(ALG, xAOD::Init());
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // set up output file
  H5::H5File output("output.h5", H5F_ACC_TRUNC);
  JetWriter jet_writer(output);

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
    RETURN_CHECK(ALG, event.readFrom(ifile.get()));

    // Loop over its events:
    const unsigned long long entries = event.getEntries();
    std::cout << "got " << entries << " entries" << std::endl;
    for (unsigned long long entry = 0; entry < entries; ++entry) {

      // Print some status
      if ( ! (entry % 500)) {
        std::cout << "Processing " << entry << "/" << entries << "\n";
      }

      // Load the event:
      bool ok = event.getEntry(entry) >= 0;
      if (!ok) throw std::logic_error("getEntry failed");

      const xAOD::JetContainer *jets = 0;
      RETURN_CHECK(ALG, event.retrieve(jets, "AntiKt4EMTopoJets"));

      for (const xAOD::Jet *jet : *jets) {
        jet_writer.write(*jet);
      }

    } // end event loop
  } // end file loop


  return 0;
}
