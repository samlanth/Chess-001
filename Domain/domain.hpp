#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// Domain<...>      : Base chess domain interface
// DomainKQvK       : KQvK chess domain
// DomainKvK        : KvK  chess domain
//
//
#ifndef _AL_CHESS_DOMAIN_H
#define _AL_CHESS_DOMAIN_H

#include "domain/partition.hpp"
#include "domain/partitionmanager.hpp"

namespace chess
{
    using ScoreType = int8_t;
    enum class ExactScore :ScoreType { LOSS = -1, DRAW = 0, WIN=1, UNKNOWN=2};

    // forward
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class DomainKvK;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class DomainKQvK;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class DomainPlayer;

    // Domain interface
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain 
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        friend class _DomainPlayer;

    protected:
        std::string             _partition_key;
        std::string             _classname_key;
        std::string             _instance_key;
        std::vector<_Domain*>   _children;

        _DomainPlayer*          _attached_domain_player;

    public:
        Domain( const std::string partition_key,
                const std::string classname_key,
                const std::string instance_key) :
            _partition_key(partition_key), _classname_key(classname_key), _instance_key(instance_key),
            _attached_domain_player(nullptr)
        {}

        virtual bool       has_known_score_move()  const = 0;
        virtual ExactScore get_known_score_move(const _Board& position, const std::vector<_Move>& m, size_t& ret_mv_idx) const = 0;

        const std::vector<_Domain*>& children() const { return _children;}

        const std::string partition_key()   const { return _partition_key; }
        const std::string classname_key()   const { return _classname_key; }
        const std::string instance_key()    const { return _instance_key;  }

        const std::string domain_key()      const { return _classname_key+"_"+_instance_key; }
        static const std::string domain_key(const std::string& k1, const std::string& k2) { return k1 + "_" + k2; }
        const std::string persist_key()      const { return _partition_key + "_" + domain_key(); }

        virtual bool isInDomain(const _Board& position) const = 0;

        // Persistence
        virtual bool save() const = 0;
        virtual bool load() = 0;

        static _Domain* make(const std::string& partition_key, const std::string& classname_key, const std::string& instance_key)
        {
            if      (classname_key == "DomainKvK")  return new DomainKvK <PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key);
            else if (classname_key == "DomainKQvK") return new DomainKQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(partition_key);
            return nullptr;
        }

        void attach_domain_player(_DomainPlayer* player) { _attached_domain_player = player; }

    protected:
        bool save_root() const
        {
            std::string f = PersistManager::instance()->get_stream_name("domain", persist_key());
            std::ofstream   filestream;
            filestream.open(f.c_str(), std::ofstream::out | std::ofstream::trunc);
            if (save_detail(filestream))
            {
                filestream.close();
                return true;
            }
            filestream.close();
            return false;
        }

        bool save_detail(std::ofstream& filestream) const
        {
            if (filestream.good())
            {
                filestream << _partition_key; filestream << std::endl;
                filestream << _classname_key; filestream << std::endl;
                filestream << _instance_key;  filestream << std::endl;

                filestream << _children.size(); filestream << std::endl;
                for (auto& v : _children)
                {
                    filestream << v->partition_key(); filestream << std::endl;
                    filestream << v->classname_key(); filestream << std::endl;
                    filestream << v->instance_key();  filestream << std::endl;
                }
                return true;
            }
            return false;
        }

        bool load_root() const
        {
            std::string f = PersistManager::instance()->get_stream_name("domain", persist_key());
            std::ifstream   filestream;
            filestream.open(f.c_str(), std::fstream::in);
            if (load_detail(filestream))
            {
                filestream.close();
                return true;
            }
            filestream.close();
            return false;
        }

        bool load_detail(std::ifstream& filestream) const
        {
            if (filestream.good())
            {
                size_t n_child;
                std::string partition_key;
                std::string classname_key;
                std::string instance_key;

                filestream >> partition_key;
                filestream >> classname_key;
                filestream >> instance_key;

                // check
                assert(partition_key == _partition_key);
                assert(classname_key == _classname_key);
                assert(instance_key == _instance_key);

                filestream >> n_child;

                _Partition* p_partition = _PartitionManager::instance()->find_partition(partition_key);
                if (p_partition == nullptr) return false;

                _Domain* p_dom = p_partition->find_domain(domain_key(classname_key, instance_key));
                if (p_dom == nullptr) return false;

                // reloading
                p_dom->_children.clear();

                bool ok = true;
                for (size_t i = 0; i < n_child; i++)
                {
                    ok = false;
                    filestream >> partition_key;
                    filestream >> classname_key;
                    filestream >> instance_key;              
                    {
                        _Domain* p_dom_child = p_partition->find_domain(domain_key(classname_key,instance_key));
                        if (p_dom_child != nullptr)
                        {
                            if (p_dom->add_child(p_dom_child))
                            {
                                ok = true;                              
                            }
                        }
                    }
                    if (!ok)
                    {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }

        // Hold a database of the best position/move so far [So can evolved a score Predictor]
        // ...

    public:
        bool add_child(_Domain* p)
        {
            // check already exist...
            _children.push_back(p);
            return true;
        }

        Domain* get_child(size_t idx) const
        {
            if ((idx>=0) && (idx < _children.size())) return _children.at(idx);
            return nullptr;
        }

        size_t size_children() { return _children.size(); }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainKvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board  = Board<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        friend class _DomainPlayer;

    public:
        DomainKvK(const std::string partition_key) 
            : _Domain(partition_key, "DomainKvK", "0") {}

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
               // else  return ExactScore::DRAW; // must know also the move
            }
            return ExactScore::UNKNOWN;
        }

        virtual bool save() const override
        {
            return save_root();
        }
        virtual bool load() override 
        {
            return load_root();
        }
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class DomainKQvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        friend class _DomainPlayer;

    public:
        DomainKQvK(const std::string partition_key) 
            : _Domain(partition_key, "DomainKQvK", "0")
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

        virtual bool save() const override  
        {
            return save_root();
        }
        virtual bool load() override
        {
            return load_root();
        }
    };

};

#endif
