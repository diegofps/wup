/*
 * kernelgens.hpp
 *
 *  Created on: Feb 18, 2015
 *      Author: diegofps
 */

#ifndef INCLUDE_WUP_MODELS_KERNELGENS_HPP
#define INCLUDE_WUP_MODELS_KERNELGENS_HPP

#include <cmath>

namespace wup {

namespace kernelgens {

    inline double *
    __createRandomKernel(const int dims)
    {
        double * kernel = new double[dims];
        for (int j=0;j<dims;++j)
            kernel[j] = rand() / double(RAND_MAX) * 2.0 - 1.0;
        return kernel;
    }

    inline void
    createRandomKernels(const int dims, int &numKernels, double** &kernels)
    {
        kernels = new double*[numKernels];
        for (int i=0;i<numKernels;++i) {
            double * v = new double[dims];
            for (int j=0;j<dims;++j)
                v[j] = rand() / double(RAND_MAX) * 2.0 - 1.0;
            kernels[i] = v;
        }
    }

    inline void
    importKernels(wup::sbreader<double> &reader, int &dims, int &numKernels, double** &kernels)
    {
        dims = reader.get();
        numKernels = reader.get();
        kernels = new double*[numKernels];
        for (int i=0;i<numKernels;++i) {
            double * v = new double[dims];
            for (int j=0;j<dims;++j)
                v[j] = reader.get();
            kernels[i] = v;
        }
    }

    inline void
    exportKernels(wup::sbwriter<double> & writer, const int dims, int numKernels, double** &kernels)
    {
        writer.put(dims);
        writer.put(numKernels);
        for (int i=0;i<numKernels;++i) {
            auto v = kernels[i];
            for (int j=0;j<dims;++j)
                writer.put(v[j]);
        }
    }

    inline double *
    __createZeroKernel(const int dims)
    {
        double * kernel = new double[dims];

        for (int j=0;j<dims;++j)
            kernel[j] = 0.0;

        return kernel;
    }

    inline double *
    __createBridsonsKernelNeighbor(const int dims, const double * const kernel,
            const double r)
    {
        double * const candidate = new double[dims];
        double sumSquares;

        while (true) {
            sumSquares = 0.0;

            for (int j=0;j<dims;++j) {
                candidate[j] = rand() / double(RAND_MAX) * 2.0 - 1.0;
                sumSquares += candidate[j] * candidate[j];
            }

            if (sumSquares > 0.0001) break;
        }

        sumSquares = sqrt(sumSquares);
        const double newLength = r * (1.0 + rand() / double(RAND_MAX));

        for (int j=0;j<dims;++j)
            candidate[j] = kernel[j] + candidate[j] / sumSquares * newLength;

        return candidate;
    }

    inline bool
    isValidBridsonsKernel(const int dims, const double * const candidate,
            const std::vector<double*> &actives,
            const std::vector<double*> &inactives,
            const double rSquared)
    {
        for (int i=0;i<dims;++i)
            if (candidate[i] < -1.0 || candidate[i] > +1.0) {
    //				debug("Invalid, outside of box");
                return false;
            }

        for (const double * kernel : actives) {
            if (wup::sdistance(candidate, kernel, dims) < rSquared) {
    //				debug("Invalid, distance to active is ", euclidianDistance(candidate, kernel), ", min is ", rSquared);
                return false;
            } else {
                //debug("Valid against active kernel, distance to it is ", euclidianDistance(candidate, kernel), ", min is ", rSquared);
            }
        }

        for (const double * kernel : inactives) {
            if (wup::sdistance(candidate, kernel, dims) < rSquared) {
    //				debug("Invalid, distance to inactive is ", euclidianDistance(candidate, kernel), ", min is ", rSquared);
                return false;
            } else {
                //debug("Valid against inactive kernel, distance to it is ", euclidianDistance(candidate, kernel), ", min is ", rSquared);
            }
        }

    //    	std::cout << YELLOW << "Good sample: ";
    //    	for (int i=0;i<dims;++i) std::cout << candidate[i] << " ";
    //    	std::cout << NORMAL << std::endl;

        return true;
    }

