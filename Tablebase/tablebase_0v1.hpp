#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Tablebase_0v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_0v1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_0v1_HPP

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_0v1 : public Tablebase<PieceID, _BoardSize, 1>
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        Tablebase_0v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 1>(v, c)
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
    bool Tablebase_0v1<PieceID, _BoardSize>::isPiecesMatch(const _Board& pos)
    {
        if (    (pos.cnt_all_piece() == 1) &&
                (pos.has_piece(_Piece::get(_piecesID[0])->get_name(), PieceColor::B)))
        {
            return true;
        }
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize>
    bool Tablebase_0v1<PieceID, _BoardSize>::build(char verbose)
    {
        ExactScore sc;
        std::vector<_Move> m;
        _Board* _work_board = new _Board();

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

        TablebaseManager<PieceID, _BoardSize>::instance()->add(name(), this);

        if (verbose) print();
        assert(check_unknown() == false);
        delete _work_board;
        return true;
    }
};
#endif

