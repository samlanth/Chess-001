#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_Xv0    :   any W piece versus 0 black pieces
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_Xv0_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_Xv0_HPP

namespace chess
{
    // Tablebase_Xv0
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_Xv0 : public Tablebase<PieceID, _BoardSize, 1>
    {
    public:
        Tablebase_Xv0(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c) {}
        ~Tablebase_Xv0() {}
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_Xv0 : public TBHandler_1<PieceID, _BoardSize>
    {
    public:
        TBHandler_Xv0(const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) : TBHandler_1<PieceID, _BoardSize>(ps, TB_TYPE::tb_Xv0, option) {}
        ~TBHandler_Xv0() {}
    };
};
#endif

