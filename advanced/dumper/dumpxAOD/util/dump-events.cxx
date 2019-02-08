// EDM things
#include "xAODJet/JetContainer.h"

// AnalysisBase tool include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#include "HDF5Utils/Writer.h"

// 3rd party includes
#include "TFile.h"
#include "H5Cpp.h"

// stl includes
#include <stdexcept>
#include <string>
#include <iostream>
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


////////////////////////////
// event struct           //
////////////////////////////
typedef unsigned int index_t;
struct Event
{
  index_t firstJet;
  index_t nJets;
};

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

  // add event consumers
  H5Utils::Consumers<const Event&> econ;
  econ.add<index_t>("firstJet", [](const Event& e) { return e.firstJet; });
  econ.add<index_t>("nJets", [](const Event& e) { return e.nJets; });
  H5Utils::Writer<0, const Event&> ewriter(output, "event", econ);

  // add jet consumers
  using xAOD::Jet;
  H5Utils::Consumers<const Jet&> jcon;
  jcon.add<float>("pt"  , [](const Jet& j) { return j.pt();  });
  jcon.add<float>("eta" , [](const Jet& j) { return j.eta(); });
  jcon.add<float>("phi" , [](const Jet& j) { return j.phi(); });
  jcon.add<float>("m"   , [](const Jet& j) { return j.m(); });

  // let's also add some b-tagging info
  typedef SG::AuxElement AE;
  AE::ConstAccessor<double> pb("DL1_pb");
  AE::ConstAccessor<double> pc("DL1_pc");
  AE::ConstAccessor<double> pu("DL1_pu");
  jcon.add<float>("btag", [pb, pc, pu](const Jet& j) {
                            const xAOD::BTagging* bp = j.btagging();
                            if (!bp) throw std::runtime_error("missing b");
                            const xAOD::BTagging& b = *bp;
                            return pb(b) / (pc(b)*0.1 + pu(b)*0.9);
                          });
  H5Utils::Writer<0, const Jet&> jwriter(output, "jet", jcon);

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

      Event event;
      event.firstJet = jwriter.index();
      for (const xAOD::Jet *jet : *jets) {
        jwriter.fill(*jet);
      }
      event.nJets = jwriter.index() - event.firstJet;
      ewriter.fill(event);

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
