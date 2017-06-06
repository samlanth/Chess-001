#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// <... work in progress ...>
//
// Player<...> : Chess Player capable of learning (Genetical Algorithm)
// The chromosome of the player are its public parameters
//
//
#ifndef _AL_CHESS_PLAYER_H
#define _AL_CHESS_PLAYER_H

#include "core/board.hpp"
#include "core/piece.hpp"
#include "core/move.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cassert>

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;
    template <typename PieceID> struct Move;

    // BasePlayer interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BasePlayer
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        BasePlayer(std::string aname) : _name(aname) {}
        virtual ~BasePlayer() {}

        virtual bool train(/*...time_limit...*/) = 0;
        virtual size_t select_move(const _Board& board, const std::vector<_Move>& m /*...time_limit...*/) const = 0;

        // Persistence
        virtual bool save() const = 0;
        virtual bool load() = 0;

        const std::string name()                        { return _name; }
        const std::vector<TYPE_PARAM>& params() const   { return _params; }

    protected:
        std::string             _name;
        std::vector<TYPE_PARAM> _params;
    };

    // Exemple
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class NullPlayer : public BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;;

    public:
        NullPlayer() : BasePlayer("NULLPlayer"){}

        size_t select_move(const _Board& board, const std::vector<_Move>&  m) const override { return 0; }
        bool save() const           override { return false; }
        bool load()                 override { return false; }
        bool train()                override { return false; }
    };
};

#endif
