#ifndef JET_WRITER_H
#define JET_WRITER_H

//////////////////////////////////////////////////////////////////////
// JetWriter class
//////////////////////////////////////////////////////////////////////
//
// This is a minmal example to dump an xAOD::Jet to an HDF5 file. The
// result is a 1D structured output array, with one entry per jet. For
// more complicated examples (i.e. sequence outputs) see the
// `/advanced` directory.
//
//////////////////////////////////////////////////////////////////////


// EDM includes
#include "xAODJet/Jet.h"
#include "HDF5Utils/Writer.h"

class JetWriter
{
public:
  // constructor: the writer will create the output dataset in some group
  JetWriter(H5::Group& output_group, bool save_nn);

  // destructor (takes care of flushing output file too)
  ~JetWriter();

  // we want to disable copying and assignment, it's not trivial to
  // make this play well with output files
  JetWriter(JetWriter&) = delete;
  JetWriter operator=(JetWriter&) = delete;

  // function that's actually called to write the jet
  void write(const xAOD::Jet& jet);

private:

  // The writer itself
  typedef H5Utils::Writer<0,const xAOD::Jet&> JetWriter_t;
  std::unique_ptr<JetWriter_t> m_writer;
};

#endif
