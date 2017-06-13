#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
// BaseFeature<...>     :   Primitive feature (a simple function on a board position)
//
//
#ifndef _AL_CHESS_FEATURE_BASEFEATURE_HPP
#define _AL_CHESS_FEATURE_BASEFEATURE_HPP

namespace chess
{
    enum class FeatureType :char { condition = 0, valuation = 1 };
    enum class CondFeatureName :int {
        eConditionFeature_isOppositeKinCheck
    };
    enum class ValuFeatureName :int {
        eValuationFeature_numberMoveForPiece,
        eValuationFeature_countCaptureKing
    };

    // Feature
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class BaseFeature
    {
        using _FeatureManager   = FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

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

        virtual bool                save(std::ofstream& filestream)  const;
        static _ConditionFeature*   get_cond(std::ifstream& filestream);
        static _ValuationFeature*   get_valu(std::ifstream& filestream);

    protected:
        FeatureType                     _featureType;
        std::string                     _classtype;
        std::string                     _classtype_argument;
    };

    // save(std::ofstream& filestream)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline bool BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(std::ofstream& filestream)  const
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

    // get_cond(std::ifstream& filestream)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*
    BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::get_cond(std::ifstream& filestream)
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

            return _FeatureManager::get_cond_feature(classtype, classtype_argument);
        }
        return nullptr;
    }

    // get_valu(std::ifstream& filestream)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::get_valu(std::ifstream& filestream)
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

            return _FeatureManager::get_valu_feature(classtype, classtype_argument);
        }
        return nullptr;
    }
};

#endif
