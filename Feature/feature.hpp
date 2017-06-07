#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// <... work in progress ...>
//
// Feature<...>     :   Primitive feature (a simple function on a board position)
// Condition feature:   Detect if something is on/off on the board 
// Valuation feature:   Compute some value
//
// Model
//  Fc = conditional term = [Fcond() and/or Fcond() and/or ...]
//  Fv =   valuation term = Sigmoid([Fval()*w + Fval()*w + ...]) range(0 to 1) 0==Black_winning 1==white_winning
//
//
#ifndef _AL_CHESS_FEATURE_H
#define _AL_CHESS_FEATURE_H

#include "core/board.hpp"
#include "core/move.hpp"
#include "domain/domain.hpp"

namespace chess
{
    enum class FeatureType {condition, valuation};

    // Feature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BaseFeature
    {
    public:
        BaseFeature(const std::string aname, const FeatureType atype) : _name(aname), _featureType(atype){ }
        virtual ~BaseFeature() {}

        const std::string name()   const { return _name; }

        // Persistence
        virtual bool save() const   { return false; } //... 
        virtual bool load()         { return false; } //...

    protected:
        std::string                     _name;
        FeatureType                     _featureType;
    };

    // ConditionFeature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionFeature : public BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        ConditionFeature(const std::string aname): BaseFeature(aname, FeatureType::condition)  { }
        virtual bool check(const _Board& position, const std::vector<_Move>& m) const = 0;
    };

    // ConditionFeature_isOppositeKingCheck
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionFeature_isOppositeKinCheck : public ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        ConditionFeature_isOppositeKinCheck() : ConditionFeature("isOppositeKinCheck"){ }
        virtual bool check(const _Board& position, const std::vector<_Move>& m) const override
        {
            size_t ret_mv;
            return position.can_capture_opposite_king(m, ret_mv);
        }
    };

    // ValuationFeature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ValuationFeature : public BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        ValuationFeature(const std::string aname) : BaseFeature(aname, FeatureType::valuation), _PARAM_NBIT(PARAM_NBIT){ }
        virtual TYPE_PARAM compute(const _Board& position, const std::vector<_Move>& m) const = 0;

    protected:
        size_t  _PARAM_NBIT;
    };

    // ValuationFeature_numberMoveForPiece
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ValuationFeature_numberMoveForPiece : public ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _Piece = Piece<PieceID, _BoardSize>;

    public:
        ValuationFeature_numberMoveForPiece(const PieceName p, const PieceColor c) : ValuationFeature("numberMoveForPiece") ,
            _p(p), _c(c)
        { 
            // make name unique
            PieceID id = _Piece::get_id(_p, _c);
            _name = "numberMoveForPiece" + _Piece::to_str(id);
        }
        virtual TYPE_PARAM compute(const _Board& position, const std::vector<_Move>& m) const override
        {
            return (TYPE_PARAM)position.cnt_piece(_p, _c);
        }
    private:
        PieceName   _p;
        PieceColor  _c;
    };

    // FeatureManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureManager
    {
        using _Partition = Partition<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _Domain = Domain<   PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    private:
        FeatureManager() {}

        FeatureManager(const FeatureManager&) = delete;
        FeatureManager & operator=(const FeatureManager &) = delete;
        FeatureManager(FeatureManager&&) = delete;

    public:
        //...register features...

        ~FeatureManager()
        {
            if (_instance.operator bool())
            {
                // cleanup ...
                _instance.release();
            }
        }

    public:
        static const FeatureManager* instance()
        {
            if (_instance == nullptr)
            {
                _instance = std::unique_ptr<FeatureManager>(new FeatureManager);
                return _instance.get();
            }
            return  _instance.get();;
        }

 
    private:
        std::map<std::string, FeatureManager>   _features;
        static std::unique_ptr<FeatureManager>  _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

};

#endif
