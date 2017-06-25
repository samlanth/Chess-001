#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_1v1
// TablebaseHandler_1v1
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
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;

    public:
        Tablebase_1v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c) {}
        ~Tablebase_1v1() {}

        bool load() override        { return load_tb(); }
        bool save() const override  { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
    };

    // TablebaseHandler_1v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_1v1 : public TablebaseBaseHandler_2<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        TablebaseHandler_1v1(const PieceSet<PieceID, _BoardSize>& ps) : TablebaseBaseHandler_2<PieceID, _BoardSize>(ps, TB_TYPE::tb_1v1) {}
        ~TablebaseHandler_1v1() 
        {
        }

    protected:
        bool find_tbh_children(PieceSet<PieceID, _BoardSize>  ps, PieceColor color_to_play, size_t& ret_index) const;
        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const override;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TablebaseHandler_1v1<PieceID, _BoardSize>::find_tbh_children(PieceSet<PieceID, _BoardSize>  ps, PieceColor color_to_play, size_t& ret_index) const
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            if (_tbh_children[i]->name(color_to_play) == ps.name(color_to_play))
            {
                ret_index = i;
                return true;
            }
        }
        return  false;
    }

    // find_score_children_tb
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TablebaseHandler_1v1<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const
    {
        size_t ret_child_index;
        std::vector<std::pair<PieceID, uint8_t>> empty_set;
        PieceID ret_id; uint16_t ret_count;
        uint16_t ret_child_sq0;
        uint16_t rank_ret_id;

        if (_pieceSet.find_child_index(color, pos, ret_child_index))
        {
            std::vector<std::pair<PieceID, uint8_t>> v = _pieceSet.children(color, ret_child_index);
            PieceSet<PieceID, _BoardSize> child_set({ (color == PieceColor::W) ? v : _pieceSet.wset(),
                (color == PieceColor::W) ? _pieceSet.bset() : v });

            size_t n = 0;
            for (size_t i = 0; i < 1; i++) // child is 1 pieces or less
            {
                rank_ret_id = child_set.find_rank_index(i, ret_id, ret_count);
                if (ret_count > 0)
                {
                    if (n == 0) ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(ret_id)->get_name(), _Piece::get(ret_id)->get_color());
                    n++;
                }
            }
            size_t ret_idx;
            if (find_tbh_children(child_set, color, ret_idx))
            {
                if (color == PieceColor::W)
                {
                    if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_Xv0)
                        ret_sc = ((TablebaseHandler_Xv0<PieceID, _BoardSize>*)_tbh_children[ret_idx])->tb_W()->score(ret_child_sq0);
                    else if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_0vX)
                        ret_sc = ((TablebaseHandler_0vX<PieceID, _BoardSize>*)_tbh_children[ret_idx])->tb_W()->score(ret_child_sq0);
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_Xv0)
                        ret_sc = ((TablebaseHandler_Xv0<PieceID, _BoardSize>*)_tbh_children[ret_idx])->tb_B()->score(ret_child_sq0);
                    else if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_0vX)
                        ret_sc = ((TablebaseHandler_0vX<PieceID, _BoardSize>*)_tbh_children[ret_idx])->tb_B()->score(ret_child_sq0);
                    else
                    {
                        return false;
                    }
                }
            }
            return true;
        }
        assert(false);
        return false;
    }
};
#endif

