// local tools
#include "Root/JetClassifier.h"

// EDM things
#include "xAODJet/JetContainer.h"
#include "HDF5Utils/Writer.h"

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

///////////////////////////////////////////////////////////////////////
// Setup Consumers
///////////////////////////////////////////////////////////////////////
//
// We define "Consumers" which run on whatever object we want to write
// (in this case a Jet). Each consumer returns some primative type
// which is written to the output file.
//
// See the function definition below.
//
H5Utils::Consumers<const xAOD::Jet&> getConsumers();
//
// This function adds the nn outputs to the consumer (if we decide to
// run the NN we just trained).
void addNN(H5Utils::Consumers<const xAOD::Jet&>&);

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

  // Set up the consumer functions
  H5Utils::Consumers<const xAOD::Jet&> consumers = getConsumers();

  // If the user passed in an nn, we'll want to add those outputs as
  // well.
  if (opts.nn_file.size() > 0) addNN(consumers);

  // The first argument for the template is the rank of the output. We
  // could be writing out multi-dimensional arrays here, but for this
  // simple example we'll just save a scalar (rank 0), i.e. one Jet
  // per entry. This scalar value can have multiple fields, one for
  // each consumer we define.
  //
  // The result is a 1D structured output array, with one entry per
  // jet.
  //
  // See the "advanced" examples for something more complicated.
  //
  H5Utils::Writer<0,const xAOD::Jet&> jet_writer(output, "jets", consumers);

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

//////////////////////////////////////////////////////////////////////
// Definition for the option parser
//////////////////////////////////////////////////////////////////////
//
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


//////////////////////////////////////////////////////////////////////
// Build Consumers
//////////////////////////////////////////////////////////////////////
//
// This is where the "consumer" functions are defined, which are
// responsible for copying variables out of EDM objects and into the
// output file.
//
H5Utils::Consumers<const xAOD::Jet&> getConsumers() {
  using xAOD::Jet;
  typedef SG::AuxElement AE;

  // Define the container for the consumers. In this case we're
  // templated to eat jets.
  H5Utils::Consumers<const Jet&> consumers;

  // get some accessors for b-tagging variables
  AE::ConstAccessor<double> rnn_pu("rnnip_pu");
  AE::ConstAccessor<double> rnn_pb("rnnip_pb");

  // Each consumer has a return type (here "float"), a name for the
  // output variable ("rnnip_log_ratio"), and a function which returns
  // the desired type.
  consumers.add<float>("rnnip_log_ratio",
                       [rnn_pu, rnn_pb](const Jet& j) {
                         const xAOD::BTagging* btag = j.btagging();
                         double num = rnn_pb(*btag);
                         double denom = rnn_pu(*btag);
                         return std::log(num / denom);
                       });
  AE::ConstAccessor<float> jf_sig("JetFitter_significance3d");
  consumers.add<float>("jf_sig",
                       [jf_sig](const Jet& j) {
                         return jf_sig(*j.btagging());
                       });
  std::string label_name = "HadronConeExclExtendedTruthLabelID";
  AE::ConstAccessor<int> label(label_name);
  consumers.add<int>(label_name, [label](const Jet& j) { return label(j); });
  return consumers;
}

void addNN(H5Utils::Consumers<const xAOD::Jet&>& consumers) {
  using xAOD::Jet;
  typedef SG::AuxElement AE;

  AE::ConstAccessor<float> light("nn_light");
  consumers.add<float>("nn_light", [light](const Jet& j){
                                   return light(*j.btagging());
                                 });
  AE::ConstAccessor<float> charm("nn_charm");
  consumers.add<float>("nn_charm", [charm](const Jet& j){
                                   return charm(*j.btagging());
                                 });
  AE::ConstAccessor<float> bottom("nn_bottom");
  consumers.add<float>("nn_bottom", [bottom](const Jet& j){
                                    return bottom(*j.btagging());
                                  });
}
