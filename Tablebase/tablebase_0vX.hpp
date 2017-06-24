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
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_0vX : public Tablebase<PieceID, _BoardSize, 1>
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        Tablebase_0vX(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c)
        {
        }

        bool load() override 
        { 
            _is_build = read_tb(); 
            if (_is_build) TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);
            return _is_build; 
        }

        bool save() const override { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
        bool isPiecesMatch(const _Board& pos) override;        
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_0vX<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if (    (pos.cnt_all_piece() >= 1) &&
                (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }

    // TablebaseHandler_0vX
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_0vX : public TablebaseBaseHandler_1<PieceID, _BoardSize>
    {
    public:
        TablebaseHandler_0vX(std::vector<PieceID>& v) : TablebaseBaseHandler_1<PieceID, _BoardSize>(v, TB_TYPE::tb_0vX)
        {
        }
        ~TablebaseHandler_0vX()
        {
        }
    };
};
#endif

