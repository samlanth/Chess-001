#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
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

namespace chess
{
    enum class FeatureType :char {condition = 0, valuation = 1};

    enum class FeatureName :int { 
        eConditionFeature_isOppositeKinCheck,
        eValuationFeature_numberMoveForPiece
    };

    // Feature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BaseFeature
    {
        using _Piece = Piece<PieceID, _BoardSize>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        BaseFeature(const FeatureType atype) : _featureType(atype) 
        {
            _classtype = "";
            _classtype_argument = "";
        }
        virtual ~BaseFeature() {}

        const std::string classtype()       const { return _classtype; }
        const std::string classtype_arg()   const { return _classtype_argument; }

        void set_classtype(const std::string& ct)           { _classtype = ct; }
        void set_classtype_argument(const std::string& arg) { _classtype_argument = arg; }

        virtual bool save(std::ofstream& filestream)  const
        { 
            if (filestream.good())
            {
                filestream << (char)_featureType;   filestream << std::endl;
                filestream << _classtype;           filestream << std::endl;
                filestream << _classtype_argument;  filestream << std::endl;
                return true;
            }
            return false;
        } 
        static _ConditionFeature* make_cond(std::ifstream& filestream)
        {
            if (filestream.good())
            {
                FeatureType featureType;
                std::string classtype;
                std::string classtype_argument;

                char c; filestream >> c;
                if (c == 0) featureType = FeatureType::condition; 
                else        featureType = FeatureType::valuation;
                filestream >> classtype;
                filestream >> classtype_argument;

                if (classtype == _FeatureManager::get_feature_name(FeatureName::eConditionFeature_isOppositeKinCheck))
                    return new _ConditionFeature_isOppositeKinCheck();
            }
            return nullptr;
        }
        static _ValuationFeature* make_val(std::ifstream& filestream)
        {
            if (filestream.good())
            {
                FeatureType featureType;
                std::string classtype;
                std::string classtype_argument;

                char c; filestream >> c;
                if (c == 0) featureType = FeatureType::condition;
                else        featureType = FeatureType::valuation;
                filestream >> classtype;
                filestream >> classtype_argument;

                if (classtype == _FeatureManager::get_feature_name(FeatureName::eValuationFeature_numberMoveForPiece))
                {
                    PieceID id = (PieceID)atoi(classtype_argument.c_str());
                    const _Piece* p = _Piece::get(id);
                    return new _ValuationFeature_numberMoveForPiece(p->get_name(), p->get_color());
                }
            }
            return nullptr;
        }

    protected:
        FeatureType                     _featureType;
        std::string                     _classtype;
        std::string                     _classtype_argument;
    };

    // ConditionFeature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionFeature : public BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;

    public:
        ConditionFeature() : BaseFeature(FeatureType::condition)  { }
        virtual ~ConditionFeature() {};
        virtual bool check(const _Board& position, const std::vector<_Move>& m) const = 0;
    };

    // ConditionFeature_isOppositeKingCheck
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class ConditionFeature_isOppositeKinCheck : public ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>
    {
        using _Board = Board<PieceID, _BoardSize>;
        using _Move = Move<PieceID>;
        using _FeatureManager = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

    public:
        ConditionFeature_isOppositeKinCheck() : ConditionFeature()
        {
            set_classtype( _FeatureManager::get_feature_name(FeatureName::eConditionFeature_isOppositeKinCheck) );
        }
        virtual ~ConditionFeature_isOppositeKinCheck() {};
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
        ValuationFeature() : BaseFeature(FeatureType::valuation), _PARAM_NBIT(PARAM_NBIT){ }
        virtual ~ValuationFeature() {};
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
        ValuationFeature_numberMoveForPiece(const PieceName p, const PieceColor c) 
            : ValuationFeature() , _p(p), _c(c)
        { 
            set_classtype(_FeatureManager::get_feature_name(FeatureName::eValuationFeature_numberMoveForPiece));
 
            // make name unique
            PieceID id = _Piece::get_id(_p, _c);
            std::stringstream ss;
            ss << _Piece::to_str(id);
            set_classtype_argument(ss.str());
        }
        virtual ~ValuationFeature_numberMoveForPiece() {};
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
    private:
        FeatureManager() 
        {
            _features.push_back((int)eConditionFeature_isOppositeKinCheck, "ConditionFeature_isOppositeKinCheck");
            _features.push_back((int)eValuationFeature_numberMoveForPiece, "ValuationFeature_numberMoveForPiece");
        }

        FeatureManager(const FeatureManager&) = delete;
        FeatureManager & operator=(const FeatureManager &) = delete;
        FeatureManager(FeatureManager&&) = delete;

    public:
        ~FeatureManager()
        {
            if (_instance.operator bool())
            {
                _features.clear();
                _instance.release();
            }
        }

    public:
        static std::string get_feature_name(FeatureName n)
        {
            return _features[(int)n];
        }

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
        static std::map<int, std::string>       _features;
        static std::unique_ptr<FeatureManager>  _instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<int, std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_features;
};

#endif
