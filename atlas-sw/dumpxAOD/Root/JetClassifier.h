#ifndef JET_CLASSIFIER_H
#define JET_CLASSIFIER_H

// forward declare EDM things
namespace xAOD {
  class Jet_v1;
  typedef Jet_v1 Jet;
}
// forward declare lwtnn things
namespace lwt {
  class LightweightGraph;
  class NanReplacer;
}

// EDM includes
#include "AthContainers/AuxElement.h"

// C++ includes
#include <istream>
#include <memory>

class JetClassifier
{
public:
  JetClassifier(std::istream& input_config);
  void decorate(const xAOD::Jet& jet) const;

private:
  // accessors for input variabls
  typedef SG::AuxElement AE;
  AE::ConstAccessor<double> m_rnnip_pu;
  AE::ConstAccessor<double> m_rnnip_pb;
  AE::ConstAccessor<float> m_jf_sig;

  // decorators for outputs
  AE::Decorator<float> m_nn_light;
  AE::Decorator<float> m_nn_charm;
  AE::Decorator<float> m_nn_bottom;

  // lightweight graph and preprocessor
  std::unique_ptr<lwt::LightweightGraph> m_graph;
  std::unique_ptr<lwt::NanReplacer> m_replacer;
};

#endif
