#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseBaseHandler_2   : handle 2 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_2_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_2_HPP

namespace chess
{
    // TablebaseBaseHandler_2
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseBaseHandler_2 : public TablebaseBaseHandlerCore<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TablebaseBaseHandler_2(std::vector<PieceID>& v) : TablebaseBaseHandlerCore(v)
        {
        }
        virtual ~TablebaseBaseHandler_2()
        {
        }

    protected:
        bool     build_base(Tablebase<PieceID, _BoardSize, 2>* tb, Tablebase<PieceID, _BoardSize, 2>* tb_oppo, char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 2>* tb);
        uint64_t set_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 2>* tb, Tablebase<PieceID, _BoardSize, 2>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 2>* tb, Tablebase<PieceID, _BoardSize, 2>* tb_oppo);
    };


    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler_2<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 2>* tb)
    {
        size_t ret_idx;
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<Move<PieceID>> m;

        for (uint16_t sq0 = 0; sq0 < tb->_dim1; sq0++)
        {
            for (uint16_t sq1 = 0; sq1 < tb->_dim1; sq1++)
            {
                {
                    if (sq0 != sq1)
                    {
                        _work_board->clear();
                        _work_board->set_color(color_to_play);
                        _work_board->set_pieceid_at(tb->_piecesID[0], sq0);
                        _work_board->set_pieceid_at(tb->_piecesID[1], sq1);

                        m = _work_board->generate_moves();
                        sc = _work_board->final_score(m);
                        if (sc != ExactScore::UNKNOWN)
                        {
                            tb->set_dtc(sq0, sq1, 0);
                            tb->set_score(sq0, sq1, sc);
                            tb->set_marker(sq0, sq1, false);
                            n_changes++;
                        }
                        else if (_work_board->can_capture_opposite_king(m, ret_idx))
                        {
                            if (_work_board->get_color() == PieceColor::W)
                            {
                                tb->set_dtc(sq0, sq1, 1);
                                tb->set_score(sq0, sq1, ExactScore::WIN);
                                tb->set_marker(sq0, sq1, false);
                                n_changes++;
                            }
                            else
                            {
                                tb->set_dtc(sq0, sq1, 1);
                                tb->set_score(sq0, sq1, ExactScore::LOSS);
                                tb->set_marker(sq0, sq1, false);
                                n_changes++;
                            }
                        }
                    }
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler_2<PieceID, _BoardSize>::set_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 2>* tb, Tablebase<PieceID, _BoardSize,2>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1;
        std::vector<Move<PieceID>> m_child;
        std::vector<ExactScore> child_sc;
        std::vector<uint8_t>    child_dtc;
        uint16_t child_sq0; uint16_t child_sq1;
        bool exist_child_score;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1);
            if (sq0 == sq1) continue;
            sc = tb->score(sq0, sq1);
            if (sc != ExactScore::UNKNOWN) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            _work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            _work_board->set_pieceid_at(tb->_piecesID[1], sq1);

            m_child = _work_board->generate_moves();
            child_sc.clear();
            child_dtc.clear();
            for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
            for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);

            exist_child_score = false;
            for (size_t j = 0; j < m_child.size(); j++)
            {
                _work_board->apply_move(m_child[j]);
                if (_work_board->cnt_all_piece() == tb->_NPIECE)
                {
                    child_sq0 = _work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                    child_sq1 = _work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                    child_sc[j] = tb_oppo->score(child_sq0, child_sq1);
                    child_dtc[j] = tb_oppo->dtc(child_sq0, child_sq1);
                }
                else
                {
                    // child in other tb
                    if (find_score_children_tb(*_work_board, _work_board->get_color(), sc))
                    {
                        child_sc[j] = sc;
                        child_dtc[j] = 0;
                    }
                    else
                    {
                        assert(false);
                    }
                }
                if (child_sc[j] != ExactScore::UNKNOWN) exist_child_score = true;
                _work_board->undo_move();
            }

            sc = tb->score(sq0, sq1);
            if (sc == ExactScore::UNKNOWN)
            {
                uint8_t ret_dtc;
                sc = minmax_dtc(tb->color(), child_sc, child_dtc, ret_dtc);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, 1 + ret_dtc);
                    tb->set_score(sq0, sq1, sc);
                    tb->set_marker(sq0, sq1, false);
                    n_changes++;
                }
                else if (exist_child_score)
                {
                    // mark parent
                    if (tb->marker(sq0, sq1) == false)
                    {
                        tb->set_marker(sq0, sq1, true);
                    }
                    n_changes++;
                }
            }
        }
        return n_changes;
    }


    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler_2<PieceID, _BoardSize>::process_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize,2>* tb, Tablebase<PieceID, _BoardSize, 2>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1;
        std::vector<Move<PieceID>> m;

        uint16_t child_sq0; uint16_t child_sq1;
        std::vector<Move<PieceID>> m_child;
        std::vector<ExactScore> child_sc;
        std::vector<uint8_t>    child_dtc;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1);
            if (sq0 == sq1) continue;
            if (tb->marker(sq0, sq1) == false)
                continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            _work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            _work_board->set_pieceid_at(tb->_piecesID[1], sq1);

            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                if (tb->score(sq0, sq1) == ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, 0);
                    tb->set_score(sq0, sq1, sc);
                    tb->set_marker(sq0, sq1, false);
                    n_changes++;
                }
            }
            else if (tb->score(sq0, sq1) == ExactScore::UNKNOWN)
            {
                m_child = _work_board->generate_moves();
                child_sc.clear();
                child_dtc.clear();
                for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
                for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);

                for (size_t j = 0; j < m_child.size(); j++)
                {
                    _work_board->apply_move(m_child[j]);
                    if (_work_board->cnt_all_piece() == tb->_NPIECE)
                    {
                        child_sq0 = _work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                        child_sq1 = _work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                        child_sc[j] = tb_oppo->score(child_sq0, child_sq1);
                        child_dtc[j] = tb_oppo->dtc(child_sq0, child_sq1);
                    }
                    else
                    {
                        if (find_score_children_tb(*_work_board, _work_board->get_color(), sc))
                        {
                            child_sc[j] = sc;
                            child_dtc[j] = 0;
                        }
                    }
                    _work_board->undo_move();
                }

                uint8_t ret_dtc;
                sc = minmax_dtc(tb->color(), child_sc, child_dtc, ret_dtc);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, 1 + ret_dtc);
                    tb->set_score(sq0, sq1, sc);
                    tb->set_marker(sq0, sq1, false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseBaseHandler_2<PieceID, _BoardSize>::build_base(Tablebase<PieceID, _BoardSize, 2>* _tb_W, Tablebase<PieceID, _BoardSize, 2>* _tb_B, char verbose)
    {
        uint64_t n = 0;
        uint64_t m = 0;
        int iter = 0;

        if (verbose) { std::cout << "scanning mate in (0/1) ply ..." << std::endl; }

        n = set_mate_score(PieceColor::W, _tb_W);
        if (verbose) { std::cout << "W (0/1 move) mate positions:" << n << std::endl; }
        if (verbose) _tb_W->print_dtc(10);

        n = set_mate_score(PieceColor::B, _tb_B);
        if (verbose) { std::cout << "B (0/1 move) mate positions:" << n << std::endl; }
        if (verbose) _tb_W->print_dtc(10);

        do
        {
            iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

            _tb_W->clear_marker();
            _tb_B->clear_marker();

            n = set_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W set_marker positions:" << n << std::endl; }

            n = process_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }
            if (verbose) _tb_W->print_dtc(10);

            m = process_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }
            if (verbose) _tb_W->print_dtc(10);


            m = set_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B set_marker positions:" << m << std::endl; }

            n = process_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }
            if (verbose) _tb_W->print_dtc(10);

            m = process_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }
            if (verbose) _tb_W->print_dtc(10);

            if ((n + m) == 0) break;
        } while (n + m > 0);

        if (verbose) _tb_W->print_dtc(5);

        _tb_W->set_build(true);
        _tb_B->set_build(true);
        _tb_W->set_unknown_to_draw();
        _tb_B->set_unknown_to_draw();
        TablebaseManager<PieceID, _BoardSize>::instance()->add(_tb_W->name(), _tb_W);
        TablebaseManager<PieceID, _BoardSize>::instance()->add(_tb_B->name(), _tb_B);
        return true;
    }

};
#endif

