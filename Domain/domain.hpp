#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// <... work in progress ...>
//
// Partiton<...>    : A partition of the chess space into hiearchical domains
// Domain<...>      : Base chess domain interface
// Domain_KQvK      : KQvK chess domain
// Domain_KvK       : KvK  chess domain
//
//
#ifndef _AL_CHESS_DOMAIN_H
#define _AL_CHESS_DOMAIN_H

#include "core/board.hpp"
#include "core/piece.hpp"
#include "core/move.hpp"
#include "player/player.hpp"

namespace chess
{
    using ScoreType = int8_t;
    enum class ExactScore :ScoreType { LOSS = -1, DRAW = 0, WIN=1, UNKNOWN=2};

    // Domain interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain 
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        std::string             _name;
        std::vector<_Domain*>   _children;

    public:
        Domain(const std::string aname) : _name(aname) {}

        virtual bool has_known_score_move()  const = 0;
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const = 0;

        const std::vector<_Domain*>& children()  const { return _children;}
        const std::string name()                 const { return _name; }

        virtual bool isInDomain(const _Board& position) const = 0;

        // Persistence
        virtual bool save() const = 0;
        virtual bool load() = 0;

        // Hold a database of the best position/move so far [So can evolved score Predictor]
        // ...

        bool add_child(_Domain* p)
        {
            _children.push_back(p);
            return true;
        }

        Domain* get_child(size_t idx) const
        {
            if (idx < _children.size())
                return _children.at(idx);
            return nullptr;
        }

        size_t size_children() { return _children.size(); }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain_KvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board  = Board<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;

    public:
        Domain_KvK() : _Domain("KvK") {}

        bool isInDomain(const _Board& position) const override 
        { 
            if ( (position.cnt_piece(PieceName::K, PieceColor::W) == 1) &&
                 (position.cnt_piece(PieceName::K, PieceColor::B) == 1) &&
                 (position.cnt_all_piece() == 2))
                return true;
            return false;
        }

        virtual bool has_known_score_move() const { return true; }
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const
        { 
            if (position.is_allow_self_check())
            {
                if (position.can_capture_opposite_king(m, ret_mv_idx))
                    return ExactScore::WIN;
               // else
               //     return ExactScore::DRAW; // must know also the move
            }
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override  { return false; } //...
        virtual bool load() override        { return false; } //...
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain_KQvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;

    public:
        Domain_KQvK() : _Domain("KQvK") 
        {
        }

        bool isInDomain(const _Board& position) const override
        {
            if ((position.cnt_piece(PieceName::K, PieceColor::W) == 1) &&
                (position.cnt_piece(PieceName::K, PieceColor::B) == 1) &&
                (position.cnt_piece(PieceName::Q, PieceColor::W) == 1) &&
                (position.cnt_all_piece() == 3))
                return true;
            return false;
        }

        virtual bool has_known_score_move() const { return false; }
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const
        {
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override  { return false; } //...
        virtual bool load() override        { return false; } //...
    };

};

#endif
