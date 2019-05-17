import aoflagger as aof
import numpy
import sys

print("Flagging with AOFlagger version " + aof.AOFlagger.get_version_string())

nch = 256
ntimes = 1000
count=50       # number of trials in the false-positives test

aoflagger = aof.AOFlagger()

# Load strategy from disk (alternatively use 'make_strategy' to use a default one)
strategy = aoflagger.load_strategy("example-strategy.rfis")

data = aoflagger.make_image_set(ntimes, nch, 8)

print("Number of times: " + str(data.width()))
print("Number of channels: " + str(data.height()))

# When flagging multiple baselines, iterate over the baselines and
# call the following code for each baseline
# (to use multithreading, make sure to create an imageset for each
# thread)

# Make eight images: real and imaginary for 4 pol

for imgindex in range(8):
    # Initialize data
    values = numpy.zeros([ntimes, nch])
    data.set_image_buffer(imgindex, values)
    
flags = aoflagger.run(strategy, data)
flagvalues = flags.get_buffer()
flagcount = sum(sum(flagvalues))
print("Percentage flags on zero data: " + str(flagcount * 100.0 / (nch*ntimes)) + "%")

ratiosum=0.0
ratiosumsq=0.0

# This is a simple example to calculate the false-positive ratio
# on Gaussian data.
for repeat in range(count):
    for imgindex in range(8):
        # Initialize data with random numbers
        values = numpy.random.normal(0, 1, [ntimes, nch])
        data.set_image_buffer(imgindex, values)
        
    flags = aoflagger.run(strategy, data)
    flagvalues = flags.get_buffer()
    ratio = float(sum(sum(flagvalues))) / (nch*ntimes)
    ratiosum += ratio
    ratiosumsq += ratio*ratio
    sys.stdout.write('.')
    sys.stdout.flush()

print('')
    
print("Percentage flags (false-positive rate) on Gaussian data: " +
      str(ratiosum * 100.0 / count) + "% +/- " +
      str(numpy.sqrt((ratiosumsq/count - ratiosum*ratiosum / (count*count))) * 100.0)
     )
