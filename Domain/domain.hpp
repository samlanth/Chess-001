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
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Domain;

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

        virtual bool save() const override  { return false; } //...
        virtual bool load() override        { return false; } //...
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Domain_KQvK : public Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

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

        virtual bool save() const override  { return false; } //...
        virtual bool load() override        { return false; } //...
    };

    // Partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class Partition
    {
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM,  PARAM_NBIT>;
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        Partition() = default;
        Partition(std::string aname) : _name(aname){ }
        const std::string name()   const { return _name; }

        // Persistence
        bool save() const   { return false; } //...
        bool load()         { return false; } //...

        bool add_domain(_Domain* p)
        {
            auto iter = _domains.find(p->name());
            if (iter == _domains.end())
            {
                _domains.insert({ p->name(), p }); 
                return true;
            }
            return false;
        }

        _Domain* find_domain(const std::string domain_name) const
        {
            auto iter = _domains.find(domain_name);
            if (iter != _domains.end())
            {
                _Domain* p = iter->second;
                return p;
            }
            return nullptr;
        }

    private:
        std::string                      _name;
        std::map<std::string, _Domain*>  _domains;  // on heap - owner (std::unique_ptr...)
    };

    // PartitionManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class PartitionManager
    {
        using _Partition    = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Domain       = Domain<   PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        PartitionManager() {}

    public:
        ~PartitionManager()
        {
            if (_instance.operator bool())
            {
                // heap cleanup ...
                _instance.release();
            }
        }

    public:
        static bool add_partition(const _Partition& p)
        {
            if (_instance == nullptr) return false;

            auto iter = _instance->_partitions.find(p.name());
            if (iter == _instance->_partitions.end())
            {
                _instance->_partitions.insert({ p.name(), p });
                return true;
            }
            return false;
        }

        static _Partition* find_partition(const std::string partition_name)
        {
            if (_instance == nullptr) return nullptr;

            auto& iter = _instance->_partitions.find(partition_name);
            if (iter != _instance->_partitions.end())
            {
                return &(iter->second);
            }
            return nullptr;
        }

        static const PartitionManager* instance()
        {
            if (_instance == nullptr)
            {
                _instance = std::unique_ptr<PartitionManager>(new PartitionManager); // work around std::make_unique public ct()
                return _instance.get();
            }
            return  _instance.get();;
        }

        // Exemple
        static bool make_classic()
        {
            _Partition p("classic");
            if (!PartitionManager::instance()->add_partition(p)) return false;

            _Partition* p_classic = PartitionManager::instance()->find_partition("classic");
            if (p_classic == nullptr) return false;

            // create domains
            _Domain* dom_KK     = new Domain_KvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>();
            _Domain* dom_KQvK   = new Domain_KQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>();

            if (!p_classic->add_domain(dom_KK))     return false;
            if (!p_classic->add_domain(dom_KQvK))   return false;

            _Domain* ptr_dom_KQvK   = p_classic->find_domain("KQvK");if (ptr_dom_KQvK == nullptr) return false;
            _Domain* ptr_dom_KK     = p_classic->find_domain("KvK"); if (ptr_dom_KK   == nullptr) return false;

            // set children relationship
            if (!ptr_dom_KQvK->add_child(ptr_dom_KK)) return false;

            return true;
        }

    private:
        std::map<std::string, _Partition>           _partitions;
        static std::unique_ptr<PartitionManager>    _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

};

#endif
