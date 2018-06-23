#ifndef TRACK_WRITER_H
#define TRACK_WRITER_H

//////////////////////////////////////////////////////////////////////
// TrackWriter class
//////////////////////////////////////////////////////////////////////
//
// This is a more complicated example to write out a 2D HDF5
// dataset. Higher-dimensional cases should be relatively easy to
// generalize from this example.
//
//////////////////////////////////////////////////////////////////////

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
  // The functions that fill the output need to be defined when the
  // output is initialized. As a result, the container they fill from
  // also has to exist. In this case we're filling from a track
  // container, which is defined here.
  std::vector<const xAOD::TrackParticle*> m_tracks;

  // we're also going to hold on to the current jet, so that we can
  // calculate things like deltaR.
  const xAOD::Jet* m_jet;

  // In the case where we have sequences, the filler functions need to
  // step through the sequence and pick out individual entries. These
  // functions need an index that points to the current entry. Here we
  // make this index a member of the class: the filler functions can
  // then access it via the object `this` pointer.
  //
  // This index is a vector to support multi-dimensional outputs. In
  // this specific case the vector only needs to have one entry.
  std::vector<size_t> m_track_idx;

  // accessors for tracks
  typedef SG::AuxElement AE;
  typedef std::vector<ElementLink<DataVector<xAOD::IParticle> > > PartLinks;
  AE::ConstAccessor<PartLinks> m_ghost_accessor;

  // The writer itself
  H5Utils::WriterXd* m_writer;
};

#endif
