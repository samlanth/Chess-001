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

        friend class TablebaseHandler_1v1<PieceID, _BoardSize>;
        friend class TablebaseHandler_2v1<PieceID, _BoardSize>;

    public:
        TablebaseUtil()
        {
        }
        virtual ~TablebaseUtil()
        {
        }
        bool  expand_position(_Board& pos);

    };

    template <typename PieceID, typename uint8_t _BoardSize>
    bool TablebaseUtil<PieceID,_BoardSize>::expand_position(_Board& pos)
    {
        //if (!_tb.isPiecesMatch())
        //    return false;

        std::vector<_Move> m = pos.generate_moves();
        if (pos.is_final(m))
            return true;

        size_t          NPIECE;
        ExactScore      sc;
        ExactScore      best_score;
        size_t          best_score_index;
        std::string     tb_name;
        PieceColor      parent_color;

        while (!pos.is_final(m))
        {
            if (pos.get_histo_size() > 10) return false;

            parent_color = pos.get_color();
            best_score_index = -1;
            best_score = ExactScore::UNKNOWN;
            for (size_t k = 0; k < m.size(); k++)
            {
                pos.apply_move(m[k]);

                std::vector<uint16_t> v_sq;
                std::vector<PieceID>  v_id = pos.get_piecesID();        // sorted
                for (auto& id : v_id)
                    v_sq.push_back (pos.get_square_ofpiece(_Piece::get(id)->get_name(), _Piece::get(id)->get_color()));

                std::string tb_name = TablebaseManager<PieceID, _BoardSize>::instance()->name_pieces(v_id, pos.get_color());

                sc = ExactScore::UNKNOWN;
                NPIECE = v_id.size();
                assert((NPIECE >= 1) && (NPIECE <= 3));

                if (NPIECE == 1)
                {
                    Tablebase<PieceID, _BoardSize, 1>* tb1 = TablebaseManager<PieceID, _BoardSize>::instance()->find_1(tb_name);
                    if(tb1 == nullptr) return false;
                    sc = tb1->score(v_sq[0]);
                }
                else if (NPIECE == 2)
                {
                    Tablebase<PieceID, _BoardSize, 2>* tb2 = TablebaseManager<PieceID, _BoardSize>::instance()->find_2(tb_name);
                    if (tb2 == nullptr) return false;
                    sc = tb2->score(v_sq[0], v_sq[1]);
                }
                else if (NPIECE == 3)
                {
                    Tablebase<PieceID, _BoardSize, 3>* tb3 = TablebaseManager<PieceID, _BoardSize>::instance()->find_3(tb_name);
                    if (tb3 == nullptr) return false;
                    sc = tb3->score(v_sq[0], v_sq[1], v_sq[2]);
                }
                else
                {
                    assert(false);
                }

                parent_color = pos.get_opposite_color();
                if (sc != ExactScore::UNKNOWN)
                {
                    if (best_score == ExactScore::UNKNOWN)
                    {
                        best_score = sc;
                        best_score_index = k;
                    }
                    else if (parent_color == PieceColor::W) // max
                    {
                        if  (sc == ExactScore::WIN) { best_score = sc; best_score_index = k; }
                        else if ((sc == ExactScore::DRAW) && (best_score != ExactScore::WIN)) { best_score = sc; best_score_index = k; }
                    }
                    else
                    {
                        if (sc == ExactScore::LOSS) { best_score = sc; best_score_index = k; }
                        else if ((sc == ExactScore::DRAW) && (best_score != ExactScore::LOSS)) { best_score = sc; best_score_index = k; }
                    }
                }

                pos.undo_move();
                if (best_score == ExactScore::UNKNOWN)
                    return false;
            }

            // do best move
            if (best_score != ExactScore::UNKNOWN)
            {
                pos.apply_move(m[best_score_index]);
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

