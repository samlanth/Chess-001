#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_1v0
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_1v0_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_1v0_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_1v0 : public Tablebase<PieceID, _BoardSize, 1>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        Tablebase_1v0(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c)
        {
            // no _children
        }
        bool load() override { return true; }
        bool save() override { return true; }
        bool build(char verbose = 0) override;
    };

 
    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_1v0<PieceID, _BoardSize>::build(char verbose)
    {
        ExactScore sc;
        uint64_t n_changes = 0;
        std::vector<_Move> m;

        for (uint16_t sq0 = 0; sq0 < _dim1; sq0++)
        {
            _work_board->clear();
            _work_board->set_color(_color);
            _work_board->set_pieceid_at(_piecesID[0], sq0);
            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                set_score(sq0,sc);
            }
            else
            {
                assert(false);
            }
        }
        _is_build = true;
        set_unknown_to_draw();

        if (verbose) print();
        assert(check_unknown() == false);
        return true;
    }
};
#endif

