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

// EDM includes
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "HDF5Utils/Writer.h"

#include <memory>

class TrackWriter
{
public:
  // constructor: the writer will create the output dataset in some group
  TrackWriter(H5::Group& output_group);

  // we want to disable copying and assignment, it's not trivial to
  // make this play well with output files
  TrackWriter(TrackWriter&) = delete;
  TrackWriter operator=(TrackWriter&) = delete;

  // function that's called to read the tracks out of the jet and
  // write them.
  void write(const xAOD::Jet& jet);

private:

  // We want to have pairs of (jet, track) so that we can save
  // variables describing the relative kinematics.
  struct JetTrack
  {
    const xAOD::Jet* jet;
    const xAOD::TrackParticle* track;
  };

  // The writer template takes two parameters:
  //
  //  - The first is the rank of the output. In this case we're rank 1
  //    because we're storing a vector of tracks for each jet.
  //
  //  - The second is the input type. Since we want to write out
  //    information which relates to both the jet and the track we use
  //    the JetTrack structure defined above.
  //
  typedef H5Utils::Writer<1,const JetTrack&> JTWriter;

  // accessors for tracks
  typedef SG::AuxElement AE;
  typedef std::vector<ElementLink<DataVector<xAOD::IParticle> > > PartLinks;
  AE::ConstAccessor<PartLinks> m_ghost_accessor;

  // The writer itself
  std::unique_ptr<JTWriter> m_writer;
};

#endif
