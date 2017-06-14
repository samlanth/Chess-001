#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#ifndef _AL_CHESS_FEATURE_FEATUREMANAGER_HPP
#define _AL_CHESS_FEATURE_FEATUREMANAGER_HPP

namespace chess
{
    // FeatureManager
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    class FeatureManager
    {
        using _ConditionFeature = ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature = ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        using _ConditionFeature_isOppositeKinCheck = ConditionFeature_isOppositeKinCheck<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_numberMoveForPiece = ValuationFeature_numberMoveForPiece<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;
        using _ValuationFeature_countCaptureKing = ValuationFeature_countCaptureKing<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>;

        using _Piece = Piece<PieceID, _BoardSize>;

    private:
        FeatureManager() 
        {
            _cond_features_name[(int)CondFeatureName::eConditionFeature_isOppositeKinCheck] = "ConditionFeature_isOppositeKinCheck";
           
            _valu_features_name[(int)ValuFeatureName::eValuationFeature_numberMoveForPiece] = "ValuationFeature_numberMoveForPiece";
            _valu_features_name[(int)ValuFeatureName::eValuationFeature_countCaptureKing]   = "ValuationFeature_countCaptureKing";

            // cond set
            _cond_features_instance[get_cond_feature_name(CondFeatureName::eConditionFeature_isOppositeKinCheck)] = (_ConditionFeature*)new _ConditionFeature_isOppositeKinCheck();
           
            // valu set
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::K, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::K, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::R, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::R, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::N, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::N, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::B, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::B, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::Q, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::Q, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::P, PieceColor::W))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::P, PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::K, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::K, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::R, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::R, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::N, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::N, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::B, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::B, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::Q, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::Q, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_numberMoveForPiece) + std::to_string(_Piece::get_id(PieceName::P, PieceColor::B))] = (_ValuationFeature*)new _ValuationFeature_numberMoveForPiece(PieceName::P, PieceColor::B);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_countCaptureKing) + "W"] = (_ValuationFeature*)new _ValuationFeature_countCaptureKing(PieceColor::W);
            _valu_features_instance[get_valu_feature_name(ValuFeatureName::eValuationFeature_countCaptureKing) + "B"] = (_ValuationFeature*)new _ValuationFeature_countCaptureKing(PieceColor::B);

            for (auto it = _cond_features_instance.begin(); it != _cond_features_instance.end(); ++it)
            {
                _v_cond_features_instance.push_back(it->second);
            }
            for (auto it = _valu_features_instance.begin(); it != _valu_features_instance.end(); ++it)
            {
                _v_valu_features_instance.push_back(it->second);
            }
        }

        FeatureManager(const FeatureManager&) = delete;
        FeatureManager & operator=(const FeatureManager &) = delete;
        FeatureManager(FeatureManager&&) = delete;

    public:
        ~FeatureManager()
        {
            if (_instance.operator bool())
            {
                for (size_t i=0;i < _v_cond_features_instance.size(); i++)
                {
                    _v_cond_features_instance[i] = nullptr; // not owner
                }
                for (size_t i = 0; i < _v_valu_features_instance.size(); i++)
                {
                    _v_valu_features_instance[i] = nullptr; // not owner
                }
                _v_cond_features_instance.clear();
                _v_valu_features_instance.clear();

                _cond_features_name.clear();
                _valu_features_name.clear();
                _cond_features_instance.clear();
                _valu_features_instance.clear();
                _instance.release();
            }
        }

    public:
        static size_t count_cond_features() { return _cond_features_instance.size(); }
        static size_t count_valu_features() { return _valu_features_instance.size(); }

        static _ConditionFeature* get_cond_feature(size_t idx)  { return _v_cond_features_instance[idx]; }
        static _ValuationFeature* get_valu_feature(size_t idx)  { return _v_valu_features_instance[idx]; }

        static std::string get_cond_feature_name(CondFeatureName n)
        {
            auto it = _cond_features_name.find((int)n);
            if (it == _cond_features_name.end()) return "";
            return _cond_features_name[(int)n];
        }
        static std::string get_valu_feature_name(ValuFeatureName n)
        {
            auto it = _valu_features_name.find((int)n);
            if (it == _valu_features_name.end()) return "";
            return _valu_features_name[(int)n];
        }

        static _ConditionFeature* get_cond_feature(const std::string name, const std::string arg) 
        {
            auto it = _cond_features_instance.find(name + arg);
            if (it == _cond_features_instance.end()) return nullptr;
            return _cond_features_instance[name+arg];
        }
        static _ValuationFeature* get_valu_feature(const std::string name, const std::string arg)
        {
            auto it = _valu_features_instance.find(name + arg);
            if (it == _valu_features_instance.end()) return nullptr;
            return _valu_features_instance[name+arg];
        }

        static const FeatureManager* instance()
        {
            if (_instance == nullptr)
            {
                _instance = std::unique_ptr<FeatureManager>(new FeatureManager);
                return _instance.get();
            }
            return  _instance.get();
        }

    private:
        static std::map<int, std::string>                   _cond_features_name;
        static std::map<int, std::string>                   _valu_features_name;
        static std::map<std::string, _ConditionFeature*>    _cond_features_instance;
        static std::map<std::string, _ValuationFeature*>    _valu_features_instance;
        static std::unique_ptr<FeatureManager>              _instance;

        // Other method of access
        static std::vector<_ConditionFeature*>              _v_cond_features_instance;
        static std::vector<_ValuationFeature*>              _v_valu_features_instance;
    };

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::unique_ptr<FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_instance = nullptr;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<int, std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_cond_features_name;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<int, std::string>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_valu_features_name;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<std::string, ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_cond_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::map<std::string, ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_valu_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<ConditionFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_v_cond_features_instance;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT>
    std::vector<ValuationFeature<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>*>
    FeatureManager<PieceID, _BoardSize, TYPE_PARAM, PARAM_NBIT>::_v_valu_features_instance;

};

#endif
