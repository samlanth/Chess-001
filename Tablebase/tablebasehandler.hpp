#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseBaseHandler
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_HPP

namespace chess
{
    // TablebaseBaseHandler
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseBaseHandler
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TablebaseBaseHandler(std::vector<PieceID>& v) : _piecesID(v)
        {
        }
        virtual ~TablebaseBaseHandler()
        {
        }

        virtual bool load() = 0;
        virtual bool save() const = 0;
        virtual bool build(char verbose) = 0;
        virtual bool is_build() const = 0;

        virtual bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const = 0;

    protected:
        std::vector<PieceID> _piecesID;

        ExactScore best_score(PieceColor color_child, ExactScore sc, ExactScore best_score, bool& is_same, bool& is_better) const;

        ExactScore TablebaseBaseHandler<PieceID, _BoardSize>::minmax_dtc(
            PieceColor                  color_parent,
            std::vector<ExactScore>&    child_sc,
            std::vector<uint8_t>&       child_dtc,
            uint8_t&                    ret_dtc) const;

        uint64_t set_mate_score_3(  PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb);
        uint64_t set_marker_3(      PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo);
        uint64_t process_marker_3(  PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo);

    };

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseBaseHandler<PieceID, _BoardSize>::best_score(PieceColor color_child, ExactScore sc, ExactScore best_score,
                                    bool& is_same, bool& is_better) const
    {
        is_same     = false;
        is_better   = false;

        if (color_child == PieceColor::B) // W at parent
        {
            if (sc == ExactScore::WIN)
            {
                if (best_score == ExactScore::WIN) is_same = true;
                else is_better = true;

                return ExactScore::WIN;
            }
            else if (sc == ExactScore::DRAW)
            {
                if (best_score != ExactScore::WIN)
                {
                    if (best_score == ExactScore::DRAW) is_same = true;
                    else is_better = true;

                    return ExactScore::DRAW;
                }
            }
            else
            {
                // sc == ExactScore::LOSS
                if (best_score == ExactScore::UNKNOWN)
                {
                    //is_better = true;
                    //return ExactScore::LOSS;
                    return ExactScore::UNKNOWN;
                }
            }
        }
        else
        {
            if (sc == ExactScore::LOSS)
            {
                if (best_score == ExactScore::LOSS) is_same = true;
                else is_better = true;

                return ExactScore::LOSS;
            }
            else if (sc == ExactScore::DRAW)
            {
                if (best_score != ExactScore::LOSS)
                {
                    if (best_score == ExactScore::DRAW) is_same = true;
                    else is_better = true;

                    return ExactScore::DRAW;
                }
            }
            else
            {
                // sc == ExactScore::WIN
                if (best_score == ExactScore::UNKNOWN)
                {
                    //is_better = true;
                    //return ExactScore::WIN;
                    return ExactScore::UNKNOWN;
                }
            }
        }
        return best_score;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseBaseHandler<PieceID, _BoardSize>::minmax_dtc(
        PieceColor                  color_parent,
        std::vector<ExactScore>&    child_sc,
        std::vector<uint8_t>&       child_dtc,
        uint8_t&                    ret_dtc) const
    {
        if (child_sc.size() == 0)
        {
            ret_dtc = 0;
            return ExactScore::UNKNOWN;
        }

        if (color_parent == PieceColor::W)
        {
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
                    return ExactScore::WIN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::UNKNOWN)
                {
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
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
                    // seek lower dtc
                    uint8_t min_dtc = child_dtc[i];
                    size_t  min_idx = i;
                    for (size_t j = 0; j < child_sc.size(); j++)
                    {
                        if (child_sc[j] == ExactScore::LOSS)
                            if (child_dtc[j] < min_dtc) { min_dtc = child_dtc[j]; min_idx = j; }
                    }
                    ret_dtc = child_dtc[min_idx];
                    return ExactScore::LOSS;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::UNKNOWN)
                {
                    ret_dtc = child_dtc[i];
                    return ExactScore::UNKNOWN;
                }
            }
            for (size_t i = 0; i < child_sc.size(); i++)
            {
                if (child_sc[i] == ExactScore::DRAW)
                {
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
                    return ExactScore::WIN;
                }
            }
        }
        ret_dtc = 0;
        return ExactScore::UNKNOWN;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler<PieceID, _BoardSize>::set_mate_score_3(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb)
    {
        size_t ret_idx;
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
                            tb->set_dtc(sq0, sq1, sq2, 0);
                            tb->set_score(sq0, sq1, sq2, sc);
                            tb->set_marker(sq0, sq1, sq2, false);
                            n_changes++;
                        }
                        else if (tb->_work_board->can_capture_opposite_king(m, ret_idx))
                        {
                            if (tb->_work_board->get_color() == PieceColor::W)
                            {
                                tb->set_dtc(sq0, sq1, sq2, 1);
                                tb->set_score(sq0, sq1, sq2, ExactScore::WIN);
                                tb->set_marker(sq0, sq1, sq2, false);
                                n_changes++;
                            }
                            else
                            {
                                tb->set_dtc(sq0, sq1, sq2, 1);
                                tb->set_score(sq0, sq1, sq2, ExactScore::LOSS);
                                tb->set_marker(sq0, sq1, sq2, false);
                                n_changes++;
                            }
                        }
                    }
                }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler<PieceID, _BoardSize>::set_marker_3(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize,3>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m_child;
        std::vector<ExactScore> child_sc;
        std::vector<uint8_t>    child_dtc;
        uint16_t child_sq0; uint16_t child_sq1; uint16_t child_sq2;
        bool exist_child_score;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;
            sc = tb->score(sq0, sq1, sq2);
            if (sc != ExactScore::UNKNOWN) continue;

            tb->_work_board->clear();
            tb->_work_board->set_color(color_to_play);
            tb->_work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            tb->_work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            tb->_work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            m_child = tb->_work_board->generate_moves();
            child_sc.clear();
            child_dtc.clear();
            for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
            for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);

            exist_child_score = false;
            for (size_t j = 0; j < m_child.size(); j++)
            {
                tb->_work_board->apply_move(m_child[j]);
                if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
                {
                    child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                    child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                    child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                    child_sc[j] = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                    child_dtc[j] = tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                }
                else
                {
                    // child in other tb
                    if (find_score_children_tb(*tb->_work_board, tb->_work_board->get_color(), sc))
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
                tb->_work_board->undo_move();
            }

            sc = tb->score(sq0, sq1, sq2);
            if (sc == ExactScore::UNKNOWN)
            {
                uint8_t ret_dtc;
                sc = minmax_dtc(tb->color(), child_sc, child_dtc, ret_dtc);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, sq2, 1 + ret_dtc);
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
                else if (exist_child_score)
                {
                    // mark parent
                    if (tb->marker(sq0, sq1, sq2) == false)
                    {
                        tb->set_marker(sq0, sq1, sq2, true);
                    }
                    n_changes++;
                }
            }
        }
        return n_changes;
    }


    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler<PieceID, _BoardSize>::process_marker_3(PieceColor color_to_play, Tablebase<PieceID, _BoardSize,3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m;

        uint16_t child_sq0; uint16_t child_sq1; uint16_t child_sq2;
        std::vector<Move<PieceID>> m_child;
        std::vector<ExactScore> child_sc;
        std::vector<uint8_t>    child_dtc;

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
                if (tb->score(sq0, sq1, sq2) == ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, sq2, 0);
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
            }
            else if (tb->score(sq0, sq1, sq2) == ExactScore::UNKNOWN)
            {
                m_child = tb->_work_board->generate_moves();
                child_sc.clear();
                child_dtc.clear();
                for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
                for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);

                for (size_t j = 0; j < m_child.size(); j++)
                {
                    tb->_work_board->apply_move(m_child[j]);
                    if (tb->_work_board->cnt_all_piece() == tb->_NPIECE)
                    {
                        child_sq0 = tb->_work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                        child_sq1 = tb->_work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color());
                        child_sq2 = tb->_work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                        child_sc[j] = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                        child_dtc[j] = tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                    }
                    else
                    {
                        if (find_score_children_tb(*tb->_work_board, tb->_work_board->get_color(), sc))
                        {
                            child_sc[j] = sc;
                            child_dtc[j] = 0;
                        }
                    }
                    tb->_work_board->undo_move();
                }

                uint8_t ret_dtc;
                sc = minmax_dtc(tb->color(), child_sc, child_dtc, ret_dtc);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, sq2, 1 + ret_dtc);
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }
};
#endif

