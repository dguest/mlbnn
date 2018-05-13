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

As a final note: the later set of dependencies are non-standard in the ATLAS workflow but are _very_ common in the data science world and are easy to install with standard installation tools like `pip`. As such Matt will factorize things: first he'll produce data files on lxplus, then he'll run the training on his laptop or some other system with minimal dependencies on "HEP" tools.


Part 0: Choosing the problem
============================

Long lived particle searches are super trendy this year. Matt doesn't want to miss out, but he also doesn't want to jump into an overcrowded group with no plan but to elbow his way to the front. There are plenty of people doing that on ATLAS already. He needs a cool new idea. How about this: we have algorithms finders for b-tagging that work ok, let's reoptimize them for long lived particles!

Again Matt doesn't have time to do this whole project himself, that's what grad students are for. We just want to get him up and running so that he can supervise his badass minions. So we'll start by training a basic b-tagger, and then we can use these ideas to look for more interesting things.


Part 1: Data Pipeline
=====================

Matt cares deeply about his students. One thing that they keep screwing up is the data pipeline. They make it too complicated! Sure, maybe DxAOD -> TinyxAOD -> EgammaNtuple -> miniTinyNtuple -> HDF5 -> pickled numpy got the job done, but now his paper is in approval and Convener Mc Jerkface wants to make some "trival" check that requires rerunning everything! Not cool!

Matt wishes that his students had just produced their silly training dataset directly from the DxAOD, so that this would be an easy one-step process.

### Aside: Frameworks are Stupid ###

Analysis frameworks are stupid. Every grad student should write one, but once you've seen one you've seen them all: they run over a list of datasets and write out histograms with systematic variations. Most of the code in frameworks is just workarounds for bad tool interfaces, and as such the main goal of every framework should be to die gracefully once the underlying tools have been fixed. The goal of the framework maintainer should be to fix the underlying tools so that the next generation of frameworks can be simpler.

Matt doesn't want to see an example of how to train neural networks in CommonxAODHelperTools, even though he's an expert, because his grad student only knows TAnalysisHistFactory. They are both piles of garbage that were once understood by the postdoc who wrote them, but that guy works at Apple now. So Matt just needs to see a minimal working example. The bells and whistles like calibration tools can be added in later.

### Minimal Dumping Tool ###

Since xAODs are the only universal API that ATLAS has for data, Matt is going to dump directly from that. First we'll grab a simulated sample to work with.

Part 2: Training
================

