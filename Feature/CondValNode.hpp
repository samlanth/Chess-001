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

#include "feature/feature.hpp"

namespace chess
{
    // forward
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class DomainPlayer;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature;

    // CondValNode
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class CondValNode
    {        
        using _CondValNode = CondValNode<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _DomainPlayer = DomainPlayer<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        friend class _DomainPlayer;

    public:
        CondValNode(CondValNode* parent, bool _is_positive_node, bool has_children):
            _is_positive_node(_is_positive_node),
            _parent(parent),
            _positive_child(nullptr),
            _negative_child(nullptr),
            _link_to_mirror(nullptr)
        { 
            if (has_children)
            {
                this->_positive_child = new CondValNode(this, true, false);
                this->_negative_child = new CondValNode(this, false, false);

                this->_positive_child->_link_to_mirror = _negative_child;
                this->_negative_child->_link_to_mirror = _positive_child;
            }
        }
        virtual ~CondValNode() {}

        bool get_condition_value(const _Board& position, const std::vector<_Move>& m) const
        {
            bool cond = true;
            for (size_t i = 0; i < _conditions.size(); i++)
            {
                if (_conditions_and_or[i] == true)
                    cond = cond && _conditions[i].check(position, m);
                else
                    cond = cond || _conditions[i].check(position, m);
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

        TYPE_PARAM get_sigmoid_valuations(const _Board& position, const std::vector<_Move>& m) const
        {
            TYPE_PARAM c = 0;
            for (size_t i = 0; i < _valuations.size(); i++)
            {
                c += _valuations[i].compute(position, m) * _weights[i];
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

        bool eval_position(const _Board& position, const std::vector<_Move>& m, size_t max_node, TYPE_PARAM& ret_eval) const
        {
            _CondValNode* terminal_node = get_terminal_node(position, m);
            if (terminal_node == nullptr)
            {
                return false;
            }
            ret_eval = get_sigmoid_valuations(position, m);
            return true;
        }

        // ...
        // Genetical Algo (Selection, CrossOver, Mutation, Adaptation) would need to:
        // add/remove child nodes 
        // add/remove/replace  _ConditionFeature and formula
        // add/remove/replace  _ValuationFeature and weights
        // ...

    protected:
        bool                            _is_positive_node;

        std::vector<_ConditionFeature>  _conditions;
        std::vector<bool>               _conditions_and_or;
        std::vector<_ValuationFeature>  _valuations;
        std::vector<TYPE_PARAM>         _weights;

        CondValNode*                    _parent;
        CondValNode*                    _positive_child;
        CondValNode*                    _negative_child;    // Miror node - reverse the condition value

        CondValNode*                    _link_to_mirror;
    };


};

#endif

