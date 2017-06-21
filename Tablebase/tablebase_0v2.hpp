#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_2v0
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_0v2_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_0v2_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_0v2 : public Tablebase<PieceID, _BoardSize, 2>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        Tablebase_0v2(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 2>(v, c)
        {
        }

        bool load() override
        {
            _is_build = read_tb();
            if (_is_build) TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);
            return _is_build;
        }

        bool save() const override { return save_tb(); }
        bool build(char verbose = 0) override;
        bool isPiecesMatch(const _Board& pos) override;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_0v2<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if ((pos.cnt_all_piece() == 2) &&
            (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::B)) &&
            (pos.has_piece(_Piece::get(_piecesID[1])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_0v2<PieceID, _BoardSize>::build(char verbose)
    {
        ExactScore sc;
        std::vector<_Move> m;

        for (uint16_t sq0 = 0; sq0 < _dim1; sq0++)
        for (uint16_t sq1 = 0; sq1 < _dim1; sq1++)
        {
            if (sq0 == sq1) continue;

            _work_board->clear();
            _work_board->set_color(_color);
            _work_board->set_pieceid_at(_piecesID[0], sq0);
            _work_board->set_pieceid_at(_piecesID[1], sq1);
            m = _work_board->generate_moves();
            sc = _work_board->final_score(m);
            if (sc != ExactScore::UNKNOWN)
            {
                set_score(sq0, sq1, sc);
            }
            else
            {
                assert(false);
            }
        }
        _is_build = true;
         set_unknown_to_draw();
         TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), *this);

        if (verbose) print();
        assert(check_unknown() == false);
        return true;
    }
};
#endif