    inline void
    createBridsonsKernels(const int dims, const double r, const int tries,
            const uint maxKernels, int &numKernels, double ** &kernels)
    {
        debug("Applying bridson's algorithm");
    //	debug("r was ", r);
    //	r = sqrt(dims) * pow(1.0/numKernels, 1.0/dims) * 0.77;
    //	debug("r became ", r);

        std::vector<double*> actives;
        std::vector<double*> inactives;
        const double rSquared = r * r;
        int t;

        debug( "r is ", r );
        debug("rSquared is ", rSquared);
        actives.push_back(__createZeroKernel(dims));

        while (!actives.empty()) {
            const int selectedId = rand() % actives.size();
            double * const selected = actives[selectedId];

    //    		std::cout << YELLOW << "Pivot is: ";
    //			for (int i=0;i<_dims;++i) std::cout << selected[i] << " ";
    //			std::cout << NORMAL << std::endl;

            for (t=0;t<tries;++t) {
                double *candidate = __createBridsonsKernelNeighbor(
                        dims, selected, r);

                if (isValidBridsonsKernel(dims, candidate, actives, inactives, rSquared)) {
                    actives.push_back(candidate);
    //    				debug("Found candidate, adding to actives. #Actives=", actives.size());
                    break;
                }
            }

            if (t == tries) {
                inactives.push_back(selected);
                actives.erase(actives.begin() + selectedId);
    //				debug("Reached max tries, removing from actives. #Actives=", actives.size(), ". #Inactives=", inactives.size());
            } else if (actives.size() + inactives.size() > maxKernels) {
                error("Stopping, space has too many kernels");
            }

            //warn("Press enter to continue...");
            //getchar();
        }

        t = 0;
        numKernels = inactives.size();
        kernels = new double*[inactives.size()];
        for (auto &kernel : inactives)
            kernels[t++] = kernel;
        print("Created ", inactives.size(), " kernels");
    }

    inline void
    createBestCandidateKernels(const int dims, const int samples,
            int &numKernels, double ** &kernels)
    {
        debug("Applying Mitchell's algorithm");

        int minI, maxI;
        double minD, maxD, distance;
        double **_tmpKernels;

        _tmpKernels = new double*[samples];
        kernels = new double*[numKernels];

        for (int s=0;s<samples;++s)
            _tmpKernels[s] = new double[dims];

        for (int i=0;i<numKernels;++i) {
            double * v = new double[dims];

            if (i == 0) {
                for (int j=0;j<dims;++j) {
                    v[j] = rand() / double(RAND_MAX) * 2.0 - 1.0;
                }
            } else {
                maxI = -1;
                for (int s=0;s<samples;++s) {
                    for (int j=0;j<dims;++j) {
                        _tmpKernels[s][j] =
                                rand() / double(RAND_MAX) * 2.0 - 1.0;
                    }

                    minI = -1;
                    for (int i2=0;i2<i;++i2) {
                        distance = sdistance(_tmpKernels[s], kernels[i2], dims);

                        if (minI == -1 || distance < minD) {
                            minI = i2;
                            minD = distance;
                        }
                    }

                    if (maxI == -1 || maxD < minD) {
                        maxI = s;
                        maxD = minD;
                    }
                }

                for (int j=0;j<dims;++j) {
                    v[j] = _tmpKernels[maxI][j];
                }
            }

            //debug("Kernel ", i, " added, ", v);
            kernels[i] = v;
        }

        for (int s=0;s<samples;++s)
            delete [] _tmpKernels[s];

        delete [] _tmpKernels;
    }

    inline void
    __createSpacedKernels(const int dims, const int dim,
            const int * const dimBreaks, int &kernelId,
            double *& tmpKernel, double ** &kernels)
    {
        if (dim == dims) {
    //        print("Creating kernel ", kernelId);

            double * kernel = new double[dims];
            memcpy(kernel, tmpKernel, sizeof(double) * dims);
            kernels[kernelId++] = kernel;

    //        for (int i=0;i<dims;++i) std::cout << kernel[i] << " ";
    //        print();
        } else {
            for (int i=0;i<dimBreaks[dim];++i) {
                const double noise = rand() / double(RAND_MAX) * 0.01 - 0.005;
                tmpKernel[dim] = 2.0 * (i+1) / (dimBreaks[dim]+1) - 1.0 + noise;
                __createSpacedKernels(dims, dim+1, dimBreaks, kernelId,
                        tmpKernel, kernels);
            }
        }
    }

