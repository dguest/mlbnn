#ifndef TRACK_WRITER_H
#define TRACK_WRITER_H

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
#include "xAODTracking/TrackParticleContainer.h"

class TrackWriter
{
public:
  // constructor: the writer will create the output dataset in some group
  TrackWriter(H5::Group& output_group);

  // destructor (takes care of flushing output file too)
  ~TrackWriter();

  // we want to disable copying and assignment, it's not trivial to
  // make this play well with output files
  TrackWriter(TrackWriter&) = delete;
  TrackWriter operator=(TrackWriter&) = delete;

  // function that's called to read the tracks out of the jet and
  // write them.
  void write(const xAOD::Jet& jet);

private:
  // the functions that fill the output need to be defined when the
  // class is initialized. The container they fill from has to exist
  // when they are defined, so they will fill from this track vector.
  std::vector<const xAOD::TrackParticle*> m_tracks;

  // we're also going to hold on to the current jet, so that we can
  // calculate things like deltaR.
  const xAOD::Jet* m_jet;

  // In the case where we have sequences, the filler functions need to
  // step through the sequence and pick out individual entries. These
  // entries are indexed by a counter which the output writer
  // increments. We define this counter here so that the filler
  // functions can find it.
  std::vector<size_t> m_track_idx;

  // accessors for tracks
  typedef SG::AuxElement AE;
  typedef std::vector<ElementLink<DataVector<xAOD::IParticle> > > PartLinks;
  AE::ConstAccessor<PartLinks> m_ghost_accessor;

  // The writer itself
  H5Utils::WriterXd* m_writer;
};

#endif
