// local tools
#include "Root/TrackWriter.h"

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
#include <fstream>
#include <memory>
#include <cassert>

//////////////////////////////
// simple options struct    //
//////////////////////////////
struct Options
{
  std::vector<std::string> files;
};
// simple options parser
Options get_options(int argc, char *argv[]);


//////////////////
// main routine //
//////////////////
int main (int argc, char *argv[])
{
  const char* ALG = argv[0];
  Options opts = get_options(argc, argv);

  // set up xAOD basics
  RETURN_CHECK(ALG, xAOD::Init());
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // set up output file
  H5::H5File output("output.h5", H5F_ACC_TRUNC);
  TrackWriter track_writer(output);

  // Loop over the specified files:
  for (std::string file_name: opts.files) {

    // Open the file:
    std::unique_ptr<TFile> ifile(TFile::Open(file_name.c_str(), "READ"));
    if ( ! ifile.get() || ifile->IsZombie()) {
      throw std::logic_error("Couldn't open file: " + file_name);
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
        track_writer.write(*jet);
      }

    } // end event loop
  } // end file loop


  return 0;
}


// define the options parser
void usage(std::string name) {
  std::cout << "usage: " << name << " [-h] <AOD>..." << std::endl;
}

Options get_options(int argc, char *argv[]) {
  Options opts;
  for (int argn = 1; argn < argc; argn++) {
    std::string arg(argv[argn]);
    if (arg == "-h") {
      usage(argv[0]);
      exit(1);
    } else {
      opts.files.push_back(arg);
    }
  }
  if (opts.files.size() == 0) {
    usage(argv[0]);
    exit(1);
  }
  return opts;
}
