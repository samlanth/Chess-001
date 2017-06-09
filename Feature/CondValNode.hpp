#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// CondValNode  : Binary tree of ConditionFeature and their attached ValuationFeature formula
//
// Model
//  Fc = conditional term = [Fcond() and/or Fcond() and/or ...]
//  Fv =   valuation term = Sigmoid([Fval()*w + Fval()*w + ...]) range(0 to 1) 0==Black_winning 1==white_winning
//
//
#ifndef _AL_CHESS_CONDVALNODE_H
#define _AL_CHESS_CONDVALNODE_H

namespace chess
{
    // CondValNode
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class CondValNode
    {        
        using _CondValNode = CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _BaseFeature = BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

    public:
        CondValNode(CondValNode* parent, bool _is_positive_node, bool create_children):
            _is_positive_node(_is_positive_node),
            _parent(parent),
            _positive_child(nullptr),
            _negative_child(nullptr),
            _link_to_mirror(nullptr),
            _persist_key("")
        { 
            _persist_key = PersistManager::instance()->make_persist_key();
            if (create_children)
            {
                this->_positive_child = new CondValNode(this, true, false);
                this->_negative_child = new CondValNode(this, false, false);
                this->_positive_child->_link_to_mirror = _negative_child;
                this->_negative_child->_link_to_mirror = _positive_child;

                if (_parent != nullptr)
                {
                    if (_is_positive_node)
                    {
                        if (_parent->_positive_child != nullptr)
                        {
                            delete _parent->_positive_child;
                        }
                        _parent->_positive_child = this;
                    }
                    else
                    {
                        if (_parent->_negative_child != nullptr)
                        {
                            delete _parent->_negative_child;
                        }
                        _parent->_negative_child = this;
                    }
                }
            }
            else if (_parent!= nullptr)
            {
                if (_is_positive_node)
                {
                    _parent->_positive_child = this;
                }
                else
                {
                    _parent->_negative_child = this;
                }
            }
        }

        virtual ~CondValNode() 
        {
            try
            {
                _conditions.clear();
                _valuations.clear();

                // recursion
                if (_positive_child != nullptr)
                {
                    delete _positive_child; 
                    _positive_child = nullptr;
                }
                if (_negative_child != nullptr)
                {
                    delete _negative_child;
                    _negative_child = nullptr;
                }
            }
            catch(...)
            {
            }
        }

        bool isRoot() const {return (_parent == nullptr);}

        bool get_condition_value(const _Board& position, const std::vector<_Move>& m) const
        {
            if (isRoot()) return true;
            if (!_is_positive_node)
            {
                if (_link_to_mirror != nullptr) return !(_link_to_mirror->get_condition_value(position, m));
                else
                {
                    // May assume conditions of negative node were copied from positive node...
                    assert(false);
                }
            }

            bool cond = true;
            for (size_t i = 0; i < _conditions.size(); i++)
            {
                if (_conditions_and_or[i] == true)
                    cond = cond && _conditions[i]->check(position, m);
                else
                    cond = cond || _conditions[i]->check(position, m);
            }
            if (!_is_positive_node) cond = !cond;
            return cond;
        }

        _CondValNode* get_terminal_node(const _Board& position, const std::vector<_Move>& m) const
        {
            // if on a node it means the composite conditions to this node was true;
            if ((_positive_child != nullptr) && (_negative_child != nullptr))
            {
                bool cond_child = _positive_child->get_condition_value(position, m);
                if (cond_child)
                    return _positive_child->get_terminal_node(position, m);
                else
                    return _negative_child->get_terminal_node(position, m);
            }
            else if ((_positive_child == nullptr) && (_negative_child == nullptr))
            {
                return (_CondValNode*)this;
            }
            // error
            return nullptr;
        }

        bool save_root() const;
        bool save(std::ofstream& filestream) const;
        bool load_root();
        bool load(std::ifstream& filestream);

        TYPE_PARAM get_sigmoid_valuations(const _Board& position, const std::vector<_Move>& m) const
        {
            TYPE_PARAM c = 0;
            for (size_t i = 0; i < _valuations.size(); i++)
            {
                c += _valuations[i]->compute(position, m) * _weights[i];
            }
            return sigmoid(c);
        }

        inline float sigmoid(float x, float a = 1.0f) const
        {
            return 1 / (1 + std::exp(-a * x));
        }

        inline double sigmoid(double x, double a = 1.0) const
        {
            return 1 / (1 + std::exp(-a * x));
        }

        bool eval_position(const _Board& position, const std::vector<_Move>& m, TYPE_PARAM& ret_eval) const
        {
            _CondValNode* terminal_node = get_terminal_node(position, m);
            if (terminal_node == nullptr)
            {
                return false;
            }
            ret_eval = terminal_node->get_sigmoid_valuations(position, m);
            return true;
        }

        // ...
        // Genetical Algo (Selection, CrossOver, Mutation, Adaptation) would need to:
        // add/remove child nodes 
        // add/remove/replace  _ConditionFeature and formula
        // add/remove/replace  _ValuationFeature and weights
        // ...

        std::string persist_key() const { return _persist_key; }

    protected:
        bool                            _is_positive_node;

        std::vector<_ConditionFeature*> _conditions;
        std::vector<bool>               _conditions_and_or;
        std::vector<_ValuationFeature*> _valuations;
        std::vector<TYPE_PARAM>         _weights;

        CondValNode*                    _parent;
        CondValNode*                    _positive_child;
        CondValNode*                    _negative_child;    // Miror node - reverse the condition value

        CondValNode*                    _link_to_mirror;
        std::string                     _persist_key;
    };

