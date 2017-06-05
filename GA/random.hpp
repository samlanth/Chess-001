#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// 
#ifndef _AL_CHESS_GA_RANDOM_H
#define _AL_CHESS_GA_RANDOM_H

namespace chess
{
    namespace ga
    {
        constexpr uint64_t pow2n(uint8_t n) { return (n>0) ? 2 * pow2n(n - 1) : 1; }
    }
}
#endif
