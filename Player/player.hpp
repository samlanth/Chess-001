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

        // Genetic Algo would play games between population members by calling select_move_algo() for fitness evolution
        virtual size_t      select_move_algo(    _Board& board, std::vector<_Move>& m, size_t max_num_position, uint16_t max_depth) = 0;
        virtual TYPE_PARAM  eval_position_algo(  _Board& board, std::vector<_Move>& m) = 0;

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
        DomainPlayer(   const std::string playername,       const std::string partition_key,
                        const std::string classname_key,    const std::string instance_key);

        virtual ~DomainPlayer();

        virtual size_t      select_move_algo(   _Board& pos, std::vector<_Move>& m, size_t max_num_position, uint16_t max_depth)  override;
        virtual TYPE_PARAM  eval_position_algo( _Board& pos, std::vector<_Move>& m)  override;

        const std::string persist_key() const;
        virtual bool save() const override;
        virtual bool load() override;

        std::string get_partition_key()     const { return _partition_key; }
        std::string get_classname_key()     const { return _classname_key; }
        std::string get_instance_key()      const { return _instance_key; }
        _Domain*    get_domain()            const { return _domain; }
        _CondValNode& get_root()            const { return _root; }

    protected:
        TYPE_PARAM minimax( _Board& board, uint16_t depth, TYPE_PARAM a, TYPE_PARAM b, 
                            bool isMaximizing,  size_t max_num_node, 
                            size_t& ret_mv_idx, size_t& num_pos_eval,
                            bool is_recursive_entry = false);

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

        size_t      select_move_algo(   _Board& board, std::vector<_Move>& m, size_t max_position, uint16_t max_depth) override { return 0; }
        TYPE_PARAM  eval_position_algo( _Board& board, std::vector<_Move>& m) override { return 0; }

        bool save() const           override { return false; }
        bool load()                 override { return false; }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::DomainPlayer( const std::string playername,       const std::string partition_key,
                    const std::string classname_key,    const std::string instance_key)
            :   _BasePlayer(playername),
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

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::~DomainPlayer()
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

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    const std::string DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    ::persist_key() const
    {
        return playername() + "_" + _partition_key + "_" + _classname_key + "_" + _instance_key;
    }


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

    // minimax
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    TYPE_PARAM DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    minimax(_Board& board, uint16_t depth, TYPE_PARAM a, TYPE_PARAM b, bool isMaximizing, size_t max_num_node,
            size_t& ret_mv_idx, size_t& num_pos_eval, bool is_recursive_entry)
    {
        size_t best_a_idx = 0;
        size_t best_b_idx = 0;

        std::vector<_Move> m = board.generate_moves();
        if (depth == 0 || board.is_final(m) || num_pos_eval >= max_num_node)
        {
            num_pos_eval++;
            return eval_position_algo(board, m);
        }

        if (isMaximizing)
        {
            for (size_t i = 0; i < m.size(); i++)
            {
                board.apply_move(m[i]);
                TYPE_PARAM temp = this->minimax(board, depth - 1, a, b, false, max_num_node, ret_mv_idx, num_pos_eval, true);
                if (temp >= a) best_a_idx = i;
                a = std::max<TYPE_PARAM>(a, temp);
                board.undo_move();
                if (b <= a)
                {
                    return b; // b cutoff.
                }
            }
            ret_mv_idx = best_a_idx;
            return a;
        }
        else
        {
            for (size_t i = 0; i < m.size(); i++)
            {
                board.apply_move(m[i]);
                TYPE_PARAM temp = this->minimax(board, depth - 1, a, b, true, max_num_node, ret_mv_idx, num_pos_eval, true);
                if (temp <= b) best_b_idx = i;
                b = std::min(b, temp);
                board.undo_move();
                if (b <= a)
                {
                    return a; // a cutoff.
                }
            }
            ret_mv_idx = best_b_idx;
            return b;
        }
    }

    // select_move_algo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    size_t DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    select_move_algo(_Board& pos, std::vector<_Move>& m, size_t max_num_position, uint16_t max_depth)
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
        // classic minmax
        TYPE_PARAM  a = -std::numeric_limits<TYPE_PARAM>::max();    // eval is only in (0..1) currently
        TYPE_PARAM  b = std::numeric_limits<TYPE_PARAM>::max();
        size_t      ret_best_move_index = 0;
        size_t      num_pos_eval = 0;
        TYPE_PARAM  e = minimax(pos, max_depth, a, b, pos.get_color() == PieceColor::W, max_num_position, ret_best_move_index, num_pos_eval);
        return ret_best_move_index;
    }

    // eval_position_algo
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    TYPE_PARAM DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::
    eval_position_algo(_Board& pos, std::vector<_Move>& m)
    {
        TYPE_PARAM ret_eval = 0.5;
        bool ret = _root.eval_position(pos, m, ret_eval);
        if (ret == true) return ret_eval;

        // This is first layer of children domain (should make sure it cover all sub domains)
        for (size_t i = 0; i < _domain->_children.size(); i++)
        {
            ret = _domain->_children[i]->_attached_domain_player->_root.eval_position(pos, m, ret_eval);
            if (ret == true) return ret_eval;
        }

        // Failure if hole in the hiearchy of the domains of the partition
        // throw...
        return ret_eval;
    }

};

#endif
