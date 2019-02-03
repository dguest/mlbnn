// this class's header
#include "TrackWriter.h"

// HDF5 things
#include "HDF5Utils/Writer.h"
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
TrackWriter::TrackWriter(H5::Group& output_group):
  m_ghost_accessor("GhostTrack"),
  m_writer(nullptr)
{
  // we operate on pairs of Jets and Tracks. Note that we need to
  // provide a default value here since some of the outputs might end
  // up empty.
  H5Utils::Consumers<const JetTrack&> fillers;
  fillers.add<float>(
    "pt", [](const JetTrack& jt) { return jt.track->pt();}, NAN);

  // We'll add another variable just for fun
  fillers.add<float>(
    "eta",[](const JetTrack& jt) {return jt.track->pt();}, NAN);

  // also save the deltaR with respect to the jet
  fillers.add<float>(
    "deltaR", [](const JetTrack& jt) {
                return jt.track->p4().DeltaR(jt.jet->p4());
              }, NAN);

  // Now we define the writer. Note that the last argument gives the
  // dimensions of the output array. If the last argument is an empty
  // vector ({}), the output array will be 1d.
  //
  // In this case we take the first 20 tracks, so the output is 2d,
  // with dimensions {N, 20} where N is the number of times that write
  // is called. It can be extended to higher dimensions too, i.e. you
  // could fill a {N, 20, 30} array by passing in {20, 30} here.
  m_writer.reset(new JTWriter(output_group, "tracks", fillers, {20}));
}


//////////////////////////////////////////////////////////////////////
// Write function
//////////////////////////////////////////////////////////////////////
//
// This gets called each time we want to write out a jet.
//
// This case is slightly more complicated: we also do some processing
// and arranging the outputs.
//
void TrackWriter::write(const xAOD::Jet& jet) {

  // We're going to do a bit of processing on the tracks before
  // writing them out.
  //
  std::vector<JetTrack> pairs;
  // Grab the ghost links and loop over them
  for (const auto& link: m_ghost_accessor(jet)) {

    // We do a lot of validity checking on various links and
    // casts. This way a truely desperate physicist can catch the
    // exceptions and continue on. But please, for the love of god,
    // don't ignore these! If any of these conditions fail go back and
    // figure out why!
    if (!link.isValid()) throw std::logic_error("invalid particle link");

    // Read the particle link
    const xAOD::IParticle* part = *link;

    // Cast to a track particle. Note that if all we care about is
    // kinematics we don't have to make this cast at all.
    const auto* track = dynamic_cast<const xAOD::TrackParticle*>(part);
    //
    // it's _possible_ that the GhostTracks aren't actuall Track
    // particles (althogh I'm not sure how that would happen), so we
    // want to check the dynamic cast just to be sure. This cast will
    // return zero if it's not actually a track particle.
    if (!track) throw std::logic_error("particle is not a TrackParticle");

    pairs.push_back({&jet,track});
  }

  // We might as well sort these tracks by pt. If the sequence we're
  // saving is shorter than the number of tracks they will be
  // truncated. By sorting by descending pt, tracks we ignore will be
  // the softest in the jet.
  std::sort(pairs.begin(), pairs.end(),
            [](const auto& p1, const auto& p2) {
              return p1.track->pt() > p2.track->pt();
            });

  m_writer->fill(pairs);
}
