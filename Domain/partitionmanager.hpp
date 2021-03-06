#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// PartitionManger  : Manage the multiple partitions
//
//
#ifndef _AL_CHESS_DOMAIN_PARTITIONMANAGER_HPP
#define _AL_CHESS_DOMAIN_PARTITIONMANAGER_HPP

namespace chess
{
    // PartitionManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class PartitionManager
    {
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        PartitionManager() {}

        PartitionManager(const PartitionManager&) = delete;
        PartitionManager & operator=(const PartitionManager &) = delete;
        PartitionManager(PartitionManager&&) = delete;

    public:
        ~PartitionManager()
        {
            if (_instance.operator bool())
            {
                _partitions.clear();
                _instance.release();
            }
        }

        static const PartitionManager* instance();

    public:
        _Partition*     load_partition(const std::string& name) const;
        bool            add_partition(_Partition* p)const;
        _Partition*     find_partition(const std::string partition_name) const;
        void            remove_partition(const std::string partition_name) const;

        bool            make_classic_partition() const;       // Exemple

    private:
        std::map<std::string, _Partition*>          _partitions;    // TODO std_unique<> ...
        static std::unique_ptr<PartitionManager>    _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

    // make_classic_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::make_classic_partition() const
    {
        std::stringstream ss_name;
        ss_name << "classic" << std::to_string(_BoardSize);

        // Check if in memory
        _Partition* p_classic = PartitionManager::instance()->find_partition(ss_name.str());
        if (p_classic == nullptr)
        {
            _Partition* p = new _Partition(ss_name.str());
            if (!PartitionManager::instance()->add_partition(p))
            {
                remove_partition(ss_name.str());
                return false;
            }

            // Try loading from disk
            if (p->load() == true)
            {
                return true;
            }
            else
            {
                // Continu to create it
            }
        }
        else
        {
            // Reload from disk
            if (p_classic->load() == false)
            {
                return false;
            }
        }

        // Check if in memory
        p_classic = PartitionManager::instance()->find_partition(ss_name.str());
        if (p_classic == nullptr)
            return false;

        // Create domains
        _Domain* dom_KK   = new DomainKvK< PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(ss_name.str());
        _Domain* dom_KQvK = new DomainKQvK<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>(ss_name.str());

        if (!p_classic->add_domain(dom_KK))     return false;
        if (!p_classic->add_domain(dom_KQvK))   return false;

        _Domain* ptr_dom_KQvK = p_classic->find_domain(_Domain::getDomainName(eDomainName::KQvK), "0"); if (ptr_dom_KQvK == nullptr) return false;
        _Domain* ptr_dom_KK   = p_classic->find_domain(_Domain::getDomainName(eDomainName::KvK) , "0"); if (ptr_dom_KK   == nullptr) return false;

        // set children relationship
        if (!ptr_dom_KQvK->add_child(ptr_dom_KK)) return false;

        ptr_dom_KQvK->save();
        ptr_dom_KK->save();
        p_classic->save();

        return true;
    }

    // find_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::find_partition(const std::string partition_name) const
    {
        if (_instance == nullptr) return nullptr;

        auto& iter = _instance->_partitions.find(partition_name);
        if (iter != _instance->_partitions.end())
        {
            return (iter->second);
        }
        return nullptr;
    }

    // remove_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    void PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::remove_partition(const std::string partition_name) const
    {
        if (_instance == nullptr) return;

        auto& iter = _instance->_partitions.find(partition_name);
        if (iter != _instance->_partitions.end())
        {
            _instance->_partitions.erase(iter);
        }
    }

    // add_partition
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::add_partition(_Partition* p) const
    {
        if (_instance == nullptr) return false;

        auto iter = _instance->_partitions.find(p->name());
        if (iter == _instance->_partitions.end())
        {
            _instance->_partitions.insert(std::pair<std::string, _Partition*>(p->name(), p));
            return true;
        }
        return false;
    }

    // instance()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    const PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::instance()
    {
        if (_instance == nullptr)
        {
            _instance = std::unique_ptr<PartitionManager>(new PartitionManager);
            return _instance.get();
        }
        return  _instance.get();
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT> * 
    PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load_partition(const std::string& name) const
    {
        // Check if in memory
        _Partition* pp = PartitionManager::instance()->find_partition(name);
        if (pp == nullptr)
        {
            _Partition* p = new _Partition(name);
            if (!PartitionManager::instance()->add_partition(p))
            {
                remove_partition(name);
                delete p;
                return nullptr;
            }

            // Try loading from disk
            if (p->load() == true)  return p;
   
            remove_partition(name);
            delete p;
            return nullptr;
        }
        else
        {
            // Reload from disk
            if (pp->load() == false)
            {
                return nullptr;
            }
            return pp;
        }
        return nullptr;
    }

};

#endif