    // save_root()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save_root() const
    {
        std::string f = PersistManager::instance()->get_stream_name("CondValNode", _persist_key);
        std::ofstream   filestream;
        filestream.open(f.c_str(), std::fstream::out | std::fstream::trunc);
        if (save(filestream))
        {
            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(std::ofstream& filestream) const
    {
        if (filestream.good())
        {
            filestream << _persist_key;         filestream << std::endl;
            filestream << _is_positive_node;    filestream << std::endl;
            filestream << _conditions.size();   filestream << std::endl;
            for (auto& v : _conditions)
            {
                v->save(filestream);
            }
            filestream << _conditions_and_or.size();   filestream << std::endl;
            for (auto v : _conditions_and_or)
            {
                filestream << v; filestream << std::endl;
            }
            filestream << _valuations.size();   filestream << std::endl;
            for (auto& v : _valuations)
            {
                v->save(filestream);
            }
            filestream << _weights.size();   filestream << std::endl;
            for (auto& v : _weights)
            {
                filestream << v; filestream << std::endl;
            }

            if (_parent != nullptr) { filestream << _parent->_persist_key; filestream << std::endl; }
            else { filestream << "null_key"; filestream << std::endl; }

            if (_positive_child != nullptr) { filestream << _positive_child->_persist_key; filestream << std::endl; }
            else { filestream << "null_key"; filestream << std::endl; }

            if (_negative_child != nullptr) { filestream << _negative_child->_persist_key; filestream << std::endl; }
            else { filestream << "null_key"; filestream << std::endl; }

            if (_link_to_mirror != nullptr) { filestream << _link_to_mirror->_persist_key; filestream << std::endl; }
            else { filestream << "null_key"; filestream << std::endl; }

            if (_positive_child != nullptr)
            {
                _positive_child->save(filestream);  // recursion
            }
            if (_negative_child != nullptr)
            {
                _negative_child->save(filestream);  // recursion
            }

            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load_root()
    {
        std::string f = PersistManager::instance()->get_stream_name("CondValNode", _persist_key);
        std::ifstream   filestream;
        filestream.open(f.c_str(), std::fstream::in);
        if (load(filestream))
        {
            filestream.close();
            return true;
        }
        filestream.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load(std::ifstream& filestream)
    {
        if (filestream.good())
        {
            size_t n;
            std::string persist_key;
            filestream >> persist_key;
            assert(persist_key == _persist_key);

            filestream >> _is_positive_node;

            _conditions.clear();
            filestream >> n;
            for(size_t i=0; i< n; i++)
            {
                _ConditionFeature* feature = _BaseFeature::make_cond(filestream);
                if (feature == nullptr) return false;
                _conditions.push_back(feature);
            }

            _conditions_and_or.clear();
            filestream >> n;
            for (size_t i = 0; i< n; i++)
            {
                bool v;
                filestream >> v;
                _conditions_and_or.push_back(v); 
            }

            _valuations.clear();
            filestream >> n;
            for (size_t i = 0; i< n; i++)
            {
                _ValuationFeature* feature = _BaseFeature::make_val(filestream);
                if (feature == nullptr) return false;
                _valuations.push_back(feature);
            }

            _weights.clear();
            filestream >> n;
            for (size_t i = 0; i< n; i++)
            {
                TYPE_PARAM v;
                filestream >> v;
                _weights.push_back(v);
            }

            std::string _parent_persist_key;
            std::string _positive_child_persist_key;
            std::string _negative_child_persist_key;
            std::string _link_to_mirror_persist_key;

            filestream >> _parent_persist_key;
            filestream >> _positive_child_persist_key;
            filestream >> _negative_child_persist_key;
            filestream >> _link_to_mirror_persist_key;

            std::map<std::string, CondValNode*> map_nodes;
            if (_parent_persist_key == "null_key")
            {
                _parent = nullptr;
            }
            else
            {
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _parent = map_nodes[_parent_persist_key];
                }
            }

            if (_positive_child_persist_key == "null_key")
            {
                _positive_child = nullptr;
            }
            else
            {
                _positive_child = new CondValNode(_parent, true, false);
                map_nodes[_positive_child_persist_key] = _positive_child;
                _positive_child->_persist_key = _positive_child_persist_key;
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _positive_child->_parent = map_nodes[_parent_persist_key];
                    _positive_child->_parent->_positive_child = _positive_child;
                }
            }

            if (_negative_child_persist_key == "null_key")
            {
                _negative_child = nullptr;
            }
            else
            {
                _negative_child = new CondValNode(_parent, false, false);
                map_nodes[_negative_child_persist_key] = _negative_child;
                _negative_child->_persist_key = _negative_child_persist_key;
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _negative_child->_parent = map_nodes[_parent_persist_key];
                    _negative_child->_parent->_negative_child = _negative_child;
                }
            }

            if (_link_to_mirror_persist_key == "null_key")
            {
                _link_to_mirror = nullptr;
            }
            else
            {
                if (map_nodes[_link_to_mirror_persist_key] != nullptr)
                {
                    _link_to_mirror = map_nodes[_link_to_mirror_persist_key];
                }
            }

            if (_positive_child != nullptr)
            {
                _positive_child->load(filestream);
            }
            if (_negative_child != nullptr)
            {
                _negative_child->load(filestream);
            }

            // recheck
            if ((_parent_persist_key != "null_key") && (_parent == nullptr))
            {
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _parent = map_nodes[_parent_persist_key];
                }
            }
            if ((_positive_child_persist_key != "null_key") && (_positive_child == nullptr))
            {
                if (map_nodes[_positive_child_persist_key] != nullptr)
                {
                    _positive_child = map_nodes[_positive_child_persist_key];
                }
            }
            if ((_negative_child_persist_key != "null_key") && (_negative_child == nullptr))
            {
                if (map_nodes[_negative_child_persist_key] != nullptr)
                {
                    _negative_child = map_nodes[_negative_child_persist_key];
                }
            }
            //_link...

            return true;
        }
        return false;
    }

};

#endif

