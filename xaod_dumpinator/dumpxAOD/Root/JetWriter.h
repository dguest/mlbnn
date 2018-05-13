#ifndef JET_WRITER_H
#define JET_WRITER_H

// forward declare HDF5 things
namespace H5 {
  class Group;
}
namespace H5Utils {
  class VariableFillers;
  class WriterXd;
}

// forward declare EDM things
namespace xAOD {
  class Jet_v1;
  typedef Jet_v1 Jet;
}

// EDM includes
#include "AthContainers/AuxElement.h"

class JetWriter
{
public:
  // constructor: the writer will create the output dataset in some group
  JetWriter(H5::Group& output_group);

  // destructor (takes care of flushing output file too)
  ~JetWriter();

  // we want to disable copying and assignment, it's not trivial to
  // make this play well with output files
  JetWriter(JetWriter&) = delete;
  JetWriter operator=(JetWriter&) = delete;

  // function that's actually called to write the jet
  void write(const xAOD::Jet& jet);

private:
  // the functions that fill the output need to be defined when the
  // class is initialized. They will fill from this jet pointer, which
  // must be updated each time we wright.
  const xAOD::Jet* m_current_jet;

  // accessors for jet / b-tagging things
  typedef SG::AuxElement AE;
  AE::ConstAccessor<double> m_rnnip_pu;
  AE::ConstAccessor<double> m_rnnip_pb;
  AE::ConstAccessor<float> m_jf_sig;

  // The writer itself
  H5Utils::WriterXd* m_writer;
};

#endif
