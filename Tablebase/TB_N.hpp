#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_N_HPP
#define _AL_CHESS_TABLEBASE_TB_N_HPP

namespace chess
{
    // TB_N
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE>
    class TB_N : public Tablebase<PieceID, _BoardSize, NPIECE>
    {
    public:
        TB_N(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, NPIECE>(v, c) {}
        ~TB_N() {}
    };
};
#endif

