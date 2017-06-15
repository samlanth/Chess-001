#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#ifndef _AL_CHESS_CORE_UTIL_HPP
#define _AL_CHESS_CORE_UTIL_HPP

namespace chess
{
    enum class PieceName        { none, R, N, B, Q, K, P };
    enum class PieceColor       { none, W, B };
    enum class PieceMoveStyle   { none, Sliding, Jumping, SlidingDiagonalCapturePromo };
    enum class ExactScore       { LOSS, DRAW, WIN, UNKNOWN }; // white win is WIN, black win is LOSS
    enum class FeatureType      { condition = 0, valuation = 1 };

    static std::string  toNULLSTR(const std::string& s) { return (s.size() == 0) ? "NullSTR" : s; }
    static void         fromNULLSTR(std::string& s)     { if (s.size() == 0) s = ""; }

    static int ExactScore_to_int(ExactScore c)
    {
        if (c == ExactScore::LOSS) return 1;
        else if (c == ExactScore::DRAW) return 2;
        else if (c == ExactScore::WIN)  return 3;
        else return 0;
    }

    static ExactScore int_to_ExactScore(int t)
    {
        if (t == 1) return ExactScore::LOSS;
        else if (t == 2) return ExactScore::DRAW;
        else if (t == 3) return ExactScore::WIN;
        return ExactScore::UNKNOWN;
    }

    static int FeatureType_to_int(FeatureType c)
    {
        if (c == FeatureType::valuation) return 1;
        else return 0;
    }
    static FeatureType int_to_FeatureType(int t)
    {
        if (t == 1) return FeatureType::valuation;
        return FeatureType::condition;
    }

    static int PieceColor_to_int(PieceColor c)
    {
        if (c == PieceColor::W) return 1;
        else if (c == PieceColor::B) return 2;
        return 0;
    }
    static PieceColor int_to_PieceColor(int t)
    {
        if (t == 1) return PieceColor::W;
        else if (t == 2) return PieceColor::B;
        return PieceColor::none;
    }

    static int bool_to_int(bool c)
    {
        if (c) return 1;
        return 0;
    }
    static bool int_to_bool(int t)
    {
        if (t == 1) return true;
        return false;
    }


}
#endif
