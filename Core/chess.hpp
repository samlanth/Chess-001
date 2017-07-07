#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#ifndef _AL_CHESS_CHESS_HPP
#define _AL_CHESS_CHESS_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <cassert>
#include <list>
#include <future>
#include <mutex>
#include <thread>
#include <conio.h>
#include <bitset>
#include <cmath>
#include <array>


namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;
    template <typename PieceID, typename uint8_t _BoardSize> class Piece;
    template <typename PieceID> struct Move;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Domain;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Partition;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class PartitionManager;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class BasePlayer;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class PlayerFactory;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionValuationNode;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class FeatureManager;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature_numberMoveForPiece;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature_isOppositeKinCheck;

    template <typename T, int PARAM_NBIT> class ChessGeneticAlgorithm;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND> class ChessCoEvolveGA;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class GameDB;
    template <typename PieceID, typename uint8_t _BoardSize> struct GameDB_Record;

    template <typename PieceID, typename uint8_t _BoardSize> class PieceSet;
    template <typename PieceID, typename uint8_t _BoardSize> class TBH_Base;
    template <typename PieceID, typename uint8_t _BoardSize> class TBH;
    template <typename PieceID, typename uint8_t _BoardSize> class TBH_1;
    template <typename PieceID, typename uint8_t _BoardSize> class TBH_2;
    template <typename PieceID, typename uint8_t _BoardSize> class TBH_3;
    template <typename PieceID, typename uint8_t _BoardSize> struct STRUCT_TBH;
    template <typename PieceID, typename uint8_t _BoardSize, uint8_t NPIECE> class TBH_Symmetry;


}

#include "core/util.hpp"
#include "core/move.hpp"
#include "core/piece.hpp"
#include "core/board.hpp"
#include "unittest/unittest.hpp"
#include "unittest/testboard.hpp"
#include "player/playerbase.hpp"
#include "player/player.hpp"
#include "player/playerfactory.hpp"
#include "domain/domain.hpp"
#include "domain/partition.hpp"
#include "domain/partitionmanager.hpp"
#include "persistence/persist.hpp"
#include "feature/basefeature.hpp"
#include "feature/feature.hpp"
#include "feature/condvalunode.hpp"
#include "feature/featuremanager.hpp"
#include "game/game.hpp"
#include "game/gamedb.hpp"
#include "ga/galgo_example.hpp"
#include "ChessGA/ChessGenAlgo.hpp"
#include "ChessGA/ChessCoEvolveGA.hpp"
#include "Tablebase/TB.hpp"
#include "Tablebase/symTB.hpp"
#include "Tablebase/pieceset.hpp"
#include "Tablebase/TBH.hpp"
#include "Tablebase/TBH_N.hpp"
#include "Tablebase/TB_util.hpp"
#include "Tablebase/TB_mgr.hpp"
#include "Tablebase/TBH_mgr.hpp"
#include "Tablebase/TB_N.hpp"
#include "Tablebase/TB_algo.hpp"

#endif
