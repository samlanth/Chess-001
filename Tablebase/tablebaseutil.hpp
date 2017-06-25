#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseUtil
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEUTIL_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEUTIL_HPP

namespace chess
{
    // TablebaseUtil
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseUtil
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TablebaseUtil() {}
        virtual ~TablebaseUtil() {}

        bool  expand_position(_Board& pos, char verbose = 0);   // Follow optimal play
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseUtil<PieceID, _BoardSize>::expand_position(_Board& pos, char verbose)
    {
        std::vector<_Move> m = pos.generate_moves();
        if (pos.is_final(m))
            return true;

        size_t          NPIECE;
        ExactScore      sc;
        uint8_t         dtc;
        std::vector<uint8_t>    v_dtc;
        std::vector<ExactScore> v_sc;
        std::string     tb_name;
        PieceColor      parent_color;

        if (verbose) std::cout << "expanding position:\n" << pos.to_str() << std::endl << std::endl;
        while (!pos.is_final(m))
        {
            if (pos.get_histo_size() > 256) return false;

            parent_color = pos.get_color();
            v_dtc.clear(); v_sc.clear();
            for (size_t i = 0; i < m.size(); i++) { v_dtc.push_back(0); v_sc.push_back(ExactScore::UNKNOWN); }

            for (size_t k = 0; k < m.size(); k++)
            {
                pos.apply_move(m[k]);

                std::vector<uint16_t> v_sq;
                std::vector<PieceID>  v_id = pos.get_piecesID();        // sorted
                for (auto& id : v_id)
                {
                    v_sq.push_back(pos.get_square_ofpiece(  _Piece::get(id)->get_name(), _Piece::get(id)->get_color()));
                }

                std::string tb_name = TablebaseManager<PieceID, _BoardSize>::instance()->name_pieces(v_id, pos.get_color());

                sc = ExactScore::UNKNOWN;
                NPIECE = v_id.size();
                assert((NPIECE >= 1) && (NPIECE <= 3));

                if (NPIECE == 1)
                {
                    Tablebase<PieceID, _BoardSize, 1>* tb1 = TablebaseManager<PieceID, _BoardSize>::instance()->find_1(tb_name);
                    if (tb1 == nullptr) return false;
                    sc  = tb1->score(v_sq[0]);
                    dtc = tb1->dtc(v_sq[0]);
                }
                else if (NPIECE == 2)
                {
                    Tablebase<PieceID, _BoardSize, 2>* tb2 = TablebaseManager<PieceID, _BoardSize>::instance()->find_2(tb_name);
                    if (tb2 == nullptr) return false;
                    sc  = tb2->score(v_sq[0], v_sq[1]);
                    dtc = tb2->dtc(v_sq[0], v_sq[1]);
                }
                else if (NPIECE == 3)
                {
                    Tablebase<PieceID, _BoardSize, 3>* tb3 = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(tb_name);
                    if (tb3 == nullptr) return false;
                    sc  = tb3->score(v_sq[0], v_sq[1], v_sq[2]);
                    dtc = tb3->dtc(v_sq[0], v_sq[1], v_sq[2]);
                }
                else
                {
                    assert(false);
                }
                v_sc[k]     = sc;
                v_dtc[k]    = dtc;

                pos.undo_move();
            }

            uint8_t ret_dtc; size_t ret_idx = 0;
            sc = TBHandlerCore<PieceID, _BoardSize>::minmax_dtc(parent_color, v_sc, v_dtc, ret_dtc, ret_idx);
            if (sc != ExactScore::UNKNOWN)
            {
                pos.apply_move(m[ret_idx]);

                if (verbose)
                {
                    std::cout << pos.to_str() << std::endl << std::endl;
                    std::cout << "best: " << ret_idx << std::endl;
                    for (size_t i = 0; i < m.size(); i++)
                    {
                        std::cout << i << " dtc:" << (int)v_dtc[i] << " sc:" << ExactScore_to_string(v_sc[i]) << std::endl;
                    }
                }
            }
            else
            {
                return false;
            }

            m = pos.generate_moves();
        }

        return true;
    }

};
#endif

