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
    //enum class FeatureType { condition = 0, valuation = 1 };

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

        virtual bool                save(std::ofstream& is)  const;
        static _ConditionFeature*   get_cond(std::ifstream& is);
        static _ValuationFeature*   get_valu(std::ifstream& is);

    protected:
        FeatureType                     _featureType;
        std::string                     _classtype;
        std::string                     _classtype_argument;
    };

    // save(std::ofstream& is)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline bool BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::save(std::ofstream& is)  const
    {
        if (is.good())
        {
            is << FeatureType_to_int(_featureType);    is << " ";
            is << toNULLSTR(_classtype);           is << " ";
            is << toNULLSTR(_classtype_argument);  is << " ";
            return true;
        }
        return false;
    }

    // get_cond(std::ifstream& is)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*
    BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::get_cond(std::ifstream& is)
    {
        if (is.good())
        {
            FeatureType featureType;
            std::string classtype;
            std::string classtype_argument;

            int c; is >> c;
            int_to_FeatureType(c);
            featureType = int_to_FeatureType(c);
            is >> classtype;            fromNULLSTR(classtype);
            is >> classtype_argument;   fromNULLSTR(classtype_argument);

            return _FeatureManager::get_cond_feature(classtype, classtype_argument);
        }
        return nullptr;
    }

    // get_valu(std::ifstream& is)
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    inline ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>* 
    BaseFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::get_valu(std::ifstream& is)
    {
        if (is.good())
        {
            FeatureType featureType;
            std::string classtype;
            std::string classtype_argument;

            int c; is >> c;
            int_to_FeatureType(c);
            featureType = int_to_FeatureType(c);
            is >> classtype;
            is >> classtype_argument;

            return _FeatureManager::get_valu_feature(classtype, classtype_argument);
        }
        return nullptr;
    }
};

#endif
