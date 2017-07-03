#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_4_HPP
#define _AL_CHESS_TABLEBASE_TB_4_HPP

namespace chess
{
    // TB_4
    template <typename PieceID, typename uint8_t _BoardSize>
    class TB_4 : public Tablebase<PieceID, _BoardSize, 4>
    {
    public:
        TB_4(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 4>(v, c) {}
        ~TB_4() {}
    };
};
#endif

