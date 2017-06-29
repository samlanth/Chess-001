#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_1v1
// TBHandler_1v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_1v1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_1v1_HPP

namespace chess
{
    // Tablebase_1v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_1v1 : public Tablebase<PieceID, _BoardSize, 2>
    {
    public:
        Tablebase_1v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c) {}
        ~Tablebase_1v1() {}
    };

    // TBHandler_1v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_1v1 : public TBHandler_2<PieceID, _BoardSize>
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TBHandler_1v1(const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) : TBHandler_2<PieceID, _BoardSize>(ps, TB_TYPE::tb_1v1, option) {}
        ~TBHandler_1v1() {}
    };

};
#endif

