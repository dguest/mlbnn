// local tools
#include "Root/JetWriter.h"
#include "Root/JetClassifier.h"

// EDM things
#include "xAODJet/JetContainer.h"

// AnalysisBase tool include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

// 3rd party includes
#include "TFile.h"
#include "H5Cpp.h"
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

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
  std::string nn_file;
  std::string jet_collection;
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

  // maybe apply the NN we're training to this data?
  std::unique_ptr<const JetClassifier> classifier(nullptr);
  if (opts.nn_file.size() > 0) {
    std::ifstream input(opts.nn_file.c_str());
    classifier.reset(new JetClassifier(input));
  }

  // set up xAOD basics
  RETURN_CHECK(ALG, xAOD::Init());
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // set up output file
  H5::H5File output("output.h5", H5F_ACC_TRUNC);
  // see JetWriter.cxx for more on how the writer is defined
  auto jet_writer = getWriter(output, bool(opts.nn_file.size() > 0));

  // Loop over the specified files:
  for (std::string file_name: opts.files) {

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
      RETURN_CHECK(ALG, event.retrieve(jets, opts.jet_collection));

      for (const xAOD::Jet *jet : *jets) {
        if (jet->pt() > 20e3 && std::abs(jet->eta()) < 2.5) {
          if (classifier) classifier->decorate(*jet);
          jet_writer.fill(*jet);
        }
      }

    } // end event loop
  } // end file loop


  return 0;
}


// define the options parser
void usage(std::string name) {
  std::cout << "usage: " << name << " [-h]"
    " [--nn-file NN_FILE]"
    " [-c JET_COLLECTION]"
    " <AOD>..." << std::endl;
}

Options get_options(int argc, char *argv[]) {
  Options opts;
  opts.jet_collection = "AntiKtVR30Rmax4Rmin02TrackGhostTagJets";
  for (int argn = 1; argn < argc; argn++) {
    std::string arg(argv[argn]);
    if (arg == "--nn-file") {
      argn++;
      opts.nn_file = argv[argn];
    } else if (arg == "-c") {
      argn++;
      opts.jet_collection = argv[argn];
    } else if (arg == "-h") {
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
