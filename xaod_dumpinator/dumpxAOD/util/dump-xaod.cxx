
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

int main (int argc, char *argv[])
{

  // set up xAOD basics
  xAOD::Init();
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

    // Loop over its events:
    const unsigned long long entries = event.getEntries();
    for (unsigned long long entry = 0; entry < entries; ++entry) {

      // Load the event:
      if (event.getEntry(entry) < 0) {
        throw std::logic_error(
          "Couldn't load entry " + std::to_string(entry) + " from file: "
          + file_name);
      }

      // Print some status:
      if ( ! (entry % 500)) {
        std::cout << "Processing " << entry << "/" << entries << "\n";
      }
      // const xAOD::JetContainer *raw_jets = 0;
      // auto full_collection = jet_collection + "Jets";
      // RETURN_CHECK( APP_NAME, event.retrieve(raw_jets, full_collection) );

      // for (const xAOD::Jet *raw_jet : *raw_jets) {
      //   std::unique_ptr<xAOD::Jet> jet(nullptr);
      //   xAOD::Jet* jet_ptr(nullptr);
      //   calib_tool.calibratedCopy(*raw_jet, jet_ptr);
      //   jet.reset(jet_ptr);
      //   if (jet->pt() < 250*GeV || std::abs(jet->eta()) > 2.0) {
      //     continue;
      //   }
      //   if (tagger.isInitialized()) tagger->decorate(*jet);

      //   // get the subjets
      //   const xAOD::Jet* parent_jet = *acc_parent(*jet);
      //   if (!parent_jet) throw std::logic_error("no valid parent");
      //   auto subjet_links = acc_subjets(*parent_jet);
      //   std::vector<const xAOD::Jet*> subjets;
      //   for (const auto& el: subjet_links) {
      //     const auto* jet = dynamic_cast<const xAOD::Jet*>(*el);
      //     if (!jet) throw std::logic_error("subjet is invalid");
      //     if (jet->pt() > 7e3) {
      //       subjets.push_back(jet);
      //     }
      //   }
      //   std::sort(subjets.begin(), subjets.end(), pt_sort);

      // } // end jet loop

    } // end event loop
  } // end file loop


  return 0;
}
