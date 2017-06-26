#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandler_1   : handle 1 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TBHandler_1_HPP
#define _AL_CHESS_TABLEBASE_TBHandler_1_HPP

namespace chess
{
    // TBHandler_1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_1 : public TBH<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TBHandler_1(const PieceSet<PieceID, _BoardSize>& ps, TB_TYPE t) : TBH(t, ps, 1)
        {
            Tablebase<PieceID, _BoardSize, 1>* tb_w = TablebaseManager<PieceID, _BoardSize>::instance()->find_1(ps.name(PieceColor::W));
            Tablebase<PieceID, _BoardSize, 1>* tb_b = TablebaseManager<PieceID, _BoardSize>::instance()->find_1(ps.name(PieceColor::B));
            std::vector<PieceID> v = PieceSet<PieceID, _BoardSize>::ps_to_pieces(ps);

            if (t == TB_TYPE::tb_0vX)
            {
                if (tb_w == nullptr)
                {
                    _tb_W = new Tablebase_0vX<PieceID, _BoardSize>(v, PieceColor::W);       // W to play
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
                } 
                else { _tb_W = tb_w; }

                if (tb_b == nullptr)
                {
                    _tb_B = new Tablebase_0vX<PieceID, _BoardSize>(v, PieceColor::B);       // B to play
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
                } 
                else { _tb_B = tb_b; }
            }
            else
            {
                if (tb_w == nullptr)
                {
                    _tb_W = new Tablebase_Xv0<PieceID, _BoardSize>(v, PieceColor::W);
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::W), _tb_W);
                } 
                else { _tb_W = tb_w; }

                if (tb_b == nullptr)
                {
                    _tb_B = new Tablebase_Xv0<PieceID, _BoardSize>(v, PieceColor::B);
                    TablebaseManager<PieceID, _BoardSize>::instance()->add(ps.name(PieceColor::B), _tb_B);
                } 
                else { _tb_B = tb_b; }
            }
        }

        virtual ~TBHandler_1() {}

        Tablebase<PieceID, _BoardSize, 1>* tb_W()   const { return _tb_W; }
        Tablebase<PieceID, _BoardSize, 1>* tb_B()   const { return _tb_B; }

        bool build(char verbose = 0) override;
        bool is_build() const override { return _tb_W->is_build() && _tb_B->is_build(); }
        bool load() override;
        bool save() const override;
        void print() const;

    protected:;
        Tablebase<PieceID, _BoardSize, 1>* _tb_W;
        Tablebase<PieceID, _BoardSize, 1>* _tb_B;

        bool     build_base(Tablebase<PieceID, _BoardSize, 1>* tb, Tablebase<PieceID, _BoardSize, 1>* tb_oppo, char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 1>* tb);

        bool find_score_children_tb(const _Board& pos, PieceColor color, bool isPromo, ExactScore& ret_sc) const override
        { 
            // No children tb exist
            ret_sc = ExactScore::UNKNOWN;
            return false; 
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::save()  const
    {
        if (!_tb_W->save()) return false;
        if (!_tb_B->save()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::load()
    {
        if (is_build()) return true;
        if (!_tb_W->load()) return false;
        if (!_tb_B->load()) return false;
        return true;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::build(char verbose)
    {
        return build_base(_tb_W, _tb_B, verbose);
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBHandler_1<PieceID, _BoardSize>::print() const
    {
        _tb_W->print();
        _tb_B->print();
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline uint64_t TBHandler_1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 1>* tb)
    {
        size_t ret_idx;
        uint64_t n_changes = 0;
        ExactScore sc;
        std::vector<Move<PieceID>> m;

        for (uint16_t sq0 = 0; sq0 < tb->_dim1; sq0++)
        {
            _work_board->clear();
            _work_board->set_color(color_to_play);
            _work_board->set_pieceid_at(tb->_piecesID[0], sq0);

            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                tb->set_dtc(sq0, 0);
                tb->set_score(sq0, sc);
                tb->set_marker(sq0, false);
                n_changes++;
            }
            else if (_work_board->can_capture_opposite_king(m, ret_idx))
            {
                if (_work_board->get_color() == PieceColor::W)
                {
                    tb->set_dtc(sq0,  1);
                    tb->set_score(sq0, ExactScore::WIN);
                    tb->set_marker(sq0,  false);
                    n_changes++;
                }
                else
                {
                    tb->set_dtc(sq0, 1);
                    tb->set_score(sq0, ExactScore::LOSS);
                    tb->set_marker(sq0,  false);
                    n_changes++;
                }
            }
        }
        return n_changes;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    inline bool TBHandler_1<PieceID, _BoardSize>::build_base(Tablebase<PieceID, _BoardSize, 1>* _tb_W, Tablebase<PieceID, _BoardSize, 1>* _tb_B, char verbose)
    {
        if (is_build()) return true;

        uint64_t n = 0;
        if (verbose) { std::cout << TB_TYPE_to_string(tb_type()) << " " << _pieceSet.name(PieceColor::W) << _pieceSet.name(PieceColor::B) << " scanning mate in (0/1) ply ..." << std::endl; }

        n = set_mate_score(PieceColor::W, _tb_W);
        if (verbose) { std::cout << "W (0/1 move) mate positions:" << n << std::endl; }
        if (verbose) _tb_W->print_dtc(2);

        n = set_mate_score(PieceColor::B, _tb_B);
        if (verbose) { std::cout << "B (0/1 move) mate positions:" << n << std::endl; }
        if (verbose) _tb_B->print_dtc(2);

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

