#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// TablebaseBaseHandler
//
//
#ifndef _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_HPP
#define _AL_CHESS_TABLEBASE_TABLEBASEHANDLER_HPP

namespace chess
{
    // TablebaseBaseHandler
    template <typename PieceID, typename uint8_t _BoardSize>
    class TablebaseBaseHandler
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        TablebaseBaseHandler(std::vector<PieceID>& v) : _piecesID(v)
        {
        }
        virtual ~TablebaseBaseHandler()
        {
        }

        virtual bool load() = 0;
        virtual bool save() const = 0;
        virtual bool build(char verbose) = 0;
        virtual bool is_build() const = 0;

    protected:
        std::vector<PieceID> _piecesID;

        ExactScore best_score(PieceColor color_child, ExactScore sc, ExactScore best_score, bool& is_same, bool& is_better) const;
    };

    template <typename PieceID, typename uint8_t _BoardSize>
    ExactScore TablebaseBaseHandler<PieceID, _BoardSize>::best_score(PieceColor color_child, ExactScore sc, ExactScore best_score,
                                    bool& is_same, bool& is_better) const
    {
        is_same     = false;
        is_better   = false;

        if (color_child == PieceColor::B) // W at parent
        {
            if (sc == ExactScore::WIN)
            {
                if (best_score == ExactScore::WIN) is_same = true;
                else is_better = true;

                return ExactScore::WIN;
            }
            else if (sc == ExactScore::DRAW)
            {
                if (best_score != ExactScore::WIN)
                {
                    if (best_score == ExactScore::DRAW) is_same = true;
                    else is_better = true;

                    return ExactScore::DRAW;
                }
            }
            else
            {
                // sc == ExactScore::LOSS
                if (best_score == ExactScore::UNKNOWN)
                {
                    is_better = true;
                    return ExactScore::LOSS;
                }
            }
        }
        else
        {
            if (sc == ExactScore::LOSS)
            {
                if (best_score == ExactScore::LOSS) is_same = true;
                else is_better = true;

                return ExactScore::LOSS;
            }
            else if (sc == ExactScore::DRAW)
            {
                if (best_score != ExactScore::LOSS)
                {
                    if (best_score == ExactScore::DRAW) is_same = true;
                    else is_better = true;

                    return ExactScore::DRAW;
                }
            }
            else
            {
                // sc == ExactScore::WIN
                if (best_score == ExactScore::UNKNOWN)
                {
                    is_better = true;
                    return ExactScore::WIN;
                }
            }
        }
        return best_score;
    }

};
#endif

