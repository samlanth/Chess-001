#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// ConditionValuationNode  : Binary tree of ConditionFeature and their attached ValuationFeature formula
//
// Model
//  Fc = conditional term = [Fcond() and/or Fcond() and/or ...]
//  Fv =   valuation term = Sigmoid([Fval()*w + Fval()*w + ...]) range(0 to 1) 0==Black_winning 1==white_winning
//
//
#ifndef _AL_CHESS_FEATURE_CONDVALNODE_HPP
#define _AL_CHESS_FEATURE_CONDVALNODE_HPP

namespace chess
{
    inline float  sigmoid(float x, float   a = 0.001f)  { return (1 / (1 + std::exp(-a * x))); }
    inline double sigmoid(double x, double a = 0.001)   { return (1 / (1 + std::exp(-a * x))); }

    // ConditionValuationNode
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionValuationNode
    {        
        using _ConditionValuationNode = ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _BaseFeature = BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

        const std::string NULLKEY = "NullKEY";

    public:
        ConditionValuationNode(ConditionValuationNode* parent, bool _is_positive_node, bool create_children):
            _is_positive_node(_is_positive_node),
            _parent(parent),
            _positive_child(nullptr),
            _negative_child(nullptr),
            _link_to_mirror(nullptr),
            _persist_key("")
        { 
            _persist_key = PersistManager::instance()->create_persist_key();
            if (create_children)
            {
                this->_positive_child = new ConditionValuationNode(this, true, false);
                this->_negative_child = new ConditionValuationNode(this, false, false);
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
                    if (_parent->_negative_child != nullptr)
                    {
                        _parent->_positive_child->_link_to_mirror = _parent->_negative_child;
                        _parent->_negative_child->_link_to_mirror = _parent->_positive_child;
                    }
                }
                else
                {
                    _parent->_negative_child = this;
                    if (_parent->_positive_child != nullptr)
                    {
                        _parent->_negative_child->_link_to_mirror = _parent->_positive_child;
                        _parent->_positive_child->_link_to_mirror = _parent->_negative_child;
                    }
                }
            }
        }

        virtual ~ConditionValuationNode() 
        {
            // recursion to delete full branch
            try
            {
                _conditions.clear();
                _valuations.clear();

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
                assert(false);
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
                    assert(false);
                    // throw..
                }
            }

