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
    // minmax_dtc
    template <typename PieceID, typename uint8_t _BoardSize>
    inline ExactScore minmax_dtc(   PieceColor color_parent,
                                    const std::vector<ExactScore>&  child_sc,
                                    const std::vector<uint8_t>&     child_dtc, 
                                    std::vector<bool>&              child_is_promo,
                                    std::vector<bool>&              child_is_capture,
                                    std::vector<bool>&              child_is_pawn,
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
                    if (child_is_promo[i] || child_is_capture[i]) // 1 move promo/capture and win
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
                    ret_dtc = child_dtc[i]; //...max limit...
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                // draw
                if (child_sc[i] == ExactScore::DRAW)
                {
                    bool init = false;
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        // seek a capture or pawn move
                        if ((child_sc[j] == ExactScore::DRAW) && ((child_is_capture[j] == true)|| (child_is_pawn[j] == true)))
                        {
                            if (init == false)
                            {
                                min_dtc = child_dtc[j]; 
                                min_idx = j;
                                init = true;
                            }
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                        }
                    }
                    ret_dtc = child_dtc[min_idx];
                    ret_idx = min_idx;
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
                    if (child_is_promo[i] || child_is_capture[i]) // 1 move promo/capture and loss
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
                    // seek a capture or pawn move
                    bool init = false;
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if ((child_sc[j] == ExactScore::DRAW) && ((child_is_capture[j] == true) || (child_is_pawn[j] == true)))
                        {
                            if (init == false)
                            {
                                min_dtc = child_dtc[j];
                                min_idx = j;
                                init = true;
                            }
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                        }
                    }
                    ret_dtc = child_dtc[min_idx];
                    ret_idx = min_idx;
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
    uint64_t set_mate_score_vv(PieceColor color_to_play, TablebaseBase<PieceID, _BoardSize>* tb)
    {
        return  set_mate_score_v(color_to_play, (Tablebase<PieceID, _BoardSize, NPIECE>*)(tb));
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t set_mate_score_v(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb)
    {
        size_t      ret_idx;
        uint64_t    n_changes = 0;
        ExactScore  sc;
        bool        legal_pos;
        std::vector<uint16_t> sq;
        std::vector<Move<PieceID>> m;

        for (size_t z = 0; z < NPIECE; z++) sq.push_back(0);
        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();

        for (uint64_t i = 0; i < tb->size_tb(); i++)
        {
            tb->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);

            legal_pos = true;
            for (size_t z = 0; z < NPIECE; z++)
            {
                // square where pieces are must be unique
                if (std::count(sq.begin(), sq.end(), sq[z]) > 1) { legal_pos = false; break; }
            }
            if (!legal_pos) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            for (size_t z = 0; z < NPIECE; z++)  _work_board->set_pieceid_at(tb->_piecesID[z], sq[z]);

            // legal position
            if (!_work_board->legal_pos()) 
                continue;

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
                tb->set_dtc_v(sq, 1);
                tb->set_score_v(sq, (_work_board->get_color() == PieceColor::W) ? ExactScore::WIN : ExactScore::LOSS);
                tb->set_marker_v(sq, false);
                n_changes++;
            }
        }
        delete _work_board;
        return n_changes;
    };

    // setup_marker  - template function
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t setup_marker_vv(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, TablebaseBase<PieceID, _BoardSize>* tb, TablebaseBase<PieceID, _BoardSize>* tb_oppo)
    {
        return  setup_marker_v(tbh, color_to_play, (Tablebase<PieceID, _BoardSize, NPIECE>*)(tb), (Tablebase<PieceID, _BoardSize, NPIECE>*)(tb_oppo));
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    uint64_t setup_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo)
    {
        ExactScore  sc;
        uint8_t     dtc;
        std::vector<uint16_t> sq;
        std::vector<Move<PieceID>>  m_child;
        std::vector<ExactScore>     child_sc;
        std::vector<uint8_t>        child_dtc;
        std::vector<bool>           child_is_promo;
        std::vector<bool>           child_is_capture;
        std::vector<bool>           child_is_pawn;
        std::vector<uint16_t>       child_sq;
        bool        exist_child_score;
        bool        isPromo;
        bool        isCapture;
        bool        isPawn;
        bool        legal_pos;
        uint64_t    n_changes = 0;

        struct STRUCT_PIECE_RANK
        {
            PieceID     ret_id;
            uint16_t    ret_count;
            uint16_t    rank_ret_id;
            uint16_t    ret_instance;
        };
        assert(tb->_NPIECE == NPIECE);

        // tb->_pieces[z] is at ret_instance in the board    
        std::map<size_t, STRUCT_PIECE_RANK> map_piece_rank;
        PieceSet<PieceID, _BoardSize> piece_set(tb->_pieces);
        for (size_t z = 0; z < NPIECE; z++)
        {
            STRUCT_PIECE_RANK r;
            r.rank_ret_id = piece_set.find_rank_index(z, r.ret_id, r.ret_count, r.ret_instance);
            if (r.ret_count > 0)
            {
                map_piece_rank[z] = r;
            }
            else
            {
                //  not reachable
                assert(false);
            }
        }

        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();
        for (size_t z = 0; z < NPIECE; z++) { sq.push_back(0); child_sq.push_back(0); }

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);

            legal_pos = true;
            for (size_t z = 0; z < NPIECE; z++)
            {
                if (std::count(sq.begin(), sq.end(), sq[z]) > 1) { legal_pos = false; break; }
            }
            if (!legal_pos) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            for (size_t z = 0; z < NPIECE; z++)  _work_board->set_pieceid_at(tb->_piecesID[z], sq[z]);
            if (!_work_board->legal_pos()) 
                continue;

            sc = tb->score_v(sq);
            if (sc != ExactScore::UNKNOWN) continue;

            m_child = _work_board->generate_moves();
            child_sc.clear();
            child_dtc.clear();
            child_is_promo.clear();
            child_is_capture.clear();
            child_is_pawn.clear();
            for (size_t j = 0; j < m_child.size(); j++) 
            { 
                child_sc.push_back(ExactScore::UNKNOWN); 
                child_dtc.push_back(0); 
                child_is_promo.push_back(false); 
                child_is_capture.push_back(false);
                child_is_pawn.push_back(false);
            }

            exist_child_score = false;
            for (size_t j = 0; j < m_child.size(); j++)
            {
                _work_board->apply_move(m_child[j]);
                isPromo = _work_board->is_last_move_promo();
                isCapture = _work_board->is_last_move_capture();
                isPawn = _work_board->is_last_move_pawn();

                if ((_work_board->cnt_all_piece() == tb->_NPIECE) && !isPromo) // exact same pieces as parent
                {
                    for (size_t z = 0; z < NPIECE; z++)
                        child_sq[z] = _work_board->get_square_ofpiece_instance(Piece<PieceID, _BoardSize>::get(map_piece_rank[z].ret_id)->get_name(), Piece<PieceID, _BoardSize>::get(map_piece_rank[z].ret_id)->get_color(), map_piece_rank[z].ret_instance);

                    // Set an ordering of repeating position in the TB
                    tb->order_sq_v(child_sq);

                    legal_pos = true;
                    for (size_t z = 0; z < NPIECE; z++)
                    {
                        // pieces on unique square
                        if (std::count(child_sq.begin(), child_sq.end(), child_sq[z]) > 1) { legal_pos = false; break; }
                    }
                    if (!legal_pos)
                    {
                        //  not reachable
                        assert(false);
                        continue;
                    }


                    child_sc[j]  = tb_oppo->score_v(child_sq);
                    child_dtc[j] = tb_oppo->dtc_v(child_sq);
                    child_is_promo[j] = isPromo;
                    child_is_capture[j] = isCapture;
                    child_is_pawn[j] = isPawn;
                }
                else
                {
                    if (tbh->find_score_children_tb(*_work_board, _work_board->get_color(), isPromo, isCapture, sc, dtc))
                    {
                        child_sc[j] = sc;
                        child_dtc[j] = 0; // dtc;
                        child_is_promo[j] = isPromo;
                        child_is_capture[j] = isCapture;
                        child_is_pawn[j] = isPawn;
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
                sc = minmax_dtc<PieceID, _BoardSize>(tb->color(), child_sc, child_dtc, child_is_promo, child_is_capture, child_is_pawn, ret_dtc, ret_idx);
                if (sc != ExactScore::UNKNOWN)
                {
                    if ((!child_is_promo[ret_idx]) && (child_is_capture[ret_idx]))
                    {
                        int debug = 1;
                        debug++;
                    }
                    if ((!child_is_promo[ret_idx]) && (!child_is_capture[ret_idx]))
                    {
                        tb->set_dtc_v(sq, 1 + ret_dtc);
                    }
                    else
                    {
                        tb->set_dtc_v(sq, 1);
                    }
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
    inline uint64_t process_marker_vv(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, TablebaseBase<PieceID, _BoardSize>* tb, TablebaseBase<PieceID, _BoardSize>* tb_oppo)
    {
        return process_marker_v(tbh, color_to_play, (Tablebase<PieceID, _BoardSize, NPIECE>*)(tb), (Tablebase<PieceID, _BoardSize, NPIECE>*) (tb_oppo));
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    inline uint64_t process_marker_v(TBH<PieceID, _BoardSize>* tbh, PieceColor color_to_play, Tablebase<PieceID, _BoardSize, NPIECE>* tb, Tablebase<PieceID, _BoardSize, NPIECE>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint8_t dtc;
        std::vector<uint16_t> sq;
        std::vector<Move<PieceID>>  m;
        std::vector<Move<PieceID>>  m_child;
        std::vector<ExactScore>     child_sc;
        std::vector<uint8_t>        child_dtc;
        std::vector<bool>           child_is_promo;
        std::vector<bool>           child_is_capture;
        std::vector<bool>           child_is_pawn;
        std::vector<uint16_t>       child_sq;
        bool        isPromo;
        bool        isCapture;
        bool        isPawn;
        bool        legal_pos;

        struct STRUCT_PIECE_RANK
        {
            PieceID     ret_id;
            uint16_t    ret_count;
            uint16_t    rank_ret_id;
            uint16_t    ret_instance;
        };

        std::map<size_t, STRUCT_PIECE_RANK> map_piece_rank;
        PieceSet<PieceID, _BoardSize> piece_set(tb->_pieces);
        for (size_t z = 0; z < NPIECE; z++)
        {
            STRUCT_PIECE_RANK r;
            r.rank_ret_id = piece_set.find_rank_index(z, r.ret_id, r.ret_count, r.ret_instance);
            if (r.ret_count > 0)
            {
                map_piece_rank[z] = r;
            }
            else
            {
                //  not reachable
                assert(false);
            }
        }

        Board<PieceID, _BoardSize>* _work_board = new Board<PieceID, _BoardSize>();
        for (size_t z = 0; z < NPIECE; z++) { sq.push_back(0); child_sq.push_back(0); }

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index_v(i, sq);
            assert(sq.size() == NPIECE);

            legal_pos = true;
            for (size_t z = 0; z < NPIECE; z++)
            {
                if (std::count(sq.begin(), sq.end(), sq[z]) > 1) { legal_pos = false; break; }
            }
            if (!legal_pos) continue;

            if (tb->marker_v(sq) == false) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            for (size_t z = 0; z < NPIECE; z++)  _work_board->set_pieceid_at(tb->_piecesID[z], sq[z]);
            if (!_work_board->legal_pos()) 
                continue;

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
                child_is_capture.clear();
                child_is_pawn.clear();
                for (size_t j = 0; j < m_child.size(); j++) 
                { 
                    child_sc.push_back(ExactScore::UNKNOWN); 
                    child_dtc.push_back(0); 
                    child_is_promo.push_back(false); 
                    child_is_capture.push_back(false);
                    child_is_pawn.push_back(false);
                }
                for (size_t j = 0; j < m_child.size(); j++)
                {
                    _work_board->apply_move(m_child[j]);
                    isPromo = _work_board->is_last_move_promo();
                    isCapture = _work_board->is_last_move_capture();
                    isPawn = _work_board->is_last_move_pawn();

                    if ((_work_board->cnt_all_piece() == tb->_NPIECE) && !isPromo) // same pieces as parent position
                    {
                        for (size_t z = 0; z < NPIECE; z++)
                            child_sq[z] = _work_board->get_square_ofpiece_instance(Piece<PieceID, _BoardSize>::get(map_piece_rank[z].ret_id)->get_name(), Piece<PieceID, _BoardSize>::get(map_piece_rank[z].ret_id)->get_color(), map_piece_rank[z].ret_instance);

                        tb->order_sq_v(child_sq);

                        legal_pos = true;
                        for (size_t z = 0; z < NPIECE; z++)
                        {
                            if (std::count(child_sq.begin(), child_sq.end(), child_sq[z]) > 1) { legal_pos = false; break; }
                        }
                        if (!legal_pos)
                        {
                            //  not reachable
                            assert(false);
                            continue;
                        }

                        child_sc[j]  = tb_oppo->score_v(child_sq);
                        child_dtc[j] = tb_oppo->dtc_v(child_sq);
                        child_is_promo[j] = isPromo;
                        child_is_capture[j] = isCapture;
                        child_is_pawn[j] = isPawn;
                    }
                    else
                    {
                        if (tbh->find_score_children_tb(*_work_board, _work_board->get_color(), isPromo, isCapture, sc, dtc))
                        {
                            child_sc[j] = sc;
                            child_dtc[j] = 0; // dtc;
                            child_is_promo[j] = isPromo;
                            child_is_capture[j] = isCapture;
                            child_is_pawn[j] = isPawn;
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    _work_board->undo_move();
                }

                uint8_t ret_dtc; size_t ret_idx;
                sc = minmax_dtc<PieceID, _BoardSize>(tb->color(), child_sc, child_dtc, child_is_promo, child_is_capture, child_is_pawn, ret_dtc, ret_idx);
                if (sc != ExactScore::UNKNOWN)
                {
                    if ((!child_is_promo[ret_idx]) && (child_is_capture[ret_idx]))
                    {
                        int debug = 1;
                        debug++;
                    }
                    if ((!child_is_promo[ret_idx]) && (!child_is_capture[ret_idx]))
                        tb->set_dtc_v(sq, 1 + ret_dtc);
                    else 
                        tb->set_dtc_v(sq, 1);
                    tb->set_score_v(sq, sc);
                    tb->set_marker_v(sq, false);
                    n_changes++;
                }
            }
        }
        delete _work_board;
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE >
    inline bool build_base_vv(TBH<PieceID, _BoardSize>* tbh, TablebaseBase<PieceID, _BoardSize>* tb_W, TablebaseBase<PieceID, _BoardSize>* tb_B, char verbose)
    {
        if (tbh->is_build()) return true;

        // Lookup in memory
        TablebaseBase<PieceID, _BoardSize>* tw = TB_Manager<PieceID, _BoardSize>::instance()->find(NPIECE, tbh->pieceSet().name(PieceColor::W));
        TablebaseBase<PieceID, _BoardSize>* tb = TB_Manager<PieceID, _BoardSize>::instance()->find(NPIECE, tbh->pieceSet().name(PieceColor::B));
        if ((tw != nullptr) && (tb != nullptr))
        {
            if ((tw->is_build() == true) && (tb->is_build() == true))
            {
                // Replace and quit
                tbh->set_TB_W(tw);
                tbh->set_TB_B(tb);
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

        if (verbose) { std::cout << TB_TYPE_to_string(tbh->tb_type()) << " " << tbh->pieceSet().name(PieceColor::W) << " " << tbh->pieceSet().name(PieceColor::B) << " scanning mate in (0/1) ply ..." << std::endl; }

        n = set_mate_score_vv<PieceID, _BoardSize, NPIECE>(PieceColor::W, tb_W);
        if (verbose) { std::cout << "W (0/1 ply) mate positions:" << n << std::endl; }
        if (verbose) ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->print_dtc(2);

        n = set_mate_score_vv<PieceID, _BoardSize, NPIECE>(PieceColor::B, tb_B);
        if (verbose) { std::cout << "B (0/1 ply) mate positions:" << n << std::endl; }
        if (verbose) ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->print_dtc(2);

        if (NPIECE > 1)
        {
            do
            {
                iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

                ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->clear_marker();
                ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->clear_marker();

                n = setup_marker_vv<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::W, tb_W, tb_B);
                if (verbose) { std::cout << "W setup_marker_v positions:" << n << std::endl; }

                n = process_marker_vv<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::B, tb_B, tb_W);
                if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

                m = process_marker_vv<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::W, tb_W, tb_B);
                if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }


                m = setup_marker_vv<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::B, tb_B, tb_W);
                if (verbose) { std::cout << "B setup_marker_v positions:" << m << std::endl; }

                n = process_marker_vv<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::B, tb_B, tb_W);
                if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

                m = process_marker_vv<PieceID, _BoardSize, NPIECE>(tbh, PieceColor::W, tb_W, tb_B);
                if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }

                if ((n + m) == 0) break;
            } while (n + m > 0);
        }

        if (verbose) ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->print_dtc(NPIECE * 5);
        if (verbose) ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->print_dtc(NPIECE * 5);

        ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->set_build(true);
        ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->set_build(true);
        ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->set_unknown_to_draw();
        ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->set_unknown_to_draw();
        TB_Manager<PieceID, _BoardSize>::instance()->add(((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->name(), ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W));
        TB_Manager<PieceID, _BoardSize>::instance()->add(((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->name(), ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B));
        
        // TEST save
        ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_W)->save();
        ((Tablebase<PieceID, _BoardSize, NPIECE>*)tb_B)->save();

        return true;
    }
};
#endif

