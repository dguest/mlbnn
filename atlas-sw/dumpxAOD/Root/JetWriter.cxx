// this class's header
#include "JetWriter.h"

// HDF5 things
#include "HDF5Utils/HdfTuple.h"
#include "H5Cpp.h"

// ATLAS things
#include "xAODJet/Jet.h"


JetWriter::JetWriter(H5::Group& output_group, bool write_nn):
  m_current_jet(nullptr),
  m_rnnip_pu("rnnip_pu"),
  m_rnnip_pb("rnnip_pb"),
  m_jf_sig("JetFitter_significance3d"),
  m_flavor_label("HadronConeExclExtendedTruthLabelID"),
  m_nn_light("nn_light"),
  m_nn_charm("nn_charm"),
  m_nn_bottom("nn_bottom"),
  m_writer(nullptr)
{
  // define the variable filling functions. Each function takes no
  // arguments, but includes a pointer to the class instance, and by
  // extension to the current jet.
  H5Utils::VariableFillers fillers;
  fillers.add<float>("rnnip_log_ratio", [this]() {
      double num = this->m_rnnip_pb(*this->m_current_jet->btagging());
      double denom = this->m_rnnip_pu(*this->m_current_jet->btagging());
      return std::log(num / denom);
    });
  fillers.add<float>("jf_sig", [this]() {
      return this->m_jf_sig(*this->m_current_jet->btagging());
    });
  fillers.add<int>("HadronConeExclExtendedTruthLabelID", [this]() {
      return this->m_flavor_label(*this->m_current_jet);
    });

  // if we're running the NN in this loop, we also save it
  if (write_nn) {
    fillers.add<float>("nn_light", [this](){
        return this->m_nn_light(*this->m_current_jet->btagging());
      });
    fillers.add<float>("nn_charm", [this](){
        return this->m_nn_charm(*this->m_current_jet->btagging());
      });
    fillers.add<float>("nn_bottom", [this](){
        return this->m_nn_bottom(*this->m_current_jet->btagging());
      });
  }

  m_writer = new H5Utils::WriterXd(output_group, "jets", fillers, {});
}

JetWriter::~JetWriter() {
  if (m_writer) m_writer->flush();
  delete m_writer;
}

void JetWriter::write(const xAOD::Jet& jet) {
  m_current_jet = &jet;
  m_writer->fillWhileIncrementing();
}
