#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TBHandler_2v1
// Tablebase_2v1
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASE_2v1_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASE_2v1_HPP

namespace chess
{
    // Tablebase_2v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class Tablebase_2v1 : public Tablebase<PieceID, _BoardSize, 3>
    {
    public:
        Tablebase_2v1(std::vector<PieceID>& v, PieceColor c) : Tablebase<PieceID, _BoardSize, 3>(v, c) {}
        ~Tablebase_2v1() {}
    };

    // TBHandler_2v1
    template <typename PieceID, typename uint8_t _BoardSize>
    class TBHandler_2v1 : public TBHandler_3<PieceID, _BoardSize>
    {
        using _Board = Board<PieceID, _BoardSize>;

    public:
        TBHandler_2v1(const PieceSet<PieceID, _BoardSize>& ps, TBH_OPTION option) : TBHandler_3<PieceID, _BoardSize>(ps, TB_TYPE::tb_2v1, option ) {}
        ~TBHandler_2v1() {}

    protected:      
         void print_pos_with_dtc_score(uint8_t value_dtc, ExactScore value_sc) const;
    };



    // Make as algo<N>...
    // print_pos_with_dtc_score
    template <typename PieceID, typename uint8_t _BoardSize>
    inline void TBHandler_2v1<PieceID, _BoardSize>::print_pos_with_dtc_score(uint8_t value_dtc, ExactScore value_sc) const
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
                            // ordering repeat...
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

