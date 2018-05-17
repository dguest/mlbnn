#include "Root/JetClassifier.h"

// EDM
#include "xAODJet/Jet.h"

// Externals
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"

JetClassifier::JetClassifier(std::istream& stream):
  m_rnnip_pu("rnnip_pu"),
  m_rnnip_pb("rnnip_pb"),
  m_jf_sig("JetFitter_significance3d"),
  m_nn_light("nn_light"),
  m_nn_charm("nn_charm"),
  m_nn_bottom("nn_bottom"),
  m_graph(nullptr),
  m_replacer(nullptr)
{
}

void JetClassifier::decorate(const xAOD::Jet& jet) const {
}
