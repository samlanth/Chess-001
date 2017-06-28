#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// algo for TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TB_ALGO_HPP
#define _AL_CHESS_TABLEBASE_TB_ALGO_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t setup_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo);

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t process_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo);

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    bool build_base_v(TBH<PieceID, _BoardSize>* tbh, Tablebase<PieceID, _BoardSize, NPIECE>** tbw, Tablebase<PieceID, _BoardSize, NPIECE>** tbb, char verbose);

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore minmax_dtc(PieceColor color_parent, const std::vector<ExactScore>&  child_sc, const std::vector<uint8_t>& child_dtc, std::vector<bool>& child_is_promo, uint8_t& ret_dtc, size_t& ret_idx);

    // minmax_dtc
    template <typename PieceID, typename uint8_t _BoardSize>
    inline ExactScore minmax_dtc(   PieceColor color_parent,const std::vector<ExactScore>&  child_sc,const std::vector<uint8_t>& child_dtc, std::vector<bool>& child_is_promo,
                                    uint8_t& ret_dtc, size_t& ret_idx)
    {
        if (child_sc.size() == 0)
        {
            ret_dtc = 0;
            ret_idx = 0;
            return ExactScore::UNKNOWN;
        }

        if (color_parent == PieceColor::W)
        {
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::WIN)
                {
                    if (child_is_promo[i]) // 1 move promo and win
                    {
                        ret_dtc = child_dtc[i];
                        ret_idx = i;
                        return ExactScore::WIN;
                    }
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::WIN)
                {
                    // seek lower dtc
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::WIN)
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                    }
                    ret_dtc = child_dtc[min_idx];
                    ret_idx = min_idx;
                    return ExactScore::WIN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::UNKNOWN)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::DRAW;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::LOSS)
                {
                    // seek longer dtc
                    uint8_t max_dtc = child_dtc[i];
                    size_t  max_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::LOSS)
                            if (child_dtc[j] > max_dtc) { max_dtc = child_dtc[j]; max_idx = j; }
                    }
                    ret_dtc = child_dtc[max_idx];
                    ret_idx = max_idx;
                    return ExactScore::LOSS;
                }
            }
        }
        else
        {
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::LOSS)
                {
                    if (child_is_promo[i]) // 1 move promo and loss
                    {
                        ret_dtc = child_dtc[i];
                        ret_idx = i;
                        return ExactScore::LOSS;
                    }
                }
            }

            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::LOSS)
                {
                    // seek lower dtc
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::LOSS)
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                    }
                    ret_dtc = child_dtc[min_idx];
                    ret_idx = min_idx;
                    return ExactScore::LOSS;
                }
            }

            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::UNKNOWN)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }

            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
                    ret_idx = i;
                    ret_dtc = child_dtc[i];
                    return ExactScore::DRAW;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::WIN)
                {
                    // seek longer dtc
                    uint8_t max_dtc = child_dtc[i];
                    size_t  max_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::WIN)
                            if (child_dtc[j] > max_dtc) { max_dtc = child_dtc[j]; max_idx = j; }
                    }
                    ret_dtc = child_dtc[max_idx];
                    ret_idx = max_idx;
                    return ExactScore::WIN;
                }
            }
        }
        ret_idx = 0;
        ret_dtc = 0;
        return ExactScore::UNKNOWN;
    }

    // set_mate_score_v - template function
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t set_mate_score_v(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb)
    {
        size_t      ret_idx;
        uint64_t    n_changes = 0;
        ExactScore  sc;
        std::vector<uint16_t> sq;
        if (NPIECE >= 1) sq.push_back(0);
        if (NPIECE >= 2) sq.push_back(0);
        if (NPIECE >= 3) sq.push_back(0);
        std::vector<Move<PieceID>> m;

        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();
        for (uint64_t i = 0; i < tb->size_tb(); i++)
        {
            tb->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);
            if (NPIECE == 2) { if (sq[0] == sq[1]) continue; }
            else if (NPIECE == 3) { if ((sq[0] == sq[1]) || (sq[0] == sq[2]) || (sq[1] == sq[2])) continue; }

            _work_board->clear();
            _work_board->set_color(color_to_play);
            if (NPIECE >= 1) _work_board->set_pieceid_at(tb->_piecesID[0], sq[0]);
            if (NPIECE >= 2) _work_board->set_pieceid_at(tb->_piecesID[1], sq[1]);
            if (NPIECE >= 3) _work_board->set_pieceid_at(tb->_piecesID[2], sq[2]);

            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                tb->set_dtc_v(sq, 0);
                tb->set_score_v(sq, sc);
                tb->set_marker_v(sq, false);
                n_changes++;
            }
            else if (_work_board->can_capture_opposite_king(m, ret_idx))
            {
                if (_work_board->get_color() == PieceColor::W)
                {
                    tb->set_dtc_v(sq, 1);
                    tb->set_score_v(sq, ExactScore::WIN);
                    tb->set_marker_v(sq, false);
                    n_changes++;
                }
                else
                {
                    tb->set_dtc_v(sq, 1);
                    tb->set_score_v(sq, ExactScore::LOSS);
                    tb->set_marker_v(sq, false);
                    n_changes++;
                }
            }
        }
        delete _work_board;
        return n_changes;
    };

    // set_marker  - template function
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t setup_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<uint16_t> sq;
        if (NPIECE >= 1) sq.push_back(0);
        if (NPIECE >= 2) sq.push_back(0);
        if (NPIECE >= 3) sq.push_back(0);
        std::vector<Move<PieceID>>  m_child;
        std::vector<ExactScore>     child_sc;
        std::vector<uint8_t>        child_dtc;
        std::vector<bool>           child_is_promo;
        std::vector<uint16_t> child_sq;
        if (NPIECE >= 1) child_sq.push_back(0);
        if (NPIECE >= 2) child_sq.push_back(0);
        if (NPIECE >= 3) child_sq.push_back(0);
        bool exist_child_score;
        bool isPromo;
        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);
            if (NPIECE == 2) {if (sq[0] == sq[1]) continue;}
            else if (NPIECE == 3) { if ((sq[0] == sq[1]) || (sq[0] == sq[2]) || (sq[1] == sq[2])) continue; }
            sc = tb->score_v(sq);
            if (sc != ExactScore::UNKNOWN) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            if (NPIECE >= 1) _work_board->set_pieceid_at(tb->_piecesID[0], sq[0]);
            if (NPIECE >= 2) _work_board->set_pieceid_at(tb->_piecesID[1], sq[1]);
            if (NPIECE >= 3) _work_board->set_pieceid_at(tb->_piecesID[2], sq[2]);

            m_child = _work_board->generate_moves();
            child_sc.clear();
            child_dtc.clear();
            child_is_promo.clear();
            for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
            for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);
            for (size_t j = 0; j < m_child.size(); j++) child_is_promo.push_back(false);

            exist_child_score = false;
            for (size_t j = 0; j < m_child.size(); j++)
            {
                _work_board->apply_move(m_child[j]);
                isPromo = _work_board->is_last_move_promo();
                if ((_work_board->cnt_all_piece() == tb->_NPIECE) && !isPromo)
                {
                    if (NPIECE >= 1) child_sq[0] = _work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                    if (NPIECE >= 2) child_sq[1] = _work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color(), true);
                    if (NPIECE >= 3) child_sq[2] = _work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                    tb->order_sq_v(child_sq);
                    if (NPIECE == 2) assert(!(child_sq[0] == child_sq[1]));
                    if (NPIECE == 3) assert(!((child_sq[0] == child_sq[1]) || (child_sq[0] == child_sq[2]) || (child_sq[1] == child_sq[2])));
                    child_sc[j]  = tb_oppo->score_v(child_sq);
                    child_dtc[j] = tb_oppo->dtc_v(child_sq);
                }
                else
                {
                    // child in other tb
                    if (tbh->find_score_children_tb(*_work_board, _work_board->get_color(), isPromo, sc))
                    {
                        child_sc[j] = sc;
                        child_dtc[j] = 0;
                        child_is_promo[j] = isPromo;
                    }
                    else
                    {
                        assert(false);
                    }
                }
                if (child_sc[j] != ExactScore::UNKNOWN) exist_child_score = true;
                _work_board->undo_move();
            }

            sc = tb->score_v(sq);
            if (sc == ExactScore::UNKNOWN)
            {
                uint8_t ret_dtc; size_t ret_idx;
                sc = minmax_dtc<PieceID, _BoardSize>(tb->color(), child_sc, child_dtc, child_is_promo, ret_dtc, ret_idx);
                if (sc != ExactScore::UNKNOWN)
                {
                    if (!child_is_promo[ret_idx])  tb->set_dtc_v(sq, 1 + ret_dtc);
                    else tb->set_dtc_v(sq, 1);
                    tb->set_score_v(sq, sc);
                    tb->set_marker_v(sq, false);
                    n_changes++;
                }
                else if (exist_child_score)
                {
                    // mark parent for later re-visit
                    if (tb->marker_v(sq) == false)
                    {
                        tb->set_marker_v(sq, true);
                    }
                    n_changes++;
                }
            }
        }
        delete _work_board;
        return n_changes;
    }

    // process_marker_v - template function
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    inline uint64_t process_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<uint16_t> sq;
        if (NPIECE >= 1) sq.push_back(0);
        if (NPIECE >= 2) sq.push_back(0);
        if (NPIECE >= 3) sq.push_back(0);
        std::vector<Move<PieceID>>  m;
        std::vector<Move<PieceID>>  m_child;
        std::vector<ExactScore>     child_sc;
        std::vector<uint8_t>        child_dtc;
        std::vector<bool>           child_is_promo;
        std::vector<uint16_t> child_sq;
        if (NPIECE >= 1) child_sq.push_back(0);
        if (NPIECE >= 2) child_sq.push_back(0);
        if (NPIECE >= 3) child_sq.push_back(0);
        bool isPromo;
        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);
            if      (NPIECE == 2) { if (sq[0] == sq[1]) continue; }
            else if (NPIECE == 3) { if ((sq[0] == sq[1]) || (sq[0] == sq[2]) || (sq[1] == sq[2])) continue; }
            if (tb->marker_v(sq) == false) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            if (NPIECE >= 1) _work_board->set_pieceid_at(tb->_piecesID[0], sq[0]);
            if (NPIECE >= 2) _work_board->set_pieceid_at(tb->_piecesID[1], sq[1]);
            if (NPIECE >= 3) _work_board->set_pieceid_at(tb->_piecesID[2], sq[2]);

            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                if (tb->score_v(sq) == ExactScore::UNKNOWN)
                {
                    tb->set_dtc_v(sq, 0);
                    tb->set_score_v(sq, sc);
                    tb->set_marker_v(sq, false);
                    n_changes++;
                }
            }
            else if (tb->score_v(sq) == ExactScore::UNKNOWN)
            {
                m_child = _work_board->generate_moves();
                child_sc.clear();
                child_dtc.clear();
                child_is_promo.clear();
                for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
                for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);
                for (size_t j = 0; j < m_child.size(); j++) child_is_promo.push_back(false);
                for (size_t j = 0; j < m_child.size(); j++)
                {
                    _work_board->apply_move(m_child[j]);
                    isPromo = _work_board->is_last_move_promo();
                    if ((_work_board->cnt_all_piece() == tb->_NPIECE) && !isPromo)
                    {
                        if (NPIECE >= 1) child_sq[0] = _work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                        if (NPIECE >= 2) child_sq[1] = _work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color(), true);
                        if (NPIECE >= 3) child_sq[2] = _work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                        tb->order_sq_v(child_sq);
                        if (NPIECE == 2) assert(!(child_sq[0] == child_sq[1]));
                        if (NPIECE == 3) assert(!((child_sq[0] == child_sq[1]) || (child_sq[0] == child_sq[2]) || (child_sq[1] == child_sq[2])));
                        child_sc[j] = tb_oppo->score_v(child_sq);
                        child_dtc[j] = tb_oppo->dtc_v(child_sq);
                    }
                    else
                    {
                        // child in other tb
                        if (tbh->find_score_children_tb(*_work_board, _work_board->get_color(), isPromo, sc))
                        {
                            child_sc[j] = sc;
                            child_dtc[j] = 0;
                            child_is_promo[j] = isPromo;
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    _work_board->undo_move();
                }

                uint8_t ret_dtc; size_t ret_idx;
                sc = minmax_dtc<PieceID, _BoardSize>(tb->color(), child_sc, child_dtc, child_is_promo, ret_dtc, ret_idx);
                if (sc != ExactScore::UNKNOWN)
                {
                    if (!child_is_promo[ret_idx])  tb->set_dtc_v(sq, 1 + ret_dtc);
                    else tb->set_dtc_v(sq, 1);
                    tb->set_score_v(sq, sc);
                    tb->set_marker_v(sq, false);
                    n_changes++;
                }
            }
        }
        delete _work_board;
        return n_changes;
    }

    // build_base_v - template function
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    inline bool build_base_v(TBH<PieceID, _BoardSize>* tbh, Tablebase<PieceID, _BoardSize, NPIECE>** tb_W, Tablebase<PieceID, _BoardSize, NPIECE>** tb_B, char verbose)
    {
        if (tbh->is_build()) return true;

        // Lookup in memory
        TablebaseBase<PieceID, _BoardSize>* tw = TablebaseManager<PieceID, _BoardSize>::instance()->find(NPIECE, tbh->pieceSet().name(PieceColor::W));
        TablebaseBase<PieceID, _BoardSize>* tb = TablebaseManager<PieceID, _BoardSize>::instance()->find(NPIECE, tbh->pieceSet().name(PieceColor::B));
        if ((tw != nullptr) && (tb != nullptr))
        {
            if ((tw->is_build() == true) && (tb->is_build() == true))
            {
                // Replace and quit
                (*tb_W) = (Tablebase<PieceID, _BoardSize, NPIECE>*)tw;
                (*tb_B) = (Tablebase<PieceID, _BoardSize, NPIECE>*)tb;
                return true;
            }
        }

        // Lookup on disk
        if (tbh->option() == TBH_OPTION::try_load_on_build)
        {
            if (tbh->load()) return true;
        }

        uint64_t n = 0;
        uint64_t m = 0;
        int iter = 0;

        if (verbose) { std::cout << TB_TYPE_to_string(tbh->tb_type()) << " " << tbh->pieceSet().name(PieceColor::W) << tbh->pieceSet().name(PieceColor::B) << " scanning mate in (0/1) ply ..." << std::endl; }

        n = set_mate_score_v<PieceID, _BoardSize, NPIECE>(PieceColor::W, (*tb_W));
        if (verbose) { std::cout << "W (0/1 ply) mate positions:" << n << std::endl; }
        if (verbose) (*tb_W)->print_dtc(2);

        n = set_mate_score_v<PieceID, _BoardSize, NPIECE>(PieceColor::B, (*tb_B));
        if (verbose) { std::cout << "B (0/1 ply) mate positions:" << n << std::endl; }
        if (verbose) (*tb_B)->print_dtc(2);

        if (NPIECE > 1)
        {
            do
            {
                iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

                (*tb_W)->clear_marker();
                (*tb_B)->clear_marker();

                n = setup_marker_v<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::W, (*tb_W), (*tb_B));
                if (verbose) { std::cout << "W setup_marker_v positions:" << n << std::endl; }

                n = process_marker_v<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::B, (*tb_B), (*tb_W));
                if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

                m = process_marker_v<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::W, (*tb_W), (*tb_B));
                if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }


                m = setup_marker_v<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::B, (*tb_B), (*tb_W));
                if (verbose) { std::cout << "B setup_marker_v positions:" << m << std::endl; }

                n = process_marker_v<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::B, (*tb_B), (*tb_W));
                if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

                m = process_marker_v<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::W, (*tb_W), (*tb_B));
                if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }

                if ((n + m) == 0) break;
            } while (n + m > 0);
        }

        if (verbose) (*tb_W)->print_dtc(NPIECE * 5);
        if (verbose) (*tb_B)->print_dtc(NPIECE * 5);

        (*tb_W)->set_build(true);
        (*tb_B)->set_build(true);
        (*tb_W)->set_unknown_to_draw();
        (*tb_B)->set_unknown_to_draw();
        TablebaseManager<PieceID, _BoardSize>::instance()->add((*tb_W)->name(), (*tb_W));
        TablebaseManager<PieceID, _BoardSize>::instance()->add((*tb_B)->name(), (*tb_B));
        return true;
    }

};
#endif