    inline void
    createSpacedKernels(const int dims, int &numKernels, double ** &kernels)
    {
        debug("Applying Nearest Grid Algorithm");

        int *dimBreaks = new int[dims]();
        double root = pow(numKernels * 2, 1.0 / dims);

        dimBreaks[0] = round(root);
        if (dimBreaks[0] < 1) dimBreaks[0] = 1;
        numKernels = dimBreaks[0];

        for (int i=1;i<dims;++i) {
            dimBreaks[i] = round(root * (i+1)) - round(root * i);
            if (dimBreaks[i] < 1) dimBreaks[i] = 1;
            numKernels *= dimBreaks[i];
        }

        int kernelId = 0;
        kernels = new double*[numKernels];
        double *tmpKernel = new double[dims];
        __createSpacedKernels(dims, 0, dimBreaks, kernelId, tmpKernel, kernels);

        debug("Linspaced created ", numKernels, " kernels");
        delete [] dimBreaks;
    }

    inline void
    createSpacedKernels2(const int dims, int &numKernels, double ** &kernels)
    {
        numKernels = pow(2, ceil(log2(numKernels)));
        int *dimBreaks = new int[dims]();
        double root = (log2(numKernels) / dims);

        int sum = 0;
        dimBreaks[0] = floor(root);
        for (int i=1;i<dims;++i) {
            dimBreaks[i] = floor((i+1)*root) - sum;
            sum += dimBreaks[i];
        }

        print("numKernels:", numKernels);
        print("root:", root);
        printn("dimbreaks:");
        for (int j=0;j<dims;++j)
            printn(dimBreaks[j], " ");
        print();

        int dimId = 0;
        int kernelId = 0;
        double *tmpKernel = new double[dims];
        int *indexes = new int[dims]();

        //print(1);
        kernels = new double*[numKernels];
        while(kernelId != numKernels) {
    //        print("dimId:", dimId);
    //        print("kernelId:", kernelId);
    //        for (int j=0;j<dims;++j)
    //            printn(indexes[j], " ");
    //        print();
    //        for (int j=0;j<dims;++j)
    //            printn(tmpKernel[j], " ");
    //        print();
    //        getchar();

            if (dimId == dims) {
                //print(2);
                printn("*");
                for (int j=0;j<dims;++j)
                    printn(tmpKernel[j], " ");
                print();

                kernels[kernelId++] = tmpKernel;
                tmpKernel = new double[dims];
                memcpy(tmpKernel, kernels[kernelId-1], sizeof(double) * dims);
                ++indexes[--dimId];
            } else {
                if (indexes[dimId] == dimBreaks[dimId]+1) {
                    //print(3);
                    indexes[dimId--] = 0;
                    if (dimId >= 0) {
                        //print(4);
                        ++indexes[dimId];
                    }
                } else {
                    //print(5);
                    tmpKernel[dimId] = 2.0 * (indexes[dimId]+1) / (dimBreaks[dimId]+2) - 1.0;
                    ++dimId;
                }
            }
        }

        debug("Linspaced created ", numKernels, " kernels");
        //delete [] tmpKernel;
        delete [] dimBreaks;
    }

    void
    createNGKernels(const int dims, const int * const coeffs, const int coeffIndex,
        double * const currentKernel, double ** const kernels, int &kernelIndex)
    {
        if (coeffIndex == dims) {
            double * const tmpKernel = new double[dims];
            memcpy(tmpKernel, currentKernel, sizeof(double) * dims);
            kernels[kernelIndex++] = tmpKernel;
        } else {
            for (int i=0;i<coeffs[coeffIndex];++i) {
                currentKernel[coeffIndex] = (i+1) / float(coeffs[coeffIndex]+1)
                        * 2.0 - 1.0;
                createNGKernels(dims, coeffs, coeffIndex+1,
                        currentKernel, kernels, kernelIndex);
            }
        }
    }

    inline void
    nearestGrid(const int dims, int &numKernels, double ** &kernels)
    {
        int n = numKernels;
        numKernels = 1;
        int *coeffs = new int[dims];
        for (int i=0;i<dims;++i) {
            int k = round(pow(n, 1.0/(dims-i)));
            numKernels *= k;
            coeffs[i] = k;
            n = n / k;
        }

        int kernelIndex = 0;
        kernels = new double*[numKernels];
        double *currentKernel = new double[dims];
        createNGKernels(dims, coeffs, 0, currentKernel, kernels, kernelIndex);

        info("Created ", kernelIndex, "/", numKernels, " kernels with GRID");

        delete [] coeffs;
        delete [] currentKernel;
    }

} /* kernelgens */

} /* wup */

#endif /* INCLUDE_WUP_MODELS_KERNELGENS_HPP */
