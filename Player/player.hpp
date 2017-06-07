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
#include "domain/partitionmanager.hpp"
#include "feature/CondValNode.hpp"

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;
    template <typename PieceID> struct Move;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class BasePlayer;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class CondValNode;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class DomainPlayer;

    // BasePlayer interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BasePlayer
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _BasePlayer = BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        BasePlayer(const std::string& playername) : _playername(playername) , _PARAM_NBIT(PARAM_NBIT){}
        virtual ~BasePlayer() {}

        virtual size_t      select_move_algo(    const _Board& board, const std::vector<_Move>& m, size_t max_num_position) const = 0;
        virtual TYPE_PARAM  eval_position_algo(  const _Board& board, const std::vector<_Move>& m, size_t max_num_position) const = 0;

        // Persistence
        virtual bool save() const = 0;
        virtual bool load() = 0;

        const std::string playername()          const   { return _playername; }
        const size_t param_NBIT()               const   { return _PARAM_NBIT; }


    protected:
        std::string             _playername;
        size_t                  _PARAM_NBIT;
    };

    // DomainPlayer
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainPlayer : public BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _CondValNode = CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _BasePlayer = BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        DomainPlayer(   const std::string playername, 
                        const std::string partition_key, 
                        const std::string classname_key, 
                        const std::string instance_key)
            : _BasePlayer(playername),
            _partition_key(partition_key), 
            _classname_key(classname_key), 
            _instance_key(instance_key), 
            _domain(nullptr),
            _root(nullptr, true, false)
        {
            _Partition* partition_ptr = _PartitionManager::instance()->find_partition(_partition_key);
            if (partition_ptr != nullptr)
            {
                _domain = partition_ptr->find_domain(_Domain::domain_key(_classname_key, instance_key));
                if (_domain != nullptr)
                {
                    if (_domain->_attached_domain_player == nullptr)
                    {
                        _domain->attach_domain_player(this);
                    }

                    _Domain*        children_domain_ptr;
                    DomainPlayer*   children_domain_player_ptr;
                    for (size_t i = 0; i < _domain->_children.size(); i++)
                    {
                        children_domain_ptr = partition_ptr->find_domain(_Domain::domain_key(_domain->_children[i]->_classname_key, _domain->_children[i]->_instance_key));
                        if (children_domain_ptr != nullptr)
                        {
                            if (children_domain_ptr->_attached_domain_player == nullptr)
                            {
                                children_domain_player_ptr = new DomainPlayer(playername, children_domain_ptr->_partition_key, children_domain_ptr->_classname_key, children_domain_ptr->_instance_key);
                                // recursion
                            }
                        }
                    }
                }
            }

        }

        ~DomainPlayer()
        {
        }

        virtual size_t select_move_algo(const _Board& pos, const std::vector<_Move>& m, size_t max_num_position) const override
        {
            if (_domain != nullptr)
            {
                if (_domain->has_known_score_move())
                {
                    size_t ret_mv_idx;
                    ExactScore sc = _domain->get_known_score_move(pos, m, ret_mv_idx);
                    if (sc != ExactScore::UNKNOWN)
                        return ret_mv_idx;
                }
            }
            //...
            // select best of minmax ( eval_position_algo, max_num_position, ...)
            //...
            TYPE_PARAM e = eval_position_algo(pos, m, max_num_position);

            return 0;
        }

        virtual TYPE_PARAM eval_position_algo(const _Board& pos, const std::vector<_Move>& m, size_t max_num_position) const override
        {
            // if position is in this Domain then _root.eval()
            // else locate children_domain player then try _root.eval()

            TYPE_PARAM ret_eval = 0.5;
            bool ret = _root.eval_position(pos, m, max_num_position, ret_eval);
            if (ret == true) return ret_eval;

            // This is first layer of children (ok 99% time)
            for (size_t i = 0; i < _domain->_children.size(); i++)
            {
                ret = _domain->_children[i]->_attached_domain_player->_root.eval_position(pos, m, max_num_position, ret_eval);
                if (ret == true) return ret_eval;
            }
            // TODO cover all child domain tree...
           
            // Failure if hole in the hiearchy of the domains of the partition
            // throw...
            return ret_eval;
        }

        // ...
        virtual bool save() const   { return false; }
        virtual bool load()         { return false; }

        std::string get_partition_key()     const { return _partition_key; }
        std::string get_classname_key()     const { return _classname_key; }
        std::string get_instance_key()      const { return _instance_key; }
        _Domain*    get_domain()            const { return _domain; }
        _CondValNode& get_root()            const { return _root; }

    protected:
        // domain
        std::string             _partition_key;
        std::string             _classname_key;
        std::string             _instance_key;
        _Domain*                _domain;

        _CondValNode            _root;    // The brain of the player!
    };


    // Exemple
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class NullPlayer : public BasePlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;;

    public:
        NullPlayer() : BasePlayer("NULLPlayer"){}

        size_t      select_move_algo(   const _Board& board, const std::vector<_Move>& m, size_t max_position) const override { return 0; }
        TYPE_PARAM  eval_position_algo( const _Board& board, const std::vector<_Move>& m, size_t max_position) const override { return 0; }

        bool save() const           override { return false; }
        bool load()                 override { return false; }
    };
};

#endif
