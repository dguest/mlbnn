#ifndef JET_WRITER_H
#define JET_WRITER_H

//////////////////////////////////////////////////////////////////////
// Function to build JetWriter
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

// The writer itself
typedef H5Utils::Writer<0,const xAOD::Jet&> JetWriter;
std::unique_ptr<JetWriter> getWriter(H5::Group& output_group, bool write_nn);


#endif
