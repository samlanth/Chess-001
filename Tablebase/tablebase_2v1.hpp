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
    template <typename PieceID, typename uint8_t _BoardSize> class Tablebase_2v0;

    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_2v1 : public Tablebase<PieceID, _BoardSize, 3>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;
        friend class TablebaseBaseHandler_3<PieceID, _BoardSize>;

    public:
        Tablebase_2v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 3>(v, c)
        {
        }

        bool load() override
        {
            _is_build = read_tb();
            if (_is_build) TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);
            return _is_build;
        }

        bool save() const override { return save_tb(); }
        bool build(char verbose = 0) override { return false; }
        bool isPiecesMatch(const _Board& pos) override;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_2v1<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 3) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[1])->get_name(), PieceColor::W)) &&
            (pos.has_piece(_Piece::get(_piecesID[2])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }

    // TablebaseHandler_2v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseHandler_2v1 : public TablebaseBaseHandler_3<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TablebaseHandler_2v1(std::vector<PieceID>& v) : TablebaseBaseHandler_3<PieceID, _BoardSize>(v)
        {
            _tb_W = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::W);
            _tb_B = new Tablebase_2v1<PieceID, _BoardSize>(v, PieceColor::B);

            _p_2v0.push_back(v[0]);     _p_2v0.push_back(v[1]);
            _p0_1v1.push_back(v[0]);    _p0_1v1.push_back(v[2]);
            _p1_1v1.push_back(v[1]);    _p1_1v1.push_back(v[2]);

            // children TB
            _tbh_0_2v0  = new TablebaseHandler_2v0<PieceID, _BoardSize>(_p_2v0);
            _tbh_0_1v1  = new TablebaseHandler_1v1<PieceID, _BoardSize>(_p0_1v1);
            _tbh_1_1v1  = new TablebaseHandler_1v1<PieceID, _BoardSize>(_p1_1v1);
        }

        ~TablebaseHandler_2v1()
        {
            delete _tb_W;
            delete _tb_B;

            delete _tbh_0_2v0;
            delete _tbh_0_1v1;
            delete _tbh_1_1v1;;
        }

        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;

        Tablebase_2v1<PieceID, _BoardSize>* tb_W() { return _tb_W; }
        Tablebase_2v1<PieceID, _BoardSize>* tb_B() { return _tb_B; }

    protected:      
        bool find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const override;
        Tablebase_2v0<PieceID, _BoardSize>* locate_children_2v0(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const;
        Tablebase_1v1<PieceID, _BoardSize>* locate_children_1v1(const _Board& pos, PieceColor c, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const;
 
        void print_pos_with_dtc_score(uint8_t value_dtc, ExactScore value_sc) const;
        void print() const;

        std::vector<PieceID>                _piecesID;
        Tablebase_2v1<PieceID, _BoardSize>* _tb_W;
        Tablebase_2v1<PieceID, _BoardSize>* _tb_B;

        std::vector<PieceID> _p_2v0;
        std::vector<PieceID> _p0_1v1;
        std::vector<PieceID> _p1_1v1;

        // children TB
        TablebaseHandler_2v0<PieceID, _BoardSize>* _tbh_0_2v0;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_0_1v1;
        TablebaseHandler_1v1<PieceID, _BoardSize>* _tbh_1_1v1;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;
        if (!_tbh_0_2v0->save()) return false;
        if (!_tbh_0_1v1->save()) return false;
        if (!_tbh_1_1v1->save()) return false;
        return true;
    }
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::load()
    {
        if (!_tbh_0_2v0->load()) return false;
        if (!_tbh_0_1v1->load()) return false;
        if (!_tbh_1_1v1->load()) return false;
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::build(char verbose)
    {
        _tbh_0_2v0->build(verbose);
        _tbh_0_1v1->build(verbose);
        _tbh_1_1v1->build(verbose);

        assert(_tbh_0_2v0->is_build() == true);
        assert(_tbh_0_1v1->is_build() == true);
        assert(_tbh_1_1v1->is_build() == true);

        std::string str_pieces;
        for (auto& v : _piecesID )str_pieces += _Piece::to_str(v);
        if (verbose) { std::cout << "TablebaseHandler_2v1: " << str_pieces << std::endl; }
        
        return build_base(_tb_W, _tb_B, verbose);        
    }

    // find_score_children_tb
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseHandler_2v1<PieceID, _BoardSize>::find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const
    {
        uint16_t ret_child_sq0; uint16_t ret_child_sq1;
        Tablebase_1v1<PieceID, _BoardSize>* tb_1v1 = locate_children_1v1(pos, color, ret_child_sq0, ret_child_sq1);
        if (tb_1v1 != nullptr)
        {
            ret_sc = tb_1v1->score(ret_child_sq0, ret_child_sq1);
            return true;
        }
        Tablebase_2v0<PieceID, _BoardSize>* tb_2v0 = locate_children_2v0(pos, color, ret_child_sq0, ret_child_sq1);
        if (tb_2v0 != nullptr)
        {
            ret_sc = tb_2v0->score(ret_child_sq0, ret_child_sq1);
            return true;
        }
        return false;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_1v1<PieceID, _BoardSize>* TablebaseHandler_2v1<PieceID, _BoardSize>::locate_children_1v1(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const
    {
        if ((color == PieceColor::W) && (_tbh_0_1v1->_tb_W->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[1])->get_name(), PieceColor::B);
            return _tbh_0_1v1->_tb_W;
        }
        if ((color == PieceColor::B) && (_tbh_0_1v1->_tb_B->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p0_1v1[1])->get_name(), PieceColor::B);
            return _tbh_0_1v1->_tb_B;
        }

        if ((color == PieceColor::W) && (_tbh_1_1v1->_tb_W->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[1])->get_name(), PieceColor::B);
            return _tbh_1_1v1->_tb_W;
        }
        if ((color == PieceColor::B) && (_tbh_1_1v1->_tb_B->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p1_1v1[1])->get_name(), PieceColor::B);
            return _tbh_1_1v1->_tb_B;
        }
        return nullptr;
    }

    // locate_children
    template <typename PieceID, typename uint8_t _BoardSize>
    Tablebase_2v0<PieceID, _BoardSize>* TablebaseHandler_2v1<PieceID, _BoardSize>::locate_children_2v0(const _Board& pos, PieceColor color, uint16_t& ret_child_sq0, uint16_t& ret_child_sq1) const
    {
        if ((color == PieceColor::W) && (_tbh_0_2v0->_tb_W->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_2v0[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p_2v0[1])->get_name(), PieceColor::W);
            return _tbh_0_2v0->_tb_W;
        }
        if ((color == PieceColor::B) && (_tbh_0_2v0->_tb_B->isPiecesMatch(pos)))
        {
            ret_child_sq0 = pos.get_square_ofpiece(_Piece::get(_p_2v0[0])->get_name(), PieceColor::W);
            ret_child_sq1 = pos.get_square_ofpiece(_Piece::get(_p_2v0[1])->get_name(), PieceColor::W);
            return _tbh_0_2v0->_tb_B;
        }
        return nullptr;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void TablebaseHandler_2v1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    void TablebaseHandler_2v1<PieceID, _BoardSize>::print_pos_with_dtc_score(uint8_t value_dtc, ExactScore value_sc) const
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

