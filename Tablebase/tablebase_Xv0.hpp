#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_Xv0    :   any W piece - 0 black pieces
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
        using _Board = Board<PieceID, _BoardSize>;

    public:
        Tablebase_Xv0(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c) {}
        ~Tablebase_Xv0() {}

        bool load() override        { return load_tb(); }
        bool save() const override  { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_Xv0 : public TablebaseBaseHandler_1<PieceID, _BoardSize>
    {
    public:
        TablebaseHandler_Xv0(const PieceSet<PieceID, _BoardSize>& ps) : TablebaseBaseHandler_1<PieceID, _BoardSize>(ps, TB_TYPE::tb_Xv0) {}
        ~TablebaseHandler_Xv0() {}
    };
};
#endif
