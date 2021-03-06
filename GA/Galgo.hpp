#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                      Original source is GALGO 2.0 (see github)         
//=================================================================================================
//
//
#ifndef _AL_CHESS_GA_GALGO_H
#define _AL_CHESS_GA_GALGO_H

#include <algorithm>
#include <bitset>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <climits>
#include <cmath>
#include <cstring>

namespace galgo
{
    // forward declarations
    template <typename T> class BaseParameter;
    template <typename T, int N = 16> class Parameter;
    template <typename T, int PARAM_NBIT> class GeneticAlgorithm;
    template <typename T, int PARAM_NBIT> class Population;
    template <typename T, int PARAM_NBIT> class Chromosome;

    // convenient typedefs
    template <typename T, int PARAM_NBIT> using CHR = std::shared_ptr<Chromosome<T, PARAM_NBIT>>;
    template <typename T> using PAR = std::unique_ptr<BaseParameter<T>>;
    template <typename T, int...N>using TUP = std::tuple<const Parameter<T, N>&...>;
}
#ifdef _OPENMP 
#include <omp.h>
    // getting maximum number of threads available
    static const int MAX_THREADS = omp_get_max_threads();
#endif

#include "ga/Randomize.hpp"
#include "ga/Converter.hpp"
#include "ga/Parameter.hpp"
#include "ga/Evolution.hpp"
#include "ga/Chromosome.hpp"
#include "ga/Population.hpp"
#include "ga/GeneticAlgorithm.hpp"
#include "random.hpp"

#endif

