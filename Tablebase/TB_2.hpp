#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TB_2
// TBH_1v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_2_HPP
#define _AL_CHESS_TABLEBASE_TB_2_HPP

namespace chess
{
    // TB_2
    template <typename PieceID, typename uint8_t _BoardSize>
    class TB_2 : public Tablebase<PieceID, _BoardSize, 2>
    {
    public:
        TB_2(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c) {}
        ~TB_2() {}
    };

    // TBH_1v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBH_1v1 : public TBH_2<PieceID, _BoardSize>
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TBH_1v1(const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) : TBH_2<PieceID, _BoardSize>(ps, TB_TYPE::tb_1v1, option) {}
        ~TBH_1v1() {}
    };

};
#endif

