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
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_2v1;
    template <typename PieceID, typename uint8_t _BoardSize> class Tablebase_2v0;

    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_2v1 : public Tablebase<PieceID, _BoardSize, 3>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        Tablebase_2v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 3>(v, c)
        {
        }

        bool load() override 
        {
            //...
            return false;
        }
        bool save()  override { return false; }
        bool build(char verbose = 0) override { return false; }

    protected:
        std::vector<PieceID> _child_piece_1v1_02w;
        std::vector<PieceID> _child_piece_1v1_12w;
        std::vector<PieceID> _child_piece_2v0_01w;
        std::vector<PieceID> _child_piece_1v1_02b;
        std::vector<PieceID> _child_piece_1v1_12b;
        std::vector<PieceID> _child_piece_2v0_01b;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_2v1
    {
        using _Piece = Piece<PieceID, _BoardSize>;

    public:
        TablebaseHandler_2v1(std::vector<PieceID>& v)
        {
            _tb_W = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::B);

            _tb_W->_child_piece_1v1_02w.push_back(v[0]);  _tb_W->_child_piece_1v1_02w.push_back(v[2]);
            _tb_W->_child_piece_1v1_12w.push_back(v[1]);  _tb_W->_child_piece_1v1_12w.push_back(v[2]);
            _tb_W->_child_piece_2v0_01w.push_back(v[0]);  _tb_W->_child_piece_2v0_01w.push_back(v[1]);
            _tb_W->_child_piece_1v1_02b.push_back(v[0]);  _tb_W->_child_piece_1v1_02b.push_back(v[2]);
            _tb_W->_child_piece_1v1_12b.push_back(v[1]);  _tb_W->_child_piece_1v1_12b.push_back(v[2]);
            _tb_W->_child_piece_2v0_01b.push_back(v[0]);  _tb_W->_child_piece_2v0_01b.push_back(v[1]);

            _tb_B->_child_piece_1v1_02w.push_back(v[0]);  _tb_B->_child_piece_1v1_02w.push_back(v[2]);
            _tb_B->_child_piece_1v1_12w.push_back(v[1]);  _tb_B->_child_piece_1v1_12w.push_back(v[2]);
            _tb_B->_child_piece_2v0_01w.push_back(v[0]);  _tb_B->_child_piece_2v0_01w.push_back(v[1]);
            _tb_B->_child_piece_1v1_02b.push_back(v[0]);  _tb_B->_child_piece_1v1_02b.push_back(v[2]);
            _tb_B->_child_piece_1v1_12b.push_back(v[1]);  _tb_B->_child_piece_1v1_12b.push_back(v[2]);
            _tb_B->_child_piece_2v0_01b.push_back(v[0]);  _tb_B->_child_piece_2v0_01b.push_back(v[1]);

            _tbh_child_piece_1v1_02w = new TablebaseHandler_1v1<PieceID, _BoardSize>(_tb_W->_child_piece_1v1_02w);
            _tbh_child_piece_1v1_12w = new TablebaseHandler_1v1<PieceID, _BoardSize>(_tb_W->_child_piece_1v1_12w);
            _tbh_child_piece_1v1_02b = new TablebaseHandler_1v1<PieceID, _BoardSize>(_tb_W->_child_piece_1v1_02b);
            _tbh_child_piece_1v1_12b = new TablebaseHandler_1v1<PieceID, _BoardSize>(_tb_W->_child_piece_1v1_12b);

            _tb_W->_children.push_back(_tbh_child_piece_1v1_02w->tb_W());
            _tb_W->_children.push_back(_tbh_child_piece_1v1_12w->tb_W());
            _tb_W->_children.push_back(new Tablebase_2v0<PieceID, _BoardSize>(_tb_W->_child_piece_2v0_01w, PieceColor::W));
            _tb_W->_children.push_back(_tbh_child_piece_1v1_02b->tb_W());
            _tb_W->_children.push_back(_tbh_child_piece_1v1_12b->tb_W());
            _tb_W->_children.push_back(new Tablebase_2v0<PieceID, _BoardSize>(_tb_W->_child_piece_2v0_01b, PieceColor::B));

            _tb_B->_children.push_back(_tbh_child_piece_1v1_02w->tb_B());
            _tb_B->_children.push_back(_tbh_child_piece_1v1_12w->tb_B());
            _tb_B->_children.push_back(new Tablebase_2v0<PieceID, _BoardSize>(_tb_B->_child_piece_2v0_01w, PieceColor::W));
            _tb_B->_children.push_back(_tbh_child_piece_1v1_02b->tb_B());
            _tb_B->_children.push_back(_tbh_child_piece_1v1_12b->tb_B());
            _tb_B->_children.push_back(new Tablebase_2v0<PieceID, _BoardSize>(_tb_B->_child_piece_2v0_01b, PieceColor::B));
        }

        ~TablebaseHandler_2v1()
        {
            delete _tb_W;
            delete _tb_B;
        }

        bool build(char verbose = 0);

    protected:
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb);
        uint64_t set_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo);
        ExactScore minmax_score(const uint64_t& idx_item, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo);
        Tablebase_1v1<PieceID, _BoardSize>* locate_children_1v1(PieceColor c, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const;
        Tablebase_2v0<PieceID, _BoardSize>* locate_children_2v0(PieceColor c, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const;
        ExactScore best_score(PieceColor color_child, ExactScore current_score, ExactScore best_score) const;
        void print() const;

        Tablebase_2v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_2v1<PieceID, _BoardSize>* _tb_B;

        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_child_piece_1v1_02w;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_child_piece_1v1_12w;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_child_piece_1v1_02b;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_child_piece_1v1_12b;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::build(char verbose)
    {
        {
            _tbh_child_piece_1v1_02w->build(verbose);
            _tbh_child_piece_1v1_12w->build(verbose);
            Tablebase_2v0<PieceID, _BoardSize>* tb_2 = ((Tablebase_2v0<PieceID, _BoardSize>*)_tb_W->children()[2]);
            tb_2->build(verbose);
            _tbh_child_piece_1v1_02b->build(verbose);
            _tbh_child_piece_1v1_12b->build(verbose);
            Tablebase_2v0<PieceID, _BoardSize>* tb_5 = ((Tablebase_2v0<PieceID, _BoardSize>*)_tb_W->children()[5]);
            tb_5->build(verbose);

            assert(tb_2->is_build() == true);
            assert(tb_5->is_build() == true);
        }
        {
           /* _tbh_child_piece_1v1_02w->build(verbose);
            _tbh_child_piece_1v1_12w->build(verbose);*/
            Tablebase_2v0<PieceID, _BoardSize>* tb_2 = ((Tablebase_2v0<PieceID, _BoardSize>*)_tb_B->children()[2]);
            tb_2->build(verbose);
            /*_tbh_child_piece_1v1_02b->build(verbose);
            _tbh_child_piece_1v1_12b->build(verbose);*/
            Tablebase_2v0<PieceID, _BoardSize>* tb_5 = ((Tablebase_2v0<PieceID, _BoardSize>*)_tb_B->children()[5]);
            tb_5->build(verbose);

            assert(tb_2->is_build() == true);
            assert(tb_5->is_build() == true);
        }

        uint64_t n = 0;
        int iter = 0;
        
        if (verbose) { std::cout << "scanning mate..." << n << std::endl; }
        set_mate_score(PieceColor::W, _tb_W);
        set_mate_score(PieceColor::B, _tb_B);

        do
        {
            iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

            n  = set_marker(PieceColor::W, _tb_W, _tb_B);
            n += set_marker(PieceColor::B, _tb_B, _tb_W);

            if (verbose) { std::cout << "set_marker positions:" << n << std::endl; }
            if (n == 0) return true;

            n =  process_marker(PieceColor::W, _tb_W, _tb_B);
            n += process_marker(PieceColor::B, _tb_B, _tb_W);

            if (verbose) { std::cout << "process_marker positions:" << n << std::endl; }
            if (verbose) print();

        } while (n > 0);

        _tb_W->_is_build = true;
        _tb_B->_is_build = true;
        _tb_W->set_unknown_to_draw();
        _tb_B->set_unknown_to_draw();
        return true;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v1<PieceID, _BoardSize>* TablebaseHandler_2v1<PieceID, _BoardSize>::
    locate_children_1v1(PieceColor c, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo,
                        uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const
    {
        // _child_piece_1v1_02w
        if (c == PieceColor::W)
        {
            if ((tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_02w[0])->get_name(), PieceColor::W)) &&
                (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_02w[1])->get_name(), PieceColor::B)))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_02w[0])->get_name(), PieceColor::W);
                ret_child_sq1 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_02w[1])->get_name(), PieceColor::B);
                return (Tablebase_1v1<PieceID, _BoardSize>*)tb->_children[0];
            }
        }
        // _child_piece_1v1_12w
        if (c == PieceColor::W)
        {
            if ((tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_12w[0])->get_name(), PieceColor::W)) &&
                (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_12w[1])->get_name(), PieceColor::B)))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_12w[0])->get_name(), PieceColor::W);
                ret_child_sq1 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_12w[1])->get_name(), PieceColor::B);
                return (Tablebase_1v1<PieceID, _BoardSize>*)tb->_children[1];
            }
        }

        // _child_piece_1v1_02b
        if (c == PieceColor::B)
        {
            if ((tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_02b[0])->get_name(), PieceColor::W)) &&
                (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_02b[1])->get_name(), PieceColor::B)))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_02b[0])->get_name(), PieceColor::W);
                ret_child_sq1 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_02b[1])->get_name(), PieceColor::B);
                return (Tablebase_1v1<PieceID, _BoardSize>*)tb->_children[3];
            }
        }
        // _child_piece_1v1_12b
        if (c == PieceColor::B)
        {
            if ((tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_12b[0])->get_name(), PieceColor::W)) &&
                (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_1v1_12b[1])->get_name(), PieceColor::B)))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_12b[0])->get_name(), PieceColor::W);
                ret_child_sq1 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_1v1_12b[1])->get_name(), PieceColor::B);
                return (Tablebase_1v1<PieceID, _BoardSize>*)tb->_children[4];
            }
        }

        return nullptr;
    }
    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_2v0<PieceID, _BoardSize>* 
    TablebaseHandler_2v1<PieceID, _BoardSize>::locate_children_2v0(PieceColor c, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const
    {
        // _child_piece_2v0_01w
        if (c == PieceColor::W)
        {
            if ((tb->_work_board->has_piece(_Piece::get(tb->_child_piece_2v0_01w[0])->get_name(), PieceColor::W)) &&
                (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_2v0_01w[1])->get_name(), PieceColor::W)))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_2v0_01w[0])->get_name(), PieceColor::W);
                ret_child_sq1 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_2v0_01w[1])->get_name(), PieceColor::W);
                return (Tablebase_2v0<PieceID, _BoardSize>*)tb->_children[2];
            }
        }

        // _child_piece_2v0_01b
        if (c == PieceColor::B)
        {
            if ((tb->_work_board->has_piece(_Piece::get(tb->_child_piece_2v0_01b[0])->get_name(), PieceColor::W)) &&
                (tb->_work_board->has_piece(_Piece::get(tb->_child_piece_2v0_01b[1])->get_name(), PieceColor::W)))
            {
                ret_child_sq0 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_2v0_01b[0])->get_name(), PieceColor::W);
                ret_child_sq1 = tb->_work_board->get_square_ofpiece(_Piece::get(tb->_child_piece_2v0_01b[1])->get_name(), PieceColor::W);
                return (Tablebase_2v0<PieceID, _BoardSize>*)tb->_children[5];
            }
        }

        return nullptr;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_2v1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb)
    {      
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<Move<PieceID>> m;

        for (uint16_t sq0 = 0; sq0 < tb->_dim1; sq0++)
        for (uint16_t sq1 = 0; sq1 < tb->_dim1; sq1++)
        for (uint16_t sq2 = 0; sq2 < tb->_dim1; sq2++)
        {
            if ((sq0 != sq1) && (sq0 != sq2) && (sq1 != sq2))
            {
                tb->_work_board->clear();
                tb->_work_board->set_color(color_to_play);
                tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
                tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
                tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);
                m = tb->_work_board->generate_moves();
                sc = tb->_work_board->final_score(m);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseHandler_2v1<PieceID, _BoardSize>::set_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        Move<PieceID> mv;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m_child;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);

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
                    uint16_t child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                    sc = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        one_child_has_score = true;
                    }
                }
                else
                {
                    uint16_t child_sq0; uint16_t child_sq1;
                    {
                        Tablebase_1v1<PieceID, _BoardSize>* tb_child_1v1 = locate_children_1v1(tb->_work_board->get_color(), tb, tb_oppo, child_sq0, child_sq1);
                        if (tb_child_1v1 == nullptr)
                        {
                            Tablebase_2v0<PieceID, _BoardSize>* tb_child_2v0 = locate_children_2v0(tb->_work_board->get_color(), tb, tb_oppo, child_sq0, child_sq1);
                            if (tb_child_2v0 != nullptr)
                            {
                                sc = tb_child_2v0->score(child_sq0, child_sq1);
                            }
                            else
                            {
                                assert(false);
                            }
                        }
                        else
                        {
                            sc = tb_child_1v1->score(child_sq0, child_sq1);
                        }
                    }
                    if (sc != ExactScore::UNKNOWN)
                    {
                        one_child_has_score = true;
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
                        uint16_t child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                        sc = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                        if (sc == ExactScore::UNKNOWN)
                        {
                            // mark child
                            if (tb_oppo->marker(child_sq0, child_sq1, child_sq2) == false)
                            {
                                tb_oppo->set_marker(child_sq0, child_sq1, child_sq2, true);
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
                sc = tb->score(sq0, sq1, sq2);
                if (sc == ExactScore::UNKNOWN)
                {
                    if (tb->marker(sq0, sq1, sq2) == false)
                    {
                        tb->set_marker(sq0, sq1, sq2, true);
                        n_changes++;
                    }
                }
            }
            
            if ((!one_mark) && (one_child_has_score))
            {
                // all known child score
                sc = tb->score(sq0, sq1, sq2);
                if (sc == ExactScore::UNKNOWN)
                {
                    sc = tb->_work_board->minmax_score(m_child);
                    if (sc != ExactScore::UNKNOWN)
                    {
                        tb->set_score(sq0, sq1, sq2, sc);
                        tb->set_marker(sq0, sq1, sq2, false);
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
    uint64_t TablebaseHandler_2v1<PieceID, _BoardSize>::process_marker(PieceColor color_to_play, Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;
            if (tb->marker(sq0, sq1, sq2) == false) 
                continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            m = tb->_work_board->generate_moves();
            sc = tb->_work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                tb->set_score(sq0, sq1, sq2, sc);
                tb->set_marker(sq0, sq1, sq2, false);
                n_changes++;
            }
            else
            {
                sc = this->minmax_score(tb->index_item(sq0, sq1, sq2), tb, tb_oppo);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseHandler_2v1<PieceID, _BoardSize>::minmax_score(const uint64_t& idx_item ,Tablebase_2v1<PieceID, _BoardSize>* tb, Tablebase_2v1<PieceID, _BoardSize>* tb_oppo)
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
                uint16_t child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                sc = tb_oppo->score(child_sq0, child_sq1, child_sq1);
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

                uint16_t child_sq0; uint16_t child_sq1;
                {
                    Tablebase_1v1<PieceID, _BoardSize>* tb_child_1v1 = locate_children_1v1(tb->_work_board->get_color(), tb, tb_oppo, child_sq0, child_sq1);
                    if (tb_child_1v1 == nullptr)
                    {
                        Tablebase_2v0<PieceID, _BoardSize>* tb_child_2v0 = locate_children_2v0(tb->_work_board->get_color(), tb, tb_oppo, child_sq0, child_sq1);
                        if (tb_child_2v0 != nullptr)
                        {
                            sc = tb_child_2v0->score(child_sq0, child_sq1);
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
                        sc = tb_child_1v1->score(child_sq0, child_sq1);
                    }
                }

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
    ExactScore TablebaseHandler_2v1<PieceID, _BoardSize>::best_score(PieceColor color_child, ExactScore sc, ExactScore best_score) const
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
    void TablebaseHandler_2v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }
};
#endif

