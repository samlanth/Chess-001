#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TB_1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_1_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class TB_1 : public Tablebase<PieceID, _BoardSize, 1>
    {
    public:
        TB_1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c) {}
        ~TB_1() {}
    };

};
#endif
