#pragma once
//=================================================================================================
//          Modified work is Copyright (C) 2017 Alain Lanthier - All Rights Reserved  
//                      Original source is GALGO 2.0 (see github)               
//=================================================================================================
//
//  Randomize<NBIT>::uniform_uint64()   : random uint64 on [0,pow2n(NBIT)]
//  T uniform<T>(T min, T max)          : random T on [min, max)
//
//
#ifndef _AL_CHESS_GA_RANDOMIZE_H
#define _AL_CHESS_GA_RANDOMIZE_H

#include "random.hpp"
#include <cassert>

namespace galgo 
{
    std::random_device  rnd_dev;                    // uniformly - distributed integer random number generator
    std::mt19937_64     rnd_generator(rnd_dev());   // Mersenne Twister 19937 pseudo-random number generator [0, pow(2,64))
    std::uniform_real_distribution<double> uniform_double(0, 1);    // uniform random probability in range [0,1)          

    template <typename T>
    inline T uniform(T min, T max)
    {   
        assert(max >= min);
        return (T)(min + uniform_double(rnd_generator) * (max - min));
    }

    template <int NBIT>
    class Randomize
    {
        static_assert(NBIT > 0 && NBIT <= 64, "NBIT not in [1,64]");

    public:
        static constexpr uint64_t MAXVAL = chess::ga::pow2n(NBIT);
        static uint64_t uniform_uint64() // random uint64 on [0,MAXVAL]
        {
            static std::uniform_int_distribution<uint64_t> udistrib(0,MAXVAL);
            return udistrib(rnd_generator);
        }
    };
}

#endif
