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
#include "domain/partition.hpp"

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
        BasePlayer(const std::string aname) : _name(aname) , _PARAM_NBIT(PARAM_NBIT){}
        virtual ~BasePlayer() {}

        virtual bool train(/*...time_limit...*/) = 0;
        virtual size_t select_move(const _Board& board, const std::vector<_Move>& m /*...time_limit...*/) const = 0;

        // Persistence
        virtual bool save() const = 0;
        virtual bool load() = 0;

        const std::string name()                const   { return _name; }
        const size_t param_NBIT()               const   { return _PARAM_NBIT; }
        const std::vector<TYPE_PARAM>& params() const   { return _params; }

    protected:
        std::string             _name;
        size_t                  _PARAM_NBIT;
        std::vector<TYPE_PARAM> _params;
    };

    // DomainPlayer
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainPlayer
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        DomainPlayer(const std::string aname, const std::string partition_key, const std::string domain_key)
            : _name(aname), _partition_key(partition_key), _domain_key(domain_key), _domain(nullptr)
        {
            _Partition* p = _PartitionManager::instance()->find_partition(_partition_key);
            if (p != nullptr)
            {
                _domain = p->find_domain(_domain_key);
            }
        }

        virtual ~DomainPlayer() {}

        virtual bool train() = 0;
        virtual size_t select_move(const _Board& pos, const std::vector<_Move>& m) const override
        {
            if (_domain != nullptr)
            {
                if (_domain->has_known_score_move())
                {
                    size_t ret_mv_idx;
                    ExactScore sc = get_known_score_move(pos, m, size_t ret_mv_idx);
                    if (sc != ExactScore::UNKNOWN)
                        return ret_mv_idx;
                }
            }
            //...
            return 0;
        }

        virtual bool save() const = 0;
        virtual bool load() = 0;

        std::string get_partition_key() const { return _partition_key; }
        std::string get_domain_key()    const { return _domain_key; }
        _Domain*    get_domain()        const { return _domain; }

    private:
        std::string _partition_key;
        std::string _domain_key;
        _Domain*    _domain;
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
