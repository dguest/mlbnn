Matt LeBlanc Trains a Neural Network
====================================

Everyone loves matt leblanc. He's charming, handsome, and a very talented particle physicist! But the last part is giving Matt trouble. Particle physicists have themselves in a bundle over this whole ``deep learning'' thing, you see. And Matt doesn't get it. But he has to get it, cause it's not going away and Matt is too young to start acting like a grumpy old man.

Matt needs to learn to train a neural network. Not a very good neural network---he has graduate students to do that. But he needs to understand the worlflow, the ideas, and most importantly he needs enough background to avoid falling for buzzword-first design principals.

This is a tutorial to teach Matt what he needs to know.

Prerequisites
-------------

Matt is an accomplished particle physicist. He already knows all about:

 - The ATLAS xAOD EDM
 - Jet labels, calibration
 - C++

He might need to learn a few things, but he doesn't have time to learn much. So we'll keep it to minimal use of:

 - numpy
 - hdf5
 - Keras


Part 1: Data Pipeline
=====================

Matt cares deeply about his students. One thing that they keep screwing up is the data pipeline. They make it too complicated! Sure, maybe DxAOD -> TinyxAOD -> EgammaNtuple -> miniTinyNtuple -> HDF5 -> pickled numpy got the job done, but now his paper is in approval and Convener Mc Jerkface wants to make some "trival" check that requires rerunning everything! Not cool!

Matt wishes that his students had just produced their silly training dataset directly from the DxAOD, so that this would be an easy one-step process.


Part 2: Training
================

Matt heard that long lived particle searches are super trendy this year. He doesn't want to miss out, but he also doesn't want to jump right into the middle of the hottest search on ATLAS: is's already too crowded and he won't add anything useful there! So he's going to be clever. We could do a much better job of reconstructing particles with a lifetime of O(mm), if we just used b-tagging information.
