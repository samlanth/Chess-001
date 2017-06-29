#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_0vX
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_0vX_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_0vX_HPP

namespace chess
{
    // Tablebase_0vX
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_0vX : public Tablebase<PieceID, _BoardSize, 1>
    {
    public:
        Tablebase_0vX(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c) {}
        ~Tablebase_0vX() {}
    };

    // TBHandler_0vX
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_0vX : public TBHandler_1<PieceID, _BoardSize>
    {
    public:
        TBHandler_0vX(const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) : TBHandler_1<PieceID, _BoardSize>(ps, TB_TYPE::tb_0vX, option) {}
        ~TBHandler_0vX() {}
    };
};
#endif
