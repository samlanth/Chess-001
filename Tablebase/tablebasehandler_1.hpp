#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseBaseHandler_1   : handle 1 pieces TB
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_1_HPP

namespace chess
{
    // TablebaseBaseHandler_1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseBaseHandler_1 : public TablebaseBaseHandlerCore<PieceID, _BoardSize>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TablebaseBaseHandler_1(std::vector<PieceID>& v) : TablebaseBaseHandlerCore(v)
        {
        }
        virtual ~TablebaseBaseHandler_1()
        {
        }

    protected:
        bool     build_base(Tablebase<PieceID, _BoardSize, 1>* tb, Tablebase<PieceID, _BoardSize, 1>* tb_oppo, char verbose = 0);
        uint64_t set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 1>* tb);

        bool     find_score_children_tb(const _Board& pos, PieceColor color, ExactScore& ret_sc) const override { return false; }
    };


    template <typename PieceID, typename uint8_t _BoardSize>
    uint64_t TablebaseBaseHandler_1<PieceID, _BoardSize>::set_mate_score(PieceColor color_to_play, Tablebase<PieceID, _BoardSize, 1>* tb)
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
    bool TablebaseBaseHandler_1<PieceID, _BoardSize>::build_base(Tablebase<PieceID, _BoardSize, 1>* _tb_W, Tablebase<PieceID, _BoardSize, 1>* _tb_B, char verbose)
    {
        uint64_t n = 0;
        if (verbose) { std::cout << "scanning mate in (0/1) ply ..." << std::endl; }

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

