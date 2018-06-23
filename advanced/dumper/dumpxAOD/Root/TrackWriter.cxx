// this class's header
#include "TrackWriter.h"

// HDF5 things
#include "HDF5Utils/HdfTuple.h"
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
  m_jet(nullptr),
  m_track_idx(1),
  m_ghost_accessor("GhostTrack"),
  m_writer(nullptr)
{
  // Define the variable filling functions. Each function takes no
  // arguments, but includes a `this` pointer to the object. From the
  // object pointer the fillers can access the tracks.
  H5Utils::VariableFillers fillers;
  fillers.add<float>("pt", [this]() -> float {

      // Look up the track index. This is a vector so that we can
      // support higher-dimensional outputs, but in this case we're
      // restricted to 1D, and thus only need the first entry.
      size_t idx = this->m_track_idx.at(0);

      // Tracks are a variable-length sequences. Numpy and HDF5 arrays
      // are much more efficinet when they are fixed-length, so we
      // fill out to some fixed index. When this index is out of
      // bounds we fill with NAN.
      if (this->m_tracks.size() <= idx) return NAN;

      // Assuming that we're in bounds, return something (pt here)
      return this->m_tracks.at(idx)->pt();
    });

  // We'll add another variable just for fun
  fillers.add<float>("eta", [this]() -> float {
      size_t idx = this->m_track_idx.at(0);
      if (this->m_tracks.size() <= idx) return NAN;
      return this->m_tracks.at(idx)->eta();
    });

  // also save the deltaR with respect to the jet
  fillers.add<float>("deltaR", [this]() -> float {
      size_t idx = this->m_track_idx.at(0);
      if (this->m_tracks.size() <= idx) return NAN;
      return this->m_tracks.at(idx)->p4().DeltaR(this->m_jet->p4());
    });

  // Now we define the writer. Note that the last argument gives the
  // dimensions of the output array. If the last argument is an empty
  // vector ({}), the output array will be 1d.
  //
  // In this case we take the first 20 tracks, so the output is 2d,
  // with dimensions {N, 20} where N is the number of times that write
  // is called. It can be extended to higher dimensions too, i.e. you
  // could fill a {N, 20, 30} array by passing in {20, 30} here.
  m_writer = new H5Utils::WriterXd(output_group, "tracks", fillers, {20});
}


TrackWriter::~TrackWriter() {
  if (m_writer) m_writer->flush();
  delete m_writer;
}


//////////////////////////////////////////////////////////////////////
// Write function
//////////////////////////////////////////////////////////////////////
//
// This gets called each time we want to write out a jet. In simpler
// cases all it just updates some pointers in the object, so that the
// filler functions can find the data to write out.
//
// This case is slightly more complicated: we also do some processing
// and arranging the outputs.
//
void TrackWriter::write(const xAOD::Jet& jet) {
  // we clear the track vector first (so we don't forget later!)
  m_tracks.clear();

  // We're going to do a bit of processing on the tracks before
  // writing them out.
  //
  // First grab the ghost links and loop over them
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

    m_tracks.push_back(track);
  }

  // We might as well sort these tracks by pt. If the sequence we're
  // saving is shorter than the number of tracks they will be
  // truncated. By sorting by descending pt, tracks we ignore will be
  // the softest in the jet.
  std::sort(m_tracks.begin(), m_tracks.end(),
            [](const auto* t1, const auto* t2) {
              return t1->pt() > t2->pt();
            });

  // We also have to set the current jet pointer, since some of the
  // outputs depend on the jet kinematics
  m_jet = &jet;

  // Finally ask the writer to fill the output file. We pass it the
  // track index so that it can increment after each fill. In this
  // case we only loop over each track, but for higher dimensional
  // outputs every index is looped over.
  m_writer->fillWhileIncrementing(m_track_idx);
}
