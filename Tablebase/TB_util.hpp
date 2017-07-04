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

    // expand_position
    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseUtil<PieceID, _BoardSize>::expand_position(_Board& pos, char verbose)
    {
        std::vector<_Move> m = pos.generate_moves();
        if (pos.is_final(m))
            return true;

        uint8_t NPIECE;
        std::vector<uint8_t>    v_dtc;
        std::vector<ExactScore> v_sc;
        std::vector<bool>       child_is_promo;
        std::vector<bool>       child_is_capture;
        std::vector<bool>       child_is_pawn;
        std::vector<std::vector<uint16_t>>    mv_sq;
        std::string     tb_name;
        PieceColor      parent_color;
        std::vector<uint16_t> v_sq;
        std::vector<PieceID>  v_id;

        if (verbose) std::cout << "expanding position:\n" << pos.to_str() << std::endl << std::endl;
        while (!pos.is_final(m))
        {
            if (pos.get_histo_size() > 256) return false;

            parent_color = pos.get_color();
            v_dtc.clear(); 
            v_sc.clear(); 
            child_is_promo.clear();
            child_is_capture.clear();
            child_is_pawn.clear();
            mv_sq.clear();
            for (size_t i = 0; i < m.size(); i++) 
            { 
                v_dtc.push_back(0); 
                v_sc.push_back(ExactScore::UNKNOWN); 
                child_is_promo.push_back(false); 
                child_is_capture.push_back(false);
                child_is_pawn.push_back(false);
            }

            for (size_t k = 0; k < m.size(); k++)
            {
                pos.apply_move(m[k]);

                v_sq.clear();
                v_id.clear();
                v_id = pos.get_piecesID();        // sorted
                for (auto& id : v_id)
                {
                    //..repeat..
                    v_sq.push_back(pos.get_square_ofpiece(  _Piece::get(id)->get_name(), _Piece::get(id)->get_color()));
                }
                mv_sq.push_back(v_sq);
                child_is_promo[k] = pos.is_last_move_promo();
                child_is_capture[k] = pos.is_last_move_capture();
                child_is_pawn[k] = pos.is_last_move_pawn();

                PieceSet<PieceID, _BoardSize> ps(PieceSet<PieceID, _BoardSize>::to_set(v_id, PieceColor::W), PieceSet<PieceID, _BoardSize>::to_set(v_id, PieceColor::B));
                uint16_t nw = ps.count_all_piece(PieceColor::W);
                uint16_t nb = ps.count_all_piece(PieceColor::B);

                std::string tb_name;
                //assert((NPIECE >= 1) && (NPIECE <= 4));

                if ((nw == 0) || (nb == 0))
                {
                    NPIECE = 1; // case Xv0 0vX
                    PieceSet<PieceID, _BoardSize> ps(v_id); ps.collapse_to_one_piece();
                    tb_name = ps.name(pos.get_color());

                    // collapse v_sq
                    uint16_t sq0 = v_sq[0];
                    v_sq.clear();
                    v_sq.push_back(sq0);

                    TablebaseBase<PieceID, _BoardSize>*  tb = TB_Manager<PieceID, _BoardSize>::instance()->find(NPIECE, tb_name);
                    if (tb == nullptr)
                        return false;
                    v_sc[k] = tb->score_v(v_sq);
                    if ((child_is_promo[k]) || (child_is_capture[k]))
                        v_dtc[k] = 0;
                    else
                        v_dtc[k] = tb->dtc_v(v_sq);
                }
                else if (nw < nb)
                {
                    assert(nw == pos.cnt_all_piece(PieceColor::W));
                    assert(nb == pos.cnt_all_piece(PieceColor::B));
                    tb_name = TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(v_id, pos.get_color());
                    NPIECE = (uint8_t)v_id.size();
                    TablebaseBase<PieceID, _BoardSize>*  tbsym = TB_Manager<PieceID, _BoardSize>::instance()->find_sym(tb_name);
                    if (tbsym == nullptr)
                        return false;
                    v_sc[k] = tbsym->score_v(v_sq);
                    if ((child_is_promo[k]) || (child_is_capture[k]))
                        v_dtc[k] = 0;
                    else
                        v_dtc[k] = tbsym->dtc_v(v_sq);
                }
                else
                {
                    tb_name = TB_Manager<PieceID, _BoardSize>::instance()->name_pieces(v_id, pos.get_color());
                    NPIECE = (uint8_t)v_id.size();
                    TablebaseBase<PieceID, _BoardSize>*  tb = TB_Manager<PieceID, _BoardSize>::instance()->find(NPIECE, tb_name);
                    if (tb == nullptr)
                        return false;
                    v_sc[k]  = tb->score_v(v_sq);
                    if ((child_is_promo[k]) || (child_is_capture[k]))
                        v_dtc[k] = 0;
                    else
                        v_dtc[k] = tb->dtc_v(v_sq);
                };

                pos.undo_move();
            }

            uint8_t ret_dtc; size_t ret_idx = 0;
            ExactScore sc = minmax_dtc<PieceID, _BoardSize>(parent_color, v_sc, v_dtc, child_is_promo, child_is_capture, child_is_pawn, ret_dtc, ret_idx);
            if (sc != ExactScore::UNKNOWN)
            {
                if (verbose)
                {
                    std::cout << "best: " << ret_idx << std::endl;
                    for (size_t i = 0; i < m.size(); i++)
                    {
                        std::cout << i << " dtc:" << (int)v_dtc[i] << " sc:" << ExactScore_to_string(v_sc[i]) << " " << m[i].to_str() << " ";
                        for (size_t z = 0; z < mv_sq[i].size(); z++) std::cout << (int) mv_sq[i][z] << " ";
                        std::cout << std::endl;
                    }
                }
                pos.apply_move(m[ret_idx]);
                if (verbose)
                {
                    std::cout << pos.to_str() << std::endl;
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

