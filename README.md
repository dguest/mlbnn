Matt LeBlanc Trains a Neural Network
====================================

Everyone loves Matt LeBlanc. He's charming, handsome, and a very talented particle physicist! But the last part is giving Matt trouble. Particle physicists have themselves in a bundle over this whole ''deep learning'' thing, you see. And Matt doesn't get it. But he has to get it, cause it's not going away and Matt is too young to start acting like a grumpy old man.

Matt needs to learn to train a neural network. Not a very good neural network---he has graduate students to do that. But he needs to understand the worlflow, the ideas, and most importantly he needs enough background to avoid falling for buzzword-first design principles.

This is a tutorial to teach Matt what he needs to know.

Prerequisites
-------------

Matt is an accomplished particle physicist. He already knows about:

 - The ATLAS xAOD EDM, git, ASG tools
 - Jet labels, calibration, all the physics
 - C++, python, numpy

He might need to learn a few things, but he doesn't have time to learn much. So we'll keep it to minimal use of:

 - hdf5
 - Keras
 - matplotlib

Matt isn't happy that there are so many trendy new packages in this list, but as long as we keep him using the few features he actually needs he'll manage.

**As a final note:** the later set of dependencies are non-standard in the ATLAS workflow but are _very_ common in the data science world and are easy to install with standard installation tools like `pip`. As such Matt will factorize things: first he'll produce data files on lxplus, then he'll run the training on his laptop or some other system with minimal dependencies on "HEP" tools.

We're also going to **force Matt to use Python 3**. Why? First off, it _is_ the future: Python 2 [will be deprecated very soon](https://pythonclock.org/). But beyond that, Matt already has 15 conflicting package managers for Python 2, so the Python 2 installation on his laptop is a smoking pile of garbage. But it's a pile of garbage where he got PyROOT working, so he can't afford to break it. The key is that Python 2 and Python 3 are _different packages_ from a package management point of view: we can play around with Python 3 _without breaking anything_ in Python 2.


Part 1: Data Pipeline
=====================

Matt has the best grad students, but they keep screwing up the data pipeline. They make it too complicated! Sure, maybe DxAOD -> TinyxAOD -> PhysicsNtuple -> miniTinyNtuple -> HDF5 -> pickled numpy got the job done, but now his paper is in approval and the Convener wants to make some "trival" check that requires rerunning everything! Not cool!

Matt wishes that his students had just produced their training dataset directly from the DxAOD, so that this would be an easy one-step process. This also gives us a nice example that will work outside your analysis group.

All the code to dump stuff lives in `atlas-sw`. Again, we want to separate "ATLAS" things from "ML" things. First we'll grab a simulated sample to work with.

```
rucio get --nrandom 1 mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_FTAG2.e6337_e5984_s3126_r9781_r9778_p3415/
```

Or for the truly lazy

```
cd atlas-sw
./get-dataset.sh
```

now Matt needs to build the dumping tool. Fortunately, there's already an example sitting in `atlas-sw`. He can build it with

```
cd atlas-sw
source dumpxAOD/setup.sh
mkdir build
cd build
cmake ../dumpxAOD
make
```

and then run it with

```
./x86_64-slc6-gcc62-opt/bin/dump-xaod <path-to-xaod>
```

this should produce an output file called `output.h5`. What the hell is that? Well, let's check:

```
h5ls -v output.h5
```

which gives something like this:

```
Opened "output.h5" with sec2 driver.
jets                     Dataset {140365/Inf}
    Location:  1:800
    Links:     1
    Chunks:    {2048} 24576 bytes
    Storage:   1684380 logical bytes, 748544 allocated bytes, 225.02% utilization
    Filter-0:  deflate-1 OPT {7}
    Type:      struct {
                   "rnnip_log_ratio"  +0    native float
                   "jf_sig"           +4    native float
                   "HadronConeExclExtendedTruthLabelID" +8    native int
               } 12 bytes
```

This tells that `output.h5` contains a dataset called `jets`, which contains roughly 140k jets. The other useful field is `Type` which says that we're storing a few fields, some as `native float` (these are the discriminants) and some as `native int` (these are labels). Also try `h5ls -dl output.h5`.

Looking in `dumpxAOD`, there are only a few files: in `util` there's a simple for loop over the events, and in `Root` there's a tool `JetWriter` which is called by the dumper loop.


Part 2: Training
================

The training part takes place outside any ATLAS environment. Nothing we're doing here depends on ROOT, so setup should be easy. All this code lives in `local-sw`.

Installing Keras and Friends
----------------------------

Assuming Matt is on his laptop, he should be able to install everything with

```
brew install python3
pip3 install keras h5py theano matplotlib
```

If this takes longer than 30 seconds something is wrong.

Running the training
--------------------

Hold on! Matt knows better than to train a neural network before he's even made a histogram. Fortunately we have a few short scripts to look at the dataset. First he downloads the `output.h5` file to the `data/` directory. Then he runs

```
./make_hist.py data/output.h5
./make_roc_curves.py data/output.h5
```

This creates a directory called `plots/` with a few plots to look at. (You can read these scripts, they aren't very long) Most importantly we see that we have several good discriminants for b vs light separation.

But maybe we can do better. To train a very simple neural network, Matt runs

```
./train_nn.py data/output.h5
```

This should take less than a minute, because it's an extremely simple network: one layer with two inputs and three output classes (basically logistic regression). The resulting network is stored in the `model/` directory, in two parts: architecture and weights. Both are easy to inspect: the architecture is a text file, while the weights can be dumped with `h5ls`.

Finally, Matt wants to check the performance. Both of the plotting scripts (`make_roc_curves.py` and `make_hists.py`) take an `--nn <arch> <weights>` argument. The NN should work _slightly_ better than the discriminants alone.

Of course we don't want to stop with slightly better, but to make a better network we'll need more inputs, more layers, and other fancy things which would detract from this example.


Part 3: Applying in Atlas
=========================

(Note: This will be expanded)

When we want to apply a neural network in ATLAS reconstruction, we have a problem: all this stuff uses Python, whereas Athena is C++. Fortunately we have a package to help, see [lwtnn][1]. For examples where this is applied, see [BoostedJetTaggers][2].

[1]: https://github.com/lwtnn/lwtnn
[2]: https://gitlab.cern.ch/atlas/athena/tree/21.2/Reconstruction/Jet/BoostedJetTaggers
