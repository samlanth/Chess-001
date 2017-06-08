#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Player<...> : Chess Player capable of learning (Genetical Algorithm)
//
// The chromosomes of the player are in CondValNode x (tree size) x number of domains:
//      std::vector<_ConditionFeature*> _conditions;
//      std::vector<bool>               _conditions_and_or;
//      std::vector<_ValuationFeature*> _valuations;
//      std::vector<TYPE_PARAM>         _weights;
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
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
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
            _root(nullptr, true, false) // initially empty, must be fill or load
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

                    _Domain* p_lookup_child_domain;
                    for (size_t i = 0; i < _domain->_children.size(); i++)
                    {
                        p_lookup_child_domain = partition_ptr->find_domain(_Domain::domain_key(_domain->_children[i]->_classname_key, _domain->_children[i]->_instance_key));
                        if (p_lookup_child_domain != nullptr)
                        {
                            if (_domain->_children[i]->_attached_domain_player == nullptr)
                            {
                                _domain->_children[i]->_attached_domain_player = new DomainPlayer(playername, _partition_key, _domain->_children[i]->_classname_key, _domain->_children[i]->_instance_key);
                                // recursion
                            }
                        }
                    }
                }
            }
        }

        virtual ~DomainPlayer()
        {
            if (_domain != nullptr)
            {
                for (size_t i = 0; i < _domain->_children.size(); i++)
                {
                    if (_domain->_children[i] != nullptr)
                    {
                        if (_domain->_children[i]->_attached_domain_player != nullptr)
                        {
                            delete _domain->_children[i]->_attached_domain_player;
                            _domain->_children[i]->_attached_domain_player = nullptr;
                            // recursion
                        }
                    }
                }
            }
        }

        const std::string persist_key() const
        {
            return playername() + "_" + _partition_key + "_" + _classname_key + "_" + _instance_key;
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
            // Possible strategy: select best of minmax ( eval_position_algo, max_num_position, ...)
            //...
            TYPE_PARAM e = eval_position_algo(pos, m, max_num_position);

            return 0;
        }

        virtual TYPE_PARAM eval_position_algo(const _Board& pos, const std::vector<_Move>& m, size_t max_num_position) const override
        {
            TYPE_PARAM ret_eval = 0.5;
            bool ret = _root.eval_position(pos, m, max_num_position, ret_eval);
            if (ret == true) return ret_eval;

            // This is first layer of children domain (should make sure it cover all sub domains)
            for (size_t i = 0; i < _domain->_children.size(); i++)
            {
                ret = _domain->_children[i]->_attached_domain_player->_root.eval_position(pos, m, max_num_position, ret_eval);
                if (ret == true) return ret_eval;
            }
           
            // Failure if hole in the hiearchy of the domains of the partition
            // throw...
            return ret_eval;
        }

        virtual bool save() const override;
        virtual bool load() override;

        std::string get_partition_key()     const { return _partition_key; }
        std::string get_classname_key()     const { return _classname_key; }
        std::string get_instance_key()      const { return _instance_key; }
        _Domain*    get_domain()            const { return _domain; }
        _CondValNode& get_root()            const { return _root; }

    protected:
        std::string             _partition_key;
        std::string             _classname_key; // domain key part
        std::string             _instance_key;  // domain key part
        _Domain*                _domain;
        _CondValNode            _root;          // The brain of the player!
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


    // DomainPlayer::save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save() const
    {
        std::string f = PersistManager::instance()->get_stream_name("domainplayer", persist_key());
        std::ofstream   filestream;
        filestream.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (filestream.good())
        {
            filestream <<  playername();  filestream << std::endl;
            filestream << _partition_key; filestream << std::endl;
            filestream << _classname_key; filestream << std::endl;
            filestream << _instance_key;  filestream << std::endl;

            filestream << _root.persist_key();  filestream << std::endl;
            _root.save_root();  // independant stream

            _Partition* partition_ptr = _PartitionManager::instance()->find_partition(_partition_key);
            if (partition_ptr != nullptr)
            {
                if (_domain != nullptr)
                {
                    if (_domain->_attached_domain_player != nullptr)
                    {
                        for (size_t i = 0; i < _domain->_children.size(); i++)
                        {
                            if (_domain->_children[i] != nullptr)
                            {
                                if (_domain->_children[i]->_attached_domain_player != nullptr)
                                {
                                    _domain->_children[i]->_attached_domain_player->save();
                                    // recursion
                                }
                            }
                        }
                    }
                }
            }

            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }

    // DomainPlayer::load()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load()
    {
        std::string f = PersistManager::instance()->get_stream_name("domainplayer", persist_key());
        std::ifstream   filestream;
        filestream.open(f.c_str(), std::fstream::in);
        if (filestream.good())
        {
            std::string playername;
            std::string partition_key;
            std::string classname_key;
            std::string instance_key;
            std::string root_persist_key;

            filestream >> playername;
            filestream >> partition_key;
            filestream >> classname_key;
            filestream >> instance_key;
            filestream >> root_persist_key;

            assert(playername    == _playername);
            assert(partition_key == _partition_key);
            assert(classname_key == _classname_key);
            assert(instance_key  == _instance_key);

            _root._persist_key = root_persist_key;;
            _root.load_root(); // root data was not load from DomainPlayer() constructor

            _Partition* p_partition = _PartitionManager::instance()->find_partition(partition_key);
            if (p_partition == nullptr) return false;

            for (size_t i = 0; i < _domain->_children.size(); i++)
            {
                if (_domain->_children[i] != nullptr)
                {
                    if (_domain->_children[i]->_attached_domain_player != nullptr)
                    {
                        _domain->_children[i]->_attached_domain_player->load();
                        // recursion
                    }
                }
            }

            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }
};

#endif
