#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_2v0
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_2v0_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_2v0_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_2v0 : public Tablebase<PieceID, _BoardSize, 2>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        Tablebase_2v0(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c)
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
    bool Tablebase_2v0<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 2) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[1])->get_name(), PieceColor::W)))
        {
            return true;
        }
        return false;
    }

    // TablebaseHandler_2v0
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_2v0 : public TablebaseBaseHandler_2<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        TablebaseHandler_2v0(std::vector<PieceID>& v) : TablebaseBaseHandler_2<PieceID, _BoardSize>(v, TB_TYPE_2::tb_2v0)
        {
        }
        ~TablebaseHandler_2v0()
        {
        }

        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const override
        {
            uint16_t ret_child_sq0;
            Tablebase_1v0<PieceID, _BoardSize>* tb_1v0 = locate_children_1v0(pos, color, ret_child_sq0);
            if (tb_1v0 != nullptr)
            {
                ret_sc = tb_1v0->score(ret_child_sq0);
                return true;
            }
            return false;
        }
        Tablebase_1v0<PieceID, _BoardSize>* locate_children_1v0(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0) const;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v0<PieceID, _BoardSize>* TablebaseHandler_2v0<PieceID, _BoardSize>::locate_children_1v0(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0) const
    {
        if ((color == PieceColor::W) && (tbh_0_1v0()->tb_W()->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_1v0[0])->get_name(), PieceColor::W);
            return (Tablebase_1v0<PieceID, _BoardSize>*)tbh_0_1v0()->tb_W();
        }
        if ((color == PieceColor::B) && (tbh_0_1v0()->tb_B()->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_1v0[0])->get_name(), PieceColor::W);
            return (Tablebase_1v0<PieceID, _BoardSize>*)tbh_0_1v0()->tb_B();
        }
        return nullptr;
    }

};
#endif

