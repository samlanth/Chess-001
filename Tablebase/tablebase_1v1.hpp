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
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_1v1;

    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_1v1 : public Tablebase<PieceID, _BoardSize, 2>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;

    public:
        Tablebase_1v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c)
        {
            // set _children
            _child_piece_0.push_back(v[0]); _children.push_back(new Tablebase_1v0<PieceID, _BoardSize>(_child_piece_0, PieceColor::W));
            _child_piece_1.push_back(v[1]); _children.push_back(new Tablebase_1v0<PieceID, _BoardSize>(_child_piece_1, PieceColor::W));
            _child_piece_2.push_back(v[0]); _children.push_back(new Tablebase_1v0<PieceID, _BoardSize>(_child_piece_0, PieceColor::B));
            _child_piece_3.push_back(v[1]); _children.push_back(new Tablebase_1v0<PieceID, _BoardSize>(_child_piece_1, PieceColor::B));
        }

        bool load() override 
        {
            ((Tablebase_1v0<PieceID, _BoardSize>*)_children[0])->load();
            ((Tablebase_1v0<PieceID, _BoardSize>*)_children[1])->load();
            ((Tablebase_1v0<PieceID, _BoardSize>*)_children[2])->load();
            ((Tablebase_1v0<PieceID, _BoardSize>*)_children[3])->load();

            //...
            return false;
        }
        bool save()  override { return false; }
        bool build(char verbose = 0) override { return false; }

    protected:
        std::vector<PieceID> _child_piece_0;
        std::vector<PieceID> _child_piece_1;
        std::vector<PieceID> _child_piece_2;
        std::vector<PieceID> _child_piece_3;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_1v1
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    public:
        TablebaseHandler_1v1(std::vector<PieceID>& v)
        {
            _tb_W = new Tablebase_1v1<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_1v1<PieceID, _BoardSize>(v, PieceColor::B);
        }
        ~TablebaseHandler_1v1()
        {
            delete _tb_W;
            delete _tb_B;
        }

        bool build(char verbose = 0);
        Tablebase_1v1<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_1v1<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb);
        uint64_t set_marker(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo);
        ExactScore minmax_score(const uint64_t& idx_item, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo);
        Tablebase_1v0<PieceID, _BoardSize>* locate_children(Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo, uint16_t& ret_child_sq0) const;
        ExactScore best_score(PieceColor color_child, ExactScore current_score, ExactScore best_score) const;
        void print() const;

        Tablebase_1v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_1v1<PieceID, _BoardSize>* _tb_B;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::build(char verbose)
    {
        {
            Tablebase_1v0<PieceID, _BoardSize>* tb_0 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_W->_children[0];
            Tablebase_1v0<PieceID, _BoardSize>* tb_1 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_W->_children[1];
            Tablebase_1v0<PieceID, _BoardSize>* tb_2 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_W->_children[2];
            Tablebase_1v0<PieceID, _BoardSize>* tb_3 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_W->_children[3];
            if (tb_0->is_build() == false) tb_0->build();
            if (tb_1->is_build() == false) tb_1->build();
            if (tb_2->is_build() == false) tb_2->build();
            if (tb_3->is_build() == false) tb_3->build();

            assert(tb_0->is_build() == true);
            assert(tb_1->is_build() == true);
            assert(tb_2->is_build() == true);
            assert(tb_3->is_build() == true);
        }
        {
            Tablebase_1v0<PieceID, _BoardSize>* tb_0 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_B->_children[0];
            Tablebase_1v0<PieceID, _BoardSize>* tb_1 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_B->_children[1];
            Tablebase_1v0<PieceID, _BoardSize>* tb_2 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_B->_children[2];
            Tablebase_1v0<PieceID, _BoardSize>* tb_3 = (Tablebase_1v0<PieceID, _BoardSize>*)_tb_B->_children[3]; 
            if (tb_0->is_build() == false) tb_0->build();
            if (tb_1->is_build() == false) tb_1->build();
            if (tb_2->is_build() == false) tb_2->build();
            if (tb_3->is_build() == false) tb_3->build();

            assert(tb_0->is_build() == true);
            assert(tb_1->is_build() == true);
            assert(tb_2->is_build() == true);
            assert(tb_3->is_build() == true);
        }

        uint64_t n = 0;
        int iter = 0;
        
        set_mate_score(PieceColor::W, _tb_W);
        set_mate_score(PieceColor::B, _tb_B);

        do
        {
            iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

            n  = set_marker(PieceColor::W, _tb_W, _tb_B);
            n += set_marker(PieceColor::B, _tb_B, _tb_W);

            if (verbose) { std::cout << "set_marker positions:" << n << std::endl; }
            if (n == 0)
            {
                if (verbose) print();
                return true;
            }

            n =  process_marker(PieceColor::W, _tb_W, _tb_B);
            n += process_marker(PieceColor::B, _tb_B, _tb_W);

            if (verbose) { std::cout << "process_marker positions:" << n << std::endl; }
            if (verbose) print();

        } while (n > 0);

        _tb_W->_is_build = true;
        _tb_B->_is_build = true;
        _tb_W->set_unknown_to_draw();
        _tb_B->set_unknown_to_draw();

        assert(_tb_W->check_unknown() == false);
        assert(_tb_B->check_unknown() == false);
        return true;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v0<PieceID, _BoardSize>* TablebaseHandler_1v1<PieceID, _BoardSize>::locate_children(Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo, uint16_t& ret_child_sq0) const
    {
        // 0 W
        if (_Piece::get(tb->_child_piece_0[0])->get_color() == PieceColor::W)
            if (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_0[0])->get_name(), PieceColor::W))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_0[0])->get_name(), PieceColor::W);
                return (Tablebase_1v0<PieceID, _BoardSize>*)tb->_children[0];
            }
        // 1 W
        if (_Piece::get(tb->_child_piece_1[0])->get_color() == PieceColor::W)
            if (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1[0])->get_name(), PieceColor::W))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1[0])->get_name(), PieceColor::W);
                return (Tablebase_1v0<PieceID, _BoardSize>*)tb->_children[1];
            }
        // 0 B
        if (_Piece::get(tb->_child_piece_2[0])->get_color() == PieceColor::B)
            if (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_0[0])->get_name(), PieceColor::B))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_0[0])->get_name(), PieceColor::B);
                return (Tablebase_1v0<PieceID, _BoardSize>*)tb->_children[2];
            }
        // 1 B
        if (_Piece::get(tb->_child_piece_3[0])->get_color() == PieceColor::B)
            if (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1[0])->get_name(), PieceColor::B))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1[0])->get_name(), PieceColor::B);
                return (Tablebase_1v0<PieceID, _BoardSize>*)tb->_children[3];
            }

        return nullptr;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_1v1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb)
    {      
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<Move<PieceID>> m;

        for (uint16_t sq0 = 0; sq0 < tb->_dim1; sq0++)
        {
            for (uint16_t sq1 = 0; sq1 < tb->_dim1; sq1++)
            {
                if (sq0 != sq1)
                {
                    tb->_work_board->clear();
                    tb->_work_board->set_color(color_to_play);
                    tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
                    tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
                    m = tb->_work_board->generate_moves();
                    sc = tb->_work_board->final_score(m);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        tb->set_score(sq0, sq1, sc);
                        tb->set_marker(sq0, sq1, false);
                        n_changes++;
                    }
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_1v1<PieceID, _BoardSize>::set_marker(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        Move<PieceID> mv;
        uint16_t sq0, sq1;
        std::vector<Move<PieceID>> m_child;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1);
            if (sq0 == sq1) continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);

            // if one child has a score, will mark all other child without score
            m_child = tb->_work_board->generate_moves();
            bool one_child_has_score = false;
            bool one_mark = false;
            for (size_t k = 0; k < m_child.size(); k++)
            {
                mv = m_child[k];
                tb->_work_board->apply_move(mv);

                if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
                {
                    uint16_t child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                    uint16_t child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                    sc = tb_oppo->score(child_sq0, child_sq1);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        one_child_has_score = true;
                    }
                }
                else
                {
                    uint16_t child_sq0;
                    Tablebase_1v0<PieceID, _BoardSize>* tb_child = locate_children(tb, tb_oppo, child_sq0);
                    if (tb_child == nullptr)
                    {
                        assert(false);
                    }
                    else
                    {
                        sc = tb_child->score(child_sq0); 
                        if (sc != ExactScore::UNKNOWN)
                        {
                            one_child_has_score = true;
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                }

                tb->_work_board->undo_move();
                if (one_child_has_score)
                    break;
            }

            if (one_child_has_score)
            {
                for (size_t j = 0; j < m_child.size(); j++)
                {
                    mv = m_child[j];
                    tb->_work_board->apply_move(mv);
                    if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
                    {
                        uint16_t child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                        uint16_t child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                        sc = tb_oppo->score(child_sq0, child_sq1);
                        if (sc == ExactScore::UNKNOWN)
                        {
                            // mark child
                            if (tb_oppo->marker(child_sq0, child_sq1) == false)
                            {
                                tb_oppo->set_marker(child_sq0, child_sq1, true);
                                n_changes++;
                            } 
                            one_mark = true;
                        }
                    }
                    else
                    {
                        // child not in this tb
                    }
                    tb->_work_board->undo_move();
                }
            }

            if (one_mark)
            {
                // mark parent also
                sc = tb->score(sq0, sq1);
                if (sc == ExactScore::UNKNOWN)
                {
                    if (tb->marker(sq0, sq1) == false)
                    {
                        tb->set_marker(sq0, sq1, true);
                        n_changes++;
                    }
                }
            }
            
            if ((!one_mark) && (one_child_has_score))
            {
                // all known child score
                sc = tb->score(sq0, sq1);
                if (sc == ExactScore::UNKNOWN)
                {
                    sc = tb->_work_board->minmax_score(m_child);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        tb->set_score(sq0, sq1, sc);
                        tb->set_marker(sq0, sq1, false);
                    }
                    else
                    {
                        assert(false);
                    }
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_1v1<PieceID, _BoardSize>::process_marker(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1;
        std::vector<Move<PieceID>> m;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1);
            if (sq0 == sq1) continue;
            if (tb->marker(sq0, sq1) == false) 
                continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);

            m = tb->_work_board->generate_moves();
            sc = tb->_work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                tb->set_score(sq0, sq1, sc); 
                tb->set_marker(sq0, sq1, false);
                n_changes++;
            }
            else
            {
                sc = this->minmax_score(tb->index_item(sq0, sq1), tb, tb_oppo);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_score(sq0, sq1, sc);
                    tb->set_marker(sq0, sq1, false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseHandler_1v1<PieceID, _BoardSize>::minmax_score(const uint64_t& idx_item ,Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo)
    {
        ExactScore sc = tb->score_at_idx(idx_item);
        if (sc != ExactScore::UNKNOWN)
            return sc;

        bool has_all_child_score = true;
        ExactScore max_score = ExactScore::UNKNOWN;

        Move<PieceID> mv;
        std::vector<Move<PieceID>> m_child = tb->_work_board->generate_moves();
        for (size_t i = 0; i < m_child.size(); i++)
        {
            mv = m_child[i];
            tb->_work_board->apply_move(mv);
            if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
            {
                uint16_t child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                uint16_t child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                sc = tb_oppo->score(child_sq0, child_sq1);
                if (sc == ExactScore::UNKNOWN)
                {
                    has_all_child_score = false;
                }
                else
                {
                    max_score = best_score(tb->_work_board->get_color(), sc, max_score);
                }
            }
            else
            {
                // children
                if (tb->_work_board->cnt_all_piece() != (tb->_NPIECE - 1))
                {
                    has_all_child_score = false;
                    assert(false);
                    continue;
                }

                uint16_t child_sq0;
                Tablebase_1v0<PieceID, _BoardSize>* tb_child = locate_children(tb, tb_oppo, child_sq0);
                if (tb_child == nullptr)
                {
                    has_all_child_score = false;
                    assert(false);
                    continue;
                }

                sc = tb_child->score(child_sq0);
                if (sc == ExactScore::UNKNOWN)
                {
                    has_all_child_score = false;
                    assert(false);
                    continue;
                }

                max_score = best_score(tb->_work_board->get_color(), sc, max_score);
            }
            tb->_work_board->undo_move();
        }

        if (has_all_child_score)
        {
            if (max_score == ExactScore::LOSS)
            {
                int debug = 1;
            }
            return max_score;
        }

        if ((tb->_work_board->get_color() == PieceColor::W) && (max_score == ExactScore::WIN))  return ExactScore::WIN;
        if ((tb->_work_board->get_color() == PieceColor::B) && (max_score == ExactScore::LOSS)) return ExactScore::LOSS;
        return ExactScore::UNKNOWN;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseHandler_1v1<PieceID, _BoardSize>::best_score(PieceColor color_child, ExactScore sc, ExactScore best_score) const
    {
        if (color_child == PieceColor::B) // W at parent
        {
            if (sc == ExactScore::WIN) return ExactScore::WIN;
            else if (sc == ExactScore::DRAW)
            {
                if (best_score != ExactScore::WIN)
                    return ExactScore::DRAW;
            }
            else
            {
                // sc == ExactScore::LOSS
                if (best_score == ExactScore::UNKNOWN)
                    return ExactScore::LOSS;
            }
        }
        else
        {
            if (sc == ExactScore::LOSS)
                return ExactScore::LOSS;
            else if (sc == ExactScore::DRAW)
            {
                if (best_score != ExactScore::LOSS)
                    return ExactScore::DRAW;
            }
            else
            {
                // sc == ExactScore::WIN
                if (best_score == ExactScore::UNKNOWN)
                    return ExactScore::WIN;
            }
        }
        return best_score;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void TablebaseHandler_1v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }
};
#endif

