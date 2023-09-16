#ifndef INCLUDE_WUP_COMMON_GENERATE_HPP
#define INCLUDE_WUP_COMMON_GENERATE_HPP
 
namespace wup { namespace generate {

inline void
randomKernels(size_t const numKernels, 
                    wup::Bundle<double> const & dimRanges, 
                    wup::Bundle<double> & kernels)
{
    kernels.reshape(numKernels, dimRanges.cols());
    for (size_t i=0;i!=numKernels;++i)
        for (size_t j=0;j!=dimRanges.cols();++j)
            kernels(i,j) = dimRanges(0,j) + (rand()/double(RAND_MAX)) * (dimRanges(1,j)-dimRanges(0,j));
}

inline void
bestCandidateKernels(size_t const numKernels, 
                           size_t const numCandidates, 
                           wup::Bundle<double> const & dimRanges, 
                           wup::Bundle<double> & kernels)
{
    kernels.reshape(numKernels, dimRanges.cols());

    if (numKernels == 0 || numCandidates == 0)
        return;
    
    // Create the first kernel
    for (size_t j=0;j!=dimRanges.cols();++j)
        kernels(0,j) = dimRanges(0,j) + (rand()/double(RAND_MAX)) * (dimRanges(1,j)-dimRanges(0,j));

    // Space to group random candidates
    wup::Bundle<double> candidates;

    // Create the remaining kernels
    for (int i=1;i!=numKernels;++i)
    {
        // Create candidates
        randomKernels(numCandidates, dimRanges, candidates);

        // Select the best candidate
        double fartestCandidateDistance = -1.0;
        int fartestCandidateId = 0;

        for (int candidateId=0;candidateId!=numCandidates;++candidateId)
        {
            double closestKernelDistance = -1.0;

            // Calculate the distance to the nearest kernel
            for (int ii=0;ii!=i;++ii)
            {
                double distance = wup::math::distance(&kernels(ii,0), &candidates(candidateId,0), dimRanges.cols());
                if (closestKernelDistance < 0 || distance < closestKernelDistance)
                    closestKernelDistance = distance;
            }

            // If this candidate is the fartest, save it as the best candidate we found so far
            if (fartestCandidateDistance < 0 || closestKernelDistance > fartestCandidateDistance)
            {
                fartestCandidateDistance = closestKernelDistance;
                fartestCandidateId = candidateId;
            }
        }

        std::copy(&candidates(fartestCandidateId,0), &candidates(fartestCandidateId,0)+candidates.cols(), &kernels(i,0));
    }

}

} }

#endif /* INCLUDE_WUP_COMMON_GENERATE_HPP */

