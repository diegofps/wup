#!/usr/bin/env python
#coding=UTF-8

from random import random
from wup import Wisard

TRAIN_ITERS = 1000
TEST_ITERS = 1000
LENGTH = 128

RAM_BITS = 16
CLASSES = 10

FILENAME = "wisard.bin"

def noisify(src, dst, noise):
    for index, value in enumerate(src):
        if random() < noise:
            dst[index] = round(random())
        else:
            dst[index] = value
    
    return dst

def test(noise):
    patterns = [[round(random()) for __ in xrange(LENGTH)] for _ in xrange(CLASSES)]
    tmp = [0] * LENGTH
    misses = 0
    hits = 0
    
    w = Wisard(LENGTH, RAM_BITS, CLASSES)
    
    for i in xrange(CLASSES):
        for _ in xrange(TRAIN_ITERS):
            tmp = noisify(patterns[i], tmp, noise)
            w.learn(tmp, i)
    
    w.export_to( FILENAME )
    
    w2 = Wisard(FILENAME)
    
    for i in xrange(CLASSES):
        for _ in xrange(TEST_ITERS):
            tmp = noisify(patterns[i], tmp, noise)
            predicted = w2.read_bleaching(tmp)
            #predicted = 1
            
            if predicted == i:
                hits += 1
            else:
                misses += 1
    
    print "Noise = %d, Accuracy = %f" % (int(noise*100), hits/float(hits+misses))

for noise in xrange(0, 101, 5):
    test( noise / 100.0 )

