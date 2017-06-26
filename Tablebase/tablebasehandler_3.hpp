#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandler_3   : handle 3 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TBHandler_3_HPP
#define _AL_CHESS_TABLEBASE_TBHandler_3_HPP

namespace chess
{
    // TBHandler_3
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_3 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBHandler_3(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t) : TBH(t, ps, 3) {}      
        virtual ~TBHandler_3() 
        {
            for (size_t i = 0; i < _tb_children_info.size(); i++)
            {
                if ((_tb_children_info[i]._owner == true) && (_tb_children_info[i]._tbh != nullptr))
                {
                    if (_tb_children_info[i]._child_index < _tbh_children.size())
                    {
                        delete _tbh_children[_tb_children_info[i]._child_index];
                    }
                }
            }
        }

        bool load() override;
        bool save() const override;
        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }

        Tablebase<PieceID, _BoardSize, 3>* tb_W() const { return _tb_W; }
        Tablebase<PieceID, _BoardSize, 3>* tb_B() const { return _tb_B; }

    protected:
        // Bases TB
        Tablebase<PieceID, _BoardSize, 3>* _tb_W;
        Tablebase<PieceID, _BoardSize, 3>* _tb_B;

        // Childs TBH
        std::vector<STRUCT_TBH<PieceID, _BoardSize>>    _tb_children_info;
        std::vector<TBH<PieceID, _BoardSize>*>          _tbh_children;

        void     print() const;
        bool     build_base(Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo, char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb);
        uint64_t set_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo);
        uint64_t process_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo);
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::save()  const
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
    inline bool TBHandler_3<PieceID, _BoardSize>::load()
    {
        if (is_build()) return true;
        // Lookup
        Tablebase<PieceID, _BoardSize, 3>* tw = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(_pieceSet.name(PieceColor::W));
        Tablebase<PieceID, _BoardSize, 3>* tb = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(_pieceSet.name(PieceColor::B));
        if ((tw != nullptr) && (tb != nullptr))
        {
            if ((tw->_is_build == true) && (tb->_is_build == true))
            {
                _tb_W->set_build(true);
                _tb_B->set_build(true);
                return true;
            }
        }

        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->load();
        }

        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBHandler_3<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::build(char verbose)
    {
        for (size_t i = 0; i < _tbh_children.size(); i++)
        {
            _tbh_children[i]->build(verbose);
        }

        if (verbose)
        {
            std::string str_pieces;
            for (auto& v : _piecesID)str_pieces += _Piece::to_str(v);
            std::cout << "TBHandler_2v1: " << str_pieces << std::endl;
        }

        return build_base(_tb_W, _tb_B, verbose);
    }

    // set_mate_score
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb)
    {
        size_t ret_idx;
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m;

        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);  // reordered for repeating pieces
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            _work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            _work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            _work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                tb->set_dtc(sq0, sq1, sq2, 0);
                tb->set_score(sq0, sq1, sq2, sc);
                tb->set_marker(sq0, sq1, sq2, false);
                n_changes++;
            }
            else if (_work_board->can_capture_opposite_king(m, ret_idx))
            {
                if (_work_board->get_color() == PieceColor::W)
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
        return n_changes;
    }

    // set_marker
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::set_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 3>* tb, Tablebase<PieceID, _BoardSize,3>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m_child;
        std::vector<ExactScore> child_sc;
        std::vector<uint8_t>    child_dtc;
        uint16_t child_sq0; uint16_t child_sq1; uint16_t child_sq2;
        bool exist_child_score;

        bool isPromo;
        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;
            sc = tb->score(sq0, sq1, sq2);
            if (sc != ExactScore::UNKNOWN) continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            _work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            _work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            _work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            m_child = _work_board->generate_moves();
            child_sc.clear();
            child_dtc.clear();
            for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
            for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);

            exist_child_score = false;
            for (size_t j = 0; j < m_child.size(); j++)
            {
                _work_board->apply_move(m_child[j]);
                isPromo = _work_board->is_last_move_promo();
                if (isPromo)
                {
                    int debug = 1;
                }
                if ((_work_board->cnt_all_piece() == tb->_NPIECE) && !isPromo)
                {
                    child_sq0 = _work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                    child_sq1 = _work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color(), true);
                    child_sq2 = _work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                    tb->order_sq_3(child_sq0, child_sq1, child_sq2);
                    assert(!((child_sq0 == child_sq1) || (child_sq0 == child_sq2) || (child_sq1 == child_sq2)));
                    child_sc[j] = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                    child_dtc[j] = tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                }
                else
                {
                    // child in other tb
                    if (find_score_children_tb(*_work_board, _work_board->get_color(), isPromo, sc))
                    {
                        child_sc[j] = sc;
                        child_dtc[j] = 0;
                    }
                    else
                    {
                        int debug = 1;
                        assert(false);
                    }
                }
                if (child_sc[j] != ExactScore::UNKNOWN) exist_child_score = true;
                _work_board->undo_move();
            }

            sc = tb->score(sq0, sq1, sq2);
            if (sc == ExactScore::UNKNOWN)
            {
                uint8_t ret_dtc; size_t ret_idx;
                sc = minmax_dtc(tb->color(), child_sc, child_dtc, ret_dtc, ret_idx);
                if (sc != ExactScore::UNKNOWN)
                {
                    tb->set_dtc(sq0, sq1, sq2, 1 + ret_dtc);
                    tb->set_score(sq0, sq1, sq2, sc);
                    tb->set_marker(sq0, sq1, sq2, false);
                    n_changes++;
                }
                else if (exist_child_score)
                {
                    // mark parent for later re-visit
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

    // process_marker
    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_3<PieceID, _BoardSize>::process_marker(PieceColor color_to_play, Tablebase<PieceID, _BoardSize,3>* tb, Tablebase<PieceID, _BoardSize, 3>* tb_oppo)
    {
        uint64_t n_changes = 0;
        ExactScore sc;
        uint16_t sq0, sq1, sq2;
        std::vector<Move<PieceID>> m;

        uint16_t child_sq0; uint16_t child_sq1; uint16_t child_sq2;
        std::vector<Move<PieceID>> m_child;
        std::vector<ExactScore> child_sc;
        std::vector<uint8_t>    child_dtc;

        bool isPromo;
        for (uint64_t i = 0; i < tb->_size_tb; i++)
        {
            tb->square_at_index(i, sq0, sq1, sq2);
            if ((sq0 == sq1) || (sq0 == sq2) || (sq1 == sq2)) continue;
            if (tb->marker(sq0, sq1, sq2) == false)
                continue;

            _work_board->clear();
            _work_board->set_color(color_to_play);
            _work_board->set_pieceid_at(tb->_piecesID[0], sq0);
            _work_board->set_pieceid_at(tb->_piecesID[1], sq1);
            _work_board->set_pieceid_at(tb->_piecesID[2], sq2);

            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
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
                m_child = _work_board->generate_moves();
                child_sc.clear();
                child_dtc.clear();
                for (size_t j = 0; j < m_child.size(); j++) child_sc.push_back(ExactScore::UNKNOWN);
                for (size_t j = 0; j < m_child.size(); j++) child_dtc.push_back(0);

                for (size_t j = 0; j < m_child.size(); j++)
                {
                    _work_board->apply_move(m_child[j]);
                    isPromo = _work_board->is_last_move_promo();
                    if ((_work_board->cnt_all_piece() == tb->_NPIECE) && !isPromo)
                    {
                        child_sq0 = _work_board->get_square_ofpiece(tb->_pieces[0]->get_name(), tb->_pieces[0]->get_color());
                        child_sq1 = _work_board->get_square_ofpiece(tb->_pieces[1]->get_name(), tb->_pieces[1]->get_color(), true);
                        child_sq2 = _work_board->get_square_ofpiece(tb->_pieces[2]->get_name(), tb->_pieces[2]->get_color());
                        tb->order_sq_3(child_sq0, child_sq1, child_sq2);
                        assert(!((child_sq0 == child_sq1) || (child_sq0 == child_sq2) || (child_sq1 == child_sq2)));
                        child_sc[j] = tb_oppo->score(child_sq0, child_sq1, child_sq2);
                        child_dtc[j] = tb_oppo->dtc(child_sq0, child_sq1, child_sq2);
                    }
                    else
                    {
                        if (find_score_children_tb(*_work_board, _work_board->get_color(), isPromo, sc))
                        {
                            child_sc[j] = sc;
                            child_dtc[j] = 0;
                        }
                        else
                        {
                            int debug = 1;
                            assert(false);
                        }
                    }
                    _work_board->undo_move();
                }

                uint8_t ret_dtc; size_t ret_idx;
                sc = minmax_dtc(tb->color(), child_sc, child_dtc, ret_dtc, ret_idx);
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

    // build_base
    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_3<PieceID, _BoardSize>::build_base(Tablebase<PieceID, _BoardSize, 3>* _tb_W, Tablebase<PieceID, _BoardSize, 3>* _tb_B, char verbose)
    {
        if (is_build()) return true;
        // Lookup
        Tablebase<PieceID, _BoardSize, 3>* tw = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(_pieceSet.name(PieceColor::W));
        Tablebase<PieceID, _BoardSize, 3>* tb = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(_pieceSet.name(PieceColor::B));
        if ((tw != nullptr) && (tb != nullptr))
        {
            if ((tw->_is_build == true) && (tb->_is_build == true))
            {
                _tb_W->set_build(true);
                _tb_B->set_build(true);
                return true;
            }
        }

        uint64_t n = 0;
        uint64_t m = 0;
        int iter = 0;

        if (verbose) { std::cout << TB_TYPE_to_string(tb_type()) << " " << _pieceSet.name(PieceColor::W) << _pieceSet.name(PieceColor::B) << " scanning mate in (0/1) ply ..." << std::endl; }

        n = set_mate_score(PieceColor::W, _tb_W);
        if (verbose) { std::cout << "W (0/1 move) mate positions:" << n << std::endl; }
        if (verbose) _tb_W->print_dtc(2);

        n = set_mate_score(PieceColor::B, _tb_B);
        if (verbose) { std::cout << "B (0/1 move) mate positions:" << n << std::endl; }
        if (verbose) _tb_B->print_dtc(2);

        do
        {
            iter++;  if (verbose) { std::cout << "Iteration: " << iter << std::endl; }

            _tb_W->clear_marker();
            _tb_B->clear_marker();

            n = set_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W set_marker positions:" << n << std::endl; }

            n = process_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

            m = process_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }


            m = set_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B set_marker positions:" << m << std::endl; }

            n = process_marker(PieceColor::B, _tb_B, _tb_W);
            if (verbose) { std::cout << "B process_marker positions:" << n << std::endl; }

            m = process_marker(PieceColor::W, _tb_W, _tb_B);
            if (verbose) { std::cout << "W process_marker positions:" << m << std::endl; }

            if ((n + m) == 0) break;
        } while (n + m > 0);

        if (verbose) _tb_W->print_dtc(20);
        if (verbose) _tb_B->print_dtc(20);

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

