// this class's header
#include "JetWriter.h"

// HDF5 things
#include "H5Cpp.h"

// ATLAS things
#include "xAODJet/Jet.h"

//////////////////////////////////////////////////////////////////////
// Class constructor
//////////////////////////////////////////////////////////////////////
//
// This is where the "filler" functions are defined, which are
// responsible for copying variables out of EDM objects and into the
// output file.
//
JetWriter::JetWriter(H5::Group& output_group, bool write_nn):
  m_writer(nullptr)
{
  using xAOD::Jet;
  typedef SG::AuxElement AE;

  // Define the variable filling functions.
  H5Utils::Consumers<const Jet&> fillers;
  AE::ConstAccessor<double> rnn_pu("rnnip_pu");
  AE::ConstAccessor<double> rnn_pb("rnnip_pb");
  fillers.add<float>("rnnip_log_ratio",
                     [rnn_pu, rnn_pb](const Jet& j) {
                       const xAOD::BTagging* btag = j.btagging();
                       double num = rnn_pb(*btag);
                       double denom = rnn_pu(*btag);
                       return std::log(num / denom);
                     });
  AE::ConstAccessor<float> jf_sig("JetFitter_significance3d");
  fillers.add<float>("jf_sig",
                     [jf_sig](const Jet& j) {
                       return jf_sig(*j.btagging());
                     });
  std::string label_name = "HadronConeExclExtendedTruthLabelID";
  AE::ConstAccessor<int> label(label_name);
  fillers.add<int>(label_name, [label](const Jet& j) { return label(j); });

  // if we're running the NN in this loop, we also save it
  if (write_nn) {
    AE::ConstAccessor<float> light("nn_light");
    fillers.add<float>("nn_light", [light](const Jet& j){
                                     return light(*j.btagging());
                                   });
    AE::ConstAccessor<float> charm("nn_charm");
    fillers.add<float>("nn_charm", [charm](const Jet& j){
                                     return charm(*j.btagging());
                                   });
    AE::ConstAccessor<float> bottom("nn_bottom");
    fillers.add<float>("nn_bottom", [bottom](const Jet& j){
                                      return bottom(*j.btagging());
                                    });
  }

  m_writer.reset(new JetWriter_t(output_group, "jets", fillers));
}

JetWriter::~JetWriter() {
  if (m_writer) m_writer->flush();
}

//////////////////////////////////////////////////////////////////////
// Write function
//////////////////////////////////////////////////////////////////////
//
// This gets called each time we want to write out a jet.
//
void JetWriter::write(const xAOD::Jet& jet) {
  m_writer->fill(jet);
}
