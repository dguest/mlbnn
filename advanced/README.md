More Advanced Examples
======================

This is a place to collect more advanced examples that would have
scared Matt off if they were part of the top level readme. As with
everything in this repository, most of the specific information and
explanation is in comments in the programs.

Advanced dumper utils
---------------------

Under `dumper` there are some more sophisticated tools to create
datasets.

 - `dump-tracks.cxx` is an example that writes out a 2d array of
   tracks, one row per jet.
 - `dump-events.cxx` is an example that writes events using variable
   length arrays to store jets. These are saved as two datasets: one
   which contains all the jets, another which specifies the offset of
   the first jet in each event. They are reconstructed as jets using
   [uproot-methods][0] in `scripts/read-four-vectors.py`.


[0]: https://github.com/scikit-hep/uproot-methods
