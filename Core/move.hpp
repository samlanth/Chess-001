#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// MoveUnit         : struct holding chess move unit direction
// Move<PieceID>    : struct holding chess move definition/data
//
//
#ifndef _AL_CHESS_MOVE_H
#define _AL_CHESS_MOVE_H

#include "core/piece.hpp"
#include <string>

namespace chess
{
    enum class PieceName;
    enum class PieceColor;

    struct MoveUnit
    {
        enum struct FLAG { none = 0, conditional = 1 };

        MoveUnit() = default;

        MoveUnit(int8_t _x, int8_t _y, uint8_t _len, FLAG _flag = FLAG::none, std::string _flag_param = "")
             : x(_x), y(_y), len(_len), flag(_flag), flag_spec(_flag_param), context_extra(""){}

        int8_t      x;       // x direction
        int8_t      y;       // y direction
        uint8_t     len;     // max horizon 1..len
        FLAG        flag;    // move category (normal or conditional) 
        std::string flag_spec;     // conditional moveunit specification
        std::string context_extra; // extra info for move from a chess position
    };

    template <typename PieceID>
    struct Move
    {
        Move() 
        {
            init_state();
        }
        ~Move() = default;

        Move(Move&& other)
        {
            // nothing to steal so copying
            copy_internal(other);
            copy_state(other);
        }

        Move(const Move& m)
        {
            copy_internal(m);
            copy_state(m);
        }

        Move& operator=(const Move& lhs)
        {
            if (this != &lhs)
            {
                copy_internal(lhs);
                copy_state(lhs);
            }
            return *this;
        }

        Move(   const uint8_t sx,
                const uint8_t sy,
                const uint8_t dx,
                const uint8_t dy,
                const PieceID psrc_id,
                const PieceID pdst_id,
                const MoveUnit& u) 
            :
                src_x(sx),
                src_y(sy),
                dst_x(dx),
                dst_y(dy),
                prev_src_id(psrc_id),
                prev_dst_id(pdst_id),
                mu(u)
        {
            init_state();
        }

        void copy_internal(const Move& m)
        {
            src_x = m.src_x;
            src_y = m.src_y;
            dst_x = m.dst_x;
            dst_y = m.dst_y;
            prev_src_id = m.prev_src_id;
            prev_dst_id = m.prev_dst_id;
            mu = m.mu;
        }

        uint8_t src_x;
        uint8_t src_y;
        uint8_t dst_x;
        uint8_t dst_y;
        PieceID prev_src_id;        // pieceID at src_x/y before applying move
        PieceID prev_dst_id;        // pieceID at dst_x/y before applying move
        MoveUnit mu;
        bool    state_castling[4];  // White castling K or Q side, Black castling KQ state after applying move
        bool    state_ep[16];       // White pawn capture en passant[0..7] Black [8..15]

        bool get_state_castling(const PieceColor c, const PieceName side) const
        {
            if      ((c == PieceColor::W) && (side == PieceName::K)) return state_castling[0];
            else if ((c == PieceColor::W) && (side == PieceName::Q)) return state_castling[1];
            else if ((c == PieceColor::B) && (side == PieceName::K)) return state_castling[2];
            else if ((c == PieceColor::B) && (side == PieceName::Q)) return state_castling[3];
            return false;
        }
        void set_state_castling(const PieceColor c, const PieceName side, bool value)
        {
            if      ((c == PieceColor::W) && (side == PieceName::K)) state_castling[0] = value;
            else if ((c == PieceColor::W) && (side == PieceName::Q)) state_castling[1] = value;
            else if ((c == PieceColor::B) && (side == PieceName::K)) state_castling[2] = value;
            else if ((c == PieceColor::B) && (side == PieceName::Q)) state_castling[3] = value;
        }

        bool get_state_ep(const PieceColor c, const uint8_t pawn_x) const
        {
            if ((c == PieceColor::W) && (pawn_x >= 0) && (pawn_x <= 7)) return state_ep[pawn_x];
            if ((c == PieceColor::B) && (pawn_x >= 0) && (pawn_x <= 7)) return state_ep[8+pawn_x];
            return false;
        }
        void set_state_ep(const PieceColor c, const uint8_t pawn_x, bool value)
        {
            if      ((c == PieceColor::W) && (pawn_x >= 0) && (pawn_x <= 7)) state_ep[pawn_x] = value;
            else if ((c == PieceColor::B) && (pawn_x >= 0) && (pawn_x <= 7)) state_ep[8 + pawn_x] = value;
        }

        void copy_state(const Move& from)
        {
            for(size_t i = 0; i < 4;  i++) this->state_castling[i] = from.state_castling[i];
            for(size_t i = 0; i < 16; i++) this->state_ep[i] = from.state_ep[i];
        }
        void init_state()
        {
            for (size_t i = 0; i < 4; i++) this->state_castling[i] = true;
            for (size_t i = 0; i < 16; i++) this->state_ep[i] = true;
        }
    };

};
#endif
