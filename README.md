Matt LeBlanc Trains a Neural Network
====================================

Everyone loves matt leblanc. He's charming, handsome, and a very talented particle physicist! But the last part is giving Matt trouble. Particle physicists have themselves in a bundle over this whole ``deep learning'' thing, you see. And Matt doesn't get it. But he has to get it, cause it's not going away and Matt is too young to start acting like a grumpy old man.

Matt needs to learn to train a neural network. Not a very good neural network---he has graduate students to do that. But he needs to understand the worlflow, the ideas, and most importantly he needs enough background to avoid falling for buzzword-first design principals.

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

We're also going to **force Matt to use python 3**. Why? First off, it _is_ the future: python 2 will be deprecated very soon. But beyond that, Matt already has 15 conflicting package managers for python 2, so the python 2 installation on his laptop is a smoking pile of garbage already. But it's a pile of garbage where he got PyRoot working, so we won't mess with that.


Part 1: Data Pipeline
=====================

Matt has the best grad students. They are diligent. They never complain. They never sleep. They don't even eat unless the food uses AtlasStyle. You've heard of the top 5 grad students on ATLAS, Matt's are better than all those. We just have to get Matt up and running so that he can supervise these badass minions.

One thing that they keep screwing up is the data pipeline. They make it too complicated! Sure, maybe DxAOD -> TinyxAOD -> PhysicsNtuple -> miniTinyNtuple -> HDF5 -> pickled numpy got the job done, but now his paper is in approval and Convener Mc Jerkface wants to make some "trival" check that requires rerunning everything! Not cool!

Matt wishes that his students had just produced their silly training dataset directly from the DxAOD, so that this would be an easy one-step process.

### Aside: Frameworks are Born to Die ###

Analysis frameworks are the first thing to die when a postdoc stops writing code. Matt doesn't want to see an example of how to train neural networks in CommonxAODHelperTools, even though he's an expert, because his grad student only knows TAnalFactory. They were both once understood by the postdoc who wrote them, but that guy works at Apple now. Matt just needs something simple, not a framework. The bells and whistles like calibration tools can be added in later.

### Minimal Dumping Tool ###

Since xAODs are the only universal API that ATLAS has for data, Matt is going to dump directly from that. All the code to do this stuff lives in `atlas-sw`. Again, we want to separate crufty "ATLAS" things from "ML" things. First we'll grab a simulated sample to work with.

```
rucio get --nrandom 1 mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.deriv.DAOD_FTAG2.e6337_e5984_s3126_r9781_r9778_p3415/
```

Or for the truly lazy

```
cd atlas-sw
./get-dataset.sh
```

now Matt needs to build the dumping tool. Fortunately, there's already an example sitting in `atlas-sw`.


Part 2: Training
================

To be done...


Part 3: Applying in Atlas
=========================

To be done...
