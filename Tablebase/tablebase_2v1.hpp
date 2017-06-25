#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseHandler_2v1
// Tablebase_2v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_2v1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_2v1_HPP


namespace chess
{
    // Tablebase_2v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_2v1 : public Tablebase<PieceID, _BoardSize, 3>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;
        friend class TablebaseBaseHandler_3<PieceID, _BoardSize>;

    public:
        Tablebase_2v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 3>(v, c) {}
        ~Tablebase_2v1() {}

        bool load() override            { return load_tb(); }
        bool save() const override      { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
    };

    // TablebaseHandler_2v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_2v1 : public TablebaseBaseHandler_3<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TablebaseHandler_2v1(const PieceSet<PieceID, _BoardSize>& ps) : TablebaseBaseHandler_3<PieceID, _BoardSize>(ps, TB_TYPE::tb_2v1)
        {
            Tablebase<PieceID, _BoardSize, 3>* tb_w = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 3>* tb_b = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            if (tb_w == nullptr)
            {
                _tb_W = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::W);
                TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
            }
            else
            {
                _tb_W = (Tablebase_2v1<PieceID, _BoardSize> *)tb_w;
            }

            if (tb_b == nullptr)
            {
                _tb_B = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::B);
                TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
            }
            else
            {
                _tb_B = (Tablebase_2v1<PieceID, _BoardSize> *)tb_b;
            }

            std::vector<TBH<PieceID, _BoardSize>*> tbh_child = TablebaseManager<PieceID, _BoardSize>::instance()->make_all_child_TBH(ps);
            for (size_t i = 0; i < tbh_child.size(); i++)
            {
                _tbh_children.push_back(tbh_child[i]);
            }
        }

        ~TablebaseHandler_2v1()
        {
            // only owner of handler
            for (size_t i = 0; i < _tbh_children.size(); i++)
            {
                delete _tbh_children[i];
            }
        }

        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;

        Tablebase_2v1<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_2v1<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:      
        bool find_tbh_children(PieceSet<PieceID, _BoardSize>  ps, PieceColor color_to_play, size_t& ret_index) const;
        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const override;

        void print_pos_with_dtc_score(uint8_t value_dtc, ExactScore value_sc) const;
        void print() const;

        // Bases TB
        Tablebase_2v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_2v1<PieceID, _BoardSize>* _tb_B;

        std::vector<TBH<PieceID, _BoardSize>*> _tbh_children;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TablebaseHandler_2v1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;

        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->save();
        }
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TablebaseHandler_2v1<PieceID, _BoardSize>::load()
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->load();
        }

        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TablebaseHandler_2v1<PieceID, _BoardSize>::build(char verbose)
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->build(verbose);
        }

        if (verbose) 
        {
            std::string str_pieces;
            for (auto& v : _piecesID)str_pieces += _Piece::to_str(v);
            std::cout << "TablebaseHandler_2v1: " << str_pieces << std::endl; 
        }
        
        return build_base(_tb_W, _tb_B, verbose);        
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TablebaseHandler_2v1<PieceID, _BoardSize>::find_tbh_children(PieceSet<PieceID, _BoardSize>  ps, PieceColor color_to_play, size_t& ret_index) const
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
    inline bool TablebaseHandler_2v1<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const
    {
        size_t ret_child_index;
        std::vector<std::pair<PieceID, uint8_t>> empty_set;
        PieceID ret_id; uint16_t ret_count;
        uint16_t ret_child_sq0; uint16_t ret_child_sq1;
        uint16_t rank_ret_id;

        if (_pieceSet.find_child_index(color, pos, ret_child_index))
        {
            std::vector<std::pair<PieceID, uint8_t>> v = _pieceSet.children(color, ret_child_index);
            PieceSet<PieceID, _BoardSize> child_set({   (color == PieceColor::W) ? v : _pieceSet.wset(),
                                                        (color == PieceColor::W) ? _pieceSet.bset() : v } );

            size_t n = 0;
            for (size_t i = 0; i < 2; i++) // child of 2v1 is 2 pieces or less
            {
                rank_ret_id = child_set.find_rank_index(i, ret_id, ret_count);
                if (ret_count > 0)
                {
                    if (n == 0)         ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(ret_id)->get_name(), _Piece::get(ret_id)->get_color());
                    else if (n == 1)    ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(ret_id)->get_name(), _Piece::get(ret_id)->get_color());
                    n++;
                }
            }
            size_t ret_idx;
            if (find_tbh_children(child_set, color, ret_idx))
            {
                if (color == PieceColor::W)
                {
                    if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_1v1)
                        ret_sc = ((TablebaseHandler_1v1<PieceID, _BoardSize>*)_tbh_children[ret_idx])->tb_W()->score(ret_child_sq0, ret_child_sq1);
                    else if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_Xv0)
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
                    if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_1v1)
                        ret_sc = ((TablebaseHandler_1v1<PieceID, _BoardSize>*)_tbh_children[ret_idx])->tb_B()->score(ret_child_sq0, ret_child_sq1);
                    else if (_tbh_children[ret_idx]->tb_type() == TB_TYPE::tb_Xv0)
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

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TablebaseHandler_2v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TablebaseHandler_2v1<PieceID, _BoardSize>::print_pos_with_dtc_score(uint8_t value_dtc, ExactScore value_sc) const
    {
        std::vector<uint64_t> v_dtc = _tb_W->get_index_dtc(value_dtc, value_sc);
        if (v_dtc.size() == 0) return;

        uint16_t sq0, sq1, sq2;
        _tb_W->square_at_index(v_dtc[0], sq0, sq1, sq2);
        if (!((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)))
        {
            ExactScore sc = _tb_W->score(sq0, sq1, sq2);
            if (sc != ExactScore::UNKNOWN)
            {
                Board<PieceID, _BoardSize> board;
                board.clear();
                board.set_color(chess::PieceColor::W);
                board.set_pieceid_at(_tb_W->pieceID(0), sq0);
                board.set_pieceid_at(_tb_W->pieceID(1), sq1);
                board.set_pieceid_at(_tb_W->pieceID(2), sq2);

                std::cout << board.to_str() << std::endl;
                std::cout << (int)_tb_W->dtc(sq0, sq1, sq2) << std::endl;

                std::vector<Move<PieceID>> m = board.generate_moves();
                if (!board.is_final(m))
                {
                    for (size_t k = 0; k < m.size(); k++)
                    {
                        board.apply_move(m[k]);

                        if (board.cnt_all_piece() == _tb_W->_NPIECE)
                        {
                            std::vector<uint16_t> v_sq;
                            std::vector<uint8_t>  v_id = board.get_piecesID();        // sorted
                            for (auto& id : v_id)
                            {
                                v_sq.push_back(board.get_square_ofpiece(
                                    Piece<PieceID, _BoardSize>::get(id)->get_name(),
                                    Piece<PieceID, _BoardSize>::get(id)->get_color()));
                            }
                            // ordering repeat...
                            {
                                Board<PieceID, _BoardSize> board;
                                board.clear();
                                board.set_color(chess::PieceColor::W);
                                board.set_pieceid_at(v_id[0], v_sq[0]);
                                board.set_pieceid_at(v_id[1], v_sq[1]);
                                board.set_pieceid_at(v_id[2], v_sq[2]);

                                std::cout << board.to_str() << std::endl;
                                std::cout << "dtc B child: (" << ExactScore_to_string(_tb_B->score(v_sq[0], v_sq[1], v_sq[2])) << ") " << (int)_tb_B->dtc(v_sq[0], v_sq[1], v_sq[2]) << std::endl;
                            }
                        }

                        board.undo_move();
                    }
                }

            }
        }
    }
};
#endif

