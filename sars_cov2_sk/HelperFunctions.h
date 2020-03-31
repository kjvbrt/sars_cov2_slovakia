
#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

namespace sars_cov2_sk	{
    // Uniform distribution from (0,1) interval. 0 and 1 never happens.
    double RandomUniform();

    unsigned int RandomPoisson(float mean_value);

    float RandomGauss(float mean, float sigma);
}

#endif