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
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_1v1 : public Tablebase<PieceID, _BoardSize, 2>
    {        
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;
        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        Tablebase_1v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c)
        {
        }

        bool load() override        { return read_tb(); }
        bool save() const override  { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
        bool isPiecesMatch(const _Board& pos) override;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_1v1<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 2) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[1])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_1v1 : public TablebaseBaseHandler<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        TablebaseHandler_1v1(std::vector<PieceID>& v) : TablebaseBaseHandler<PieceID, _BoardSize>(v)
        {
            _tb_W = new Tablebase_1v1<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_1v1<PieceID, _BoardSize>(v, PieceColor::B);

            _p_1v0.push_back(v[0]);
            _p_0v1.push_back(v[1]);

            _tb_1v0_w = new Tablebase_1v0<PieceID, _BoardSize>(_p_1v0, PieceColor::W);
            _tb_1v0_b = new Tablebase_1v0<PieceID, _BoardSize>(_p_1v0, PieceColor::B);
            _tb_0v1_w = new Tablebase_0v1<PieceID, _BoardSize>(_p_0v1, PieceColor::W);
            _tb_0v1_b = new Tablebase_0v1<PieceID, _BoardSize>(_p_0v1, PieceColor::B);
        }
        ~TablebaseHandler_1v1()
        {
            delete _tb_W;
            delete _tb_B;

            delete _tb_1v0_w;
            delete _tb_1v0_b;
            delete _tb_0v1_w;
            delete _tb_0v1_b;
        }

        bool build(char verbose = 0) override;
        bool is_build() const override  { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;

        Tablebase_1v1<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_1v1<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb);
        uint64_t set_marker(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo);
        ExactScore minmax_score(const uint64_t& idx_item, Tablebase_1v1<PieceID, _BoardSize>* tb, Tablebase_1v1<PieceID, _BoardSize>* tb_oppo);
        Tablebase_1v0<PieceID, _BoardSize>* locate_children_1v0(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0) const;
        Tablebase_0v1<PieceID, _BoardSize>* locate_children_0v1(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0) const;
        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const;
        void print() const;

        Tablebase_1v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_1v1<PieceID, _BoardSize>* _tb_B;

        std::vector<PieceID> _p_1v0;
        std::vector<PieceID> _p_0v1;

        // children TB
        Tablebase_1v0<PieceID, _BoardSize>* _tb_1v0_w;
        Tablebase_1v0<PieceID, _BoardSize>* _tb_1v0_b;
        Tablebase_0v1<PieceID, _BoardSize>* _tb_0v1_w;
        Tablebase_0v1<PieceID, _BoardSize>* _tb_0v1_b;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;
        if (!_tb_1v0_w->save()) return false;
        if (!_tb_1v0_b->save()) return false;
        if (!_tb_0v1_w->save()) return false;
        if (!_tb_0v1_b->save()) return false;
        return true;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::load()
    {
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        if (!_tb_1v0_w->load()) return false;
        if (!_tb_1v0_b->load()) return false;
        if (!_tb_0v1_w->load()) return false;
        if (!_tb_0v1_b->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::build(char verbose)
    {
        _tb_1v0_w->build(verbose);
        _tb_1v0_b->build(verbose);
        _tb_0v1_w->build(verbose);
        _tb_0v1_b->build(verbose);

        assert(_tb_1v0_w->is_build() == true);
        assert(_tb_1v0_b->is_build() == true);
        assert(_tb_0v1_w->is_build() == true);
        assert(_tb_0v1_b->is_build() == true);

        uint64_t n = 0;
        uint64_t m = 0;
        int iter = 0;
        
        set_mate_score(PieceColor::W, _tb_W);
        set_mate_score(PieceColor::B, _tb_B);

        do
        {
            iter++;  if (verbose) { std::cout << "Iteration TablebaseHandler_1v1: " << iter << std::endl; }

            _tb_W->clear_marker();
            _tb_B->clear_marker();

            n = set_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W set_marker positions:" << n << std::endl; }

            n = process_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

            m = set_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B set_marker positions:" << m << std::endl; }

            m = process_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }

            if (verbose) print();
            if ((n + m) == 0) break;

        } while (n+m > 0);

        _tb_W->set_build(true);
        _tb_B->set_build(true);
        _tb_W->set_unknown_to_draw();
        _tb_B->set_unknown_to_draw();

        assert(_tb_W->check_unknown() == false);
        assert(_tb_B->check_unknown() == false);
        return true;
    }

    // find_score_children_tb
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_1v1<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const
    {
        uint16_t ret_child_sq0;
        Tablebase_1v0<PieceID, _BoardSize>* tb_1v0 = locate_children_1v0(pos, color, ret_child_sq0);
        if (tb_1v0 != nullptr)
        {
            ret_sc = tb_1v0->score(ret_child_sq0);
            return true;
        }
        Tablebase_0v1<PieceID, _BoardSize>* tb_0v1 = locate_children_0v1(pos, color, ret_child_sq0);
        if (tb_0v1 != nullptr)
        {
            ret_sc = tb_0v1->score(ret_child_sq0);
            return true;
        }
        return false;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v0<PieceID, _BoardSize>* TablebaseHandler_1v1<PieceID, _BoardSize>::locate_children_1v0(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0) const
    {
        if ((color == PieceColor::W) && (_tb_1v0_w->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_1v0[0])->get_name(), PieceColor::W);
            return _tb_1v0_w;
        }
        if ((color == PieceColor::B) && (_tb_1v0_b->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_1v0[0])->get_name(), PieceColor::W);
            return _tb_1v0_b;
        }
        return nullptr;
    }
    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_0v1<PieceID, _BoardSize>* TablebaseHandler_1v1<PieceID, _BoardSize>::locate_children_0v1(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0) const
    {
        if ((color == PieceColor::W) && (_tb_0v1_w->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_0v1[0])->get_name(), PieceColor::B);
            return _tb_0v1_w;
        }
        if ((color == PieceColor::B) && (_tb_0v1_b->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_0v1[0])->get_name(), PieceColor::B);
            return _tb_0v1_b;
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

            sc = tb->score(sq0, sq1);
            if (sc != ExactScore::UNKNOWN) continue;

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
                    if (find_score_children_tb(*tb->_work_board, tb->_work_board->get_color(), sc))
                    {
                        if (sc != ExactScore::UNKNOWN)
                        {
                            one_child_has_score = true;
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    else
                    {
                        assert(false);
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
                                //n_changes++;
                            } 
                            n_changes++;
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
                        //n_changes++;
                    }
                    n_changes++;
                }
            }
            
            if ((!one_mark) && (one_child_has_score))
            {
                // all known child score
                sc = tb->score(sq0, sq1);
                if (sc == ExactScore::UNKNOWN)
                {
                    sc = this->minmax_score(tb->index_item(sq0, sq1), tb, tb_oppo);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        tb->set_score(sq0, sq1, sc);
                        tb->set_marker(sq0, sq1, false);
                        n_changes++;
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

                if (find_score_children_tb(*tb->_work_board, tb->_work_board->get_color(), sc))
                {
                    if (sc != ExactScore::UNKNOWN)
                    {
                        has_all_child_score = true;
                    }
                    else
                    {
                        has_all_child_score = false;
                        assert(false);
                        continue;
                    }
                }
                else
                {
                    has_all_child_score = false;
                    assert(false);
                    continue;
                }

                //sc = tb_child->score(child_sq0);
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
    void TablebaseHandler_1v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }
};
#endif

