import aoflagger
import numpy

def flag(input):

  # Values below can be tweaked
  flag_polarizations = input.polarizations()
  flag_representations = [ aoflagger.ComplexRepresentation.AmplitudePart ]

  iteration_count = 3
  threshold_factor_step = 2.0
  base_threshold = 1.0
  
  # Use above values to calculate thresholds in iteration
  r = range((iteration_count-1), 0, -1)
  threshold_factors = numpy.power(threshold_factor_step, r) * base_threshold

  inpPolarizations = input.polarizations()
  output = input
  output.clear_mask()
  
  for polarization in flag_polarizations:
      
    data = input.convert_to_polarization(polarization)

    for representation in flag_representations:

      data = data.convert_to_complex(representation)
      
      for threshold_factor in threshold_factors:
      
        print 'Flagging polarization ' + str(polarization) + ' (' + str(representation) + ', ' + str(threshold_factor) + ')'

        aoflagger.sumthreshold(data, threshold_factor, True, True)
        aoflagger.high_pass_filter(data, 21, 31, 1.6, 2.2);

      aoflagger.sumthreshold(data, base_threshold, True, True)

    if polarization in inpPolarizations:
      output.set_polarization_data(polarization, data)
    else:
      output.join_mask(polarization, data)

aoflagger.set_flag_function(flag)

print 'strategy.py parsed'
