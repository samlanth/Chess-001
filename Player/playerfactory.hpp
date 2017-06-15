#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
//
#ifndef _AL_CHESS_PLAYER_PLAYERFACTORY_HPP
#define _AL_CHESS_PLAYER_PLAYERFACTORY_HPP

namespace chess
{
    enum class CondFeatureSelection { none, one_random, one_at_index, all };
    enum class ValuFeatureSelection { none, one_random, one_at_index, all };

    struct PlayerFactoryConfig
    {
        CondFeatureSelection cond_s;
        ValuFeatureSelection valu_s;
        bool cascading;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class PlayerFactory
    {
        using _Domain = Domain<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PlayerFactory = PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _PartitionManager = PartitionManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        PlayerFactory()
        {
        }

        PlayerFactory(const PlayerFactory&) = delete;
        PlayerFactory & operator=(const PlayerFactory &) = delete;
        PlayerFactory(PlayerFactory&&) = delete;

    public:
        ~PlayerFactory()
        {
            if (_instance.operator bool())
            {
                _instance.release();
            }
        }

        static const PlayerFactory* instance()
        {
            if (!_instance.operator bool())
            {
                _instance = std::unique_ptr<PlayerFactory>(new PlayerFactory);
            }
            return _instance.get();
        }

        bool reconfigPlayerRoot(_DomainPlayer* player, const PlayerFactoryConfig& cfg) const
        {
            if (player == nullptr) return true;

            size_t nc = _FeatureManager::instance()->count_cond_features();
            size_t nv = _FeatureManager::instance()->count_valu_features();
            if ((cfg.cond_s != CondFeatureSelection::none) && (nc == 0)) return false;
            if ((cfg.valu_s != ValuFeatureSelection::none) && (nv == 0)) return false;

            if (player->get_root()->_positive_child != nullptr)
            {
                delete player->get_root()->_positive_child;
                player->get_root()->_positive_child = nullptr;
            }
            if (player->get_root()->_negative_child != nullptr)
            {
                delete player->get_root()->_positive_child;
                player->get_root()->_positive_child = nullptr;
            }

            if (cfg.cond_s == CondFeatureSelection::one_random)
            {
                _ConditionValuationNode* child1 = new _ConditionValuationNode(player->get_root(), true, false);
                _ConditionValuationNode* child2 = new _ConditionValuationNode(player->get_root(), false, false);

                if (cfg.valu_s == ValuFeatureSelection::all)
                {
                    _ValuationFeature* v;
                    size_t nv = _FeatureManager::instance()->count_valu_features();
                    for (size_t i = 0; i < nv; i++)
                    {
                        v = _FeatureManager::instance()->get_valu_feature(i);
                        if (player->get_domain()->is_valu_feature_valid(*v))
                        {
                            child1->add_valuations(_FeatureManager::instance()->get_valu_feature(i));
                            child1->add_weights(1.0);
                        }
                    }
                    for (size_t i = 0; i < nv; i++)
                    {
                        v = _FeatureManager::instance()->get_valu_feature(i);
                        if (player->get_domain()->is_valu_feature_valid(*v))
                        {
                            child2->add_valuations(_FeatureManager::instance()->get_valu_feature(i));
                            child2->add_weights(1.0);
                        }
                    }
                    if (nc == 1)
                    {
                        child1->add_conditions(_FeatureManager::instance()->get_cond_feature(0));
                        child1->add_conditions_and_or(true);
                    }
                    else
                    {
                        child1->add_conditions(_FeatureManager::instance()->get_cond_feature(std::rand() % nc));
                        child1->add_conditions_and_or(true);
                    }
                    // child2 mirror node reuse child1 cond

                    if (cfg.cascading)
                    {
                        if (!reconfig_children(player, cfg))
                        {
                            //...
                            return false;
                        }
                    }
                    return true;
                }
                else
                {
                    //...
                }
            }
            else if (cfg.cond_s == CondFeatureSelection::none)   // Simplest - only one node of valuations - sum(Fv*w)
            {
                if (cfg.valu_s == ValuFeatureSelection::all)
                {
                    _ValuationFeature* v;
                    for (size_t i = 0; i < nv; i++)
                    {
                        v = _FeatureManager::instance()->get_valu_feature(i);
                        if (player->get_domain()->is_valu_feature_valid(*v))
                        {
                            player->get_root()->add_valuations(_FeatureManager::instance()->get_valu_feature(i));
                            player->get_root()->add_weights(1.0);
                        }
                    }
                    if (cfg.cascading)
                    {
                        if (!reconfig_children(player, cfg))
                        {
                            //...
                            return false;
                        }
                    }
                    return true;
                }
                else
                {
                    //...
                }
            }
            //...
            return false;
        }

        bool reconfig_children(_DomainPlayer* player, const PlayerFactoryConfig& cfg) const
        {
            if (player == nullptr) return false;
            if (player->_domain == nullptr) return false;

            _Partition* partition_ptr = _PartitionManager::instance()->find_partition(player->_domain->_partition_key);
            if (partition_ptr == nullptr) return false;

            _Domain* p_lookup_child_domain;
            for (size_t i = 0; i < player->_domain->_children.size(); i++)
            {                
                p_lookup_child_domain = partition_ptr->find_domain(_Domain::domain_key(player->_domain->_children[i]->_domainname_key, player->_domain->_children[i]->_instance_key));
                if (p_lookup_child_domain != nullptr)
                {
                    if (player->_color_player == PieceColor::W)
                    {
                        if (player->_domain->_children[i]->_attached_domain_playerW != nullptr)
                        {
                            reconfigPlayerRoot(player->_domain->_children[i]->_attached_domain_playerW,cfg);
                            // recursion
                        }
                    }
                    else
                    {
                        if (player->_domain->_children[i]->_attached_domain_playerB != nullptr)
                        {
                            reconfigPlayerRoot(player->_domain->_children[i]->_attached_domain_playerB, cfg);
                            // recursion
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            return true;
        }

    private:
        static std::unique_ptr<PlayerFactory> _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    PlayerFactory<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;


}
#endif