            assert(_conditions_and_or.size() == _conditions.size() );
            bool cond = true;
            for (size_t i = 0; i < _conditions.size(); i++)
            {
                if (i == 0) cond = _conditions[0]->check(position, m); //_conditions_and_or[0] ignored (assume true)
                else
                {
                    if (_conditions_and_or[i] == true)
                        cond = cond && _conditions[i]->check(position, m);
                    else
                        cond = cond || _conditions[i]->check(position, m);
                }
            }
            if (!_is_positive_node) 
                cond = !cond;
            return cond;
        }

        _ConditionValuationNode* get_terminal_node(const _Board& position, const std::vector<_Move>& m) const
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
                return (_ConditionValuationNode*)this;
            }

            // error
            assert(false);
            return nullptr;
        }

        bool save_root() const;
        bool save(std::ofstream& is) const;
        bool load_root();
        bool load(std::ifstream& is);

        TYPE_PARAM get_valuations_value(const _Board& position, const std::vector<_Move>& m, char verbose) const
        {
            assert(_weights.size() >= _valuations.size());

            TYPE_PARAM c = 0;
            TYPE_PARAM v = 0;

            if (verbose > 1) std::cout << _is_positive_node << " ";
            for (size_t i = 0; i < _valuations.size(); i++)
            {
                v = _valuations[i]->compute(position, m);
                c += (v * _weights[i]);
                if (verbose > 1)
                {
                    std::cout << v << "*" << _weights[i] << " ";
                }
            }
            if (verbose > 1) std::cout << "(" << c << "," << sigmoid(c) << ")" << std::endl;
            return sigmoid(c);
        }

        bool eval_position(const _Board& position, const std::vector<_Move>& m, TYPE_PARAM& ret_eval, char verbose) const
        {
            _ConditionValuationNode* terminal_node = get_terminal_node(position, m);
            if (terminal_node == nullptr)
            {
                return false;
            }
            ret_eval = terminal_node->get_valuations_value(position, m, verbose);
            return true;
        }

        std::string persist_key() const { return _persist_key; }
        void set_persist_key(std::string s) { _persist_key = s; }

        std::vector<TYPE_PARAM>& weights() { return _weights; }
        ConditionValuationNode* positive_child() const { return _positive_child; }
        ConditionValuationNode* negative_child() const { return _negative_child; }

        std::vector<_ConditionFeature*> get_conditions()        const { return _conditions; }
        std::vector<bool>               get_conditions_and_or() const { return _conditions_and_or; }
        std::vector<_ValuationFeature*> get_valuations()        const { return _valuations; }
        std::vector<TYPE_PARAM>         get_weights()           const { return _weights; }

        void set_conditions(std::vector<_ConditionFeature*>& v) { _conditions = v; }
        void set_conditions_and_or(std::vector<bool>& v)        { _conditions_and_or = v; }
        void set_valuations(std::vector<_ValuationFeature*>& v) { _valuations = v; }
        void set_weights(std::vector<TYPE_PARAM>& v)            { _weights = v; }

        void add_conditions(_ConditionFeature* v)   { _conditions.push_back(v); }
        void add_conditions_and_or(bool v)          { _conditions_and_or.push_back(v); }
        void add_valuations(_ValuationFeature* v)   { _valuations.push_back(v); }
        void add_weights(TYPE_PARAM v)              { _weights.push_back(v); }

        void get_term_nodes(std::vector<_ConditionValuationNode*>& v)
        {
            if ((_positive_child == nullptr) && (_negative_child == nullptr))
            {
                v.push_back(this);
            }
            else
            {
                if (_positive_child != nullptr)
                {
                    _positive_child->get_term_nodes(v);
                }
                if (_negative_child != nullptr)
                {
                    _negative_child->get_term_nodes(v);
                }
            }
        }

    protected:
        bool _is_positive_node;

        std::vector<_ConditionFeature*> _conditions;
        std::vector<bool>               _conditions_and_or;
        std::vector<_ValuationFeature*> _valuations;
        std::vector<TYPE_PARAM>         _weights;

        ConditionValuationNode*         _parent;
        ConditionValuationNode*         _positive_child;
        ConditionValuationNode*         _negative_child;    // Miror node - reverse the condition value
        ConditionValuationNode*         _link_to_mirror;
        std::string                     _persist_key;
    };

    // save_root()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save_root() const
    {
        std::string f = PersistManager::instance()->get_stream_name("ConditionValuationNode", _persist_key);
        std::ofstream   is;
        is.open(f.c_str(), std::fstream::out | std::fstream::trunc);
        if (save(is))
        {
            is.close();
            return true;
        }
        is.close();
        return false;
    }

    // save()
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(std::ofstream& is) const
    {
        if (is.good())
        {
            is << _persist_key;         is << " ";
            is << _is_positive_node;    is << " ";
            is << _conditions.size();   is << " ";
            for (auto& v : _conditions)
            {
                v->save(is);
            }
            is << _conditions_and_or.size();   is << " ";
            for (auto v : _conditions_and_or)
            {
                is << v; is << " ";
            }
            is << _valuations.size();   is << " ";
            for (auto& v : _valuations)
            {
                v->save(is);
            }
            is << _weights.size();   is << " ";
            for (auto& v : _weights)
            {
                is << v; is << " ";
            }

            if (_parent != nullptr) { is << _parent->_persist_key; is << " "; }
            else { is << "NULLKEY"; is << " "; }

            if (_positive_child != nullptr) { is << _positive_child->_persist_key; is << " "; }
            else { is << "NULLKEY"; is << " "; }

            if (_negative_child != nullptr) { is << _negative_child->_persist_key; is << " "; }
            else { is << "NULLKEY"; is << " "; }

            if (_link_to_mirror != nullptr) { is << _link_to_mirror->_persist_key; is << " "; }
            else { is << "NULLKEY"; is << " "; }

            if (_positive_child != nullptr)
            {
                _positive_child->save(is);  // recursion
            }
            if (_negative_child != nullptr)
            {
                _negative_child->save(is);  // recursion
            }

            is.close();
            return true;
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load_root()
    {
        std::string f = PersistManager::instance()->get_stream_name("ConditionValuationNode", _persist_key);
        std::ifstream   is;
        is.open(f.c_str(), std::fstream::in);
        if (load(is))
        {
            is.close();
            return true;
        }
        is.close();
        return false;
    }

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    bool ConditionValuationNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::load(std::ifstream& is)
    {
        if (is.good())
        {
            size_t n;
            std::string persist_key;
            is >> persist_key;
            assert(persist_key == _persist_key);

            is >> _is_positive_node;

            for (size_t i = 0; i < _conditions.size(); i++) _conditions[i] = nullptr; // not owner
            _conditions.clear();
            is >> n;
            for(size_t i=0; i< n; i++)
            {
                _ConditionFeature* feature = _BaseFeature::get_cond(is);
                if (feature == nullptr) return false;
                _conditions.push_back(feature);
            }

            _conditions_and_or.clear();
            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                bool v;
                is >> v;
                _conditions_and_or.push_back(v); 
            }

            for (size_t i = 0; i < _valuations.size(); i++) _valuations[i] = nullptr; // not owner
            _valuations.clear();
            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                _ValuationFeature* feature = _BaseFeature::get_valu(is);
                if (feature == nullptr) return false;
                _valuations.push_back(feature);
            }

            _weights.clear();
            is >> n;
            for (size_t i = 0; i< n; i++)
            {
                TYPE_PARAM v;
                is >> v;
                _weights.push_back(v);
            }

            std::string _parent_persist_key;
            std::string _positive_child_persist_key;
            std::string _negative_child_persist_key;
            std::string _link_to_mirror_persist_key;

            is >> _parent_persist_key;
            is >> _positive_child_persist_key;
            is >> _negative_child_persist_key;
            is >> _link_to_mirror_persist_key;

            std::map<std::string, ConditionValuationNode*> map_nodes;
            if (_parent_persist_key == "NULLKEY")
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

            if (_positive_child_persist_key == "NULLKEY")
            {
                _positive_child = nullptr;
            }
            else
            {
                _positive_child = new ConditionValuationNode(_parent, true, false);
                map_nodes[_positive_child_persist_key] = _positive_child;
                _positive_child->_persist_key = _positive_child_persist_key;
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _positive_child->_parent = map_nodes[_parent_persist_key];
                    _positive_child->_parent->_positive_child = _positive_child;
                }
            }

            if (_negative_child_persist_key == "NULLKEY")
            {
                _negative_child = nullptr;
            }
            else
            {
                _negative_child = new ConditionValuationNode(_parent, false, false);
                map_nodes[_negative_child_persist_key] = _negative_child;
                _negative_child->_persist_key = _negative_child_persist_key;
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _negative_child->_parent = map_nodes[_parent_persist_key];
                    _negative_child->_parent->_negative_child = _negative_child;
                }
            }

            if (_link_to_mirror_persist_key == "NULLKEY")
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
                _positive_child->load(is);
            }
            if (_negative_child != nullptr)
            {
                _negative_child->load(is);
            }

            // recheck
            if ((_parent_persist_key != "NULLKEY") && (_parent == nullptr))
            {
                if (map_nodes[_parent_persist_key] != nullptr)
                {
                    _parent = map_nodes[_parent_persist_key];
                }
            }
            if ((_positive_child_persist_key != "NULLKEY") && (_positive_child == nullptr))
            {
                if (map_nodes[_positive_child_persist_key] != nullptr)
                {
                    _positive_child = map_nodes[_positive_child_persist_key];
                }
            }
            if ((_negative_child_persist_key != "NULLKEY") && (_negative_child == nullptr))
            {
                if (map_nodes[_negative_child_persist_key] != nullptr)
                {
                    _negative_child = map_nodes[_negative_child_persist_key];
                }
            }
            if ((_link_to_mirror_persist_key != "NULLKEY") && (_link_to_mirror == nullptr))
            {
                if (map_nodes[_link_to_mirror_persist_key] != nullptr)
                {
                    _link_to_mirror = map_nodes[_link_to_mirror_persist_key];
                }
            }

            return true;
        }
        return false;
    }

};

#endif

