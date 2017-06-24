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
#include <conio.h>
#include <bitset>
#include <cmath>
#include <array>

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;
    template <typename PieceID, typename uint8_t _BoardSize> class Piece;
    template <typename PieceID> struct Move;
    enum class ExactScore;
    enum class PieceName;
    enum class PieceColor;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Domain;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Partition;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class PartitionManager;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class PlayerFactory;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionValuationNode;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class FeatureManager;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature_numberMoveForPiece;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature_isOppositeKinCheck;

    template <typename T, int PARAM_NBIT> class ChessGeneticAlgorithm;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionValuationNode;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT, int WEIGHT_BOUND> class ChessCoEvolveGA;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class GameDB;
    template <typename PieceID, typename uint8_t _BoardSize> struct GameDB_Record;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class BasePlayer;

    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseBaseHandler_1;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseBaseHandler_2;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseBaseHandler_3;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseBaseHandlerCore;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_Xv0;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_0vX;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_2v0;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_0v2;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_1v1;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseHandler_2v1;
    template <typename PieceID, typename uint8_t _BoardSize> class TablebaseManager;

    //template <typename PieceID, typename uint8_t _BoardSize> class Tablebase_2v0;

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
#include "Tablebase/tablebase.hpp"
#include "Tablebase/symmetryTB.hpp"
#include "Tablebase/pieceset.hpp"
#include "Tablebase/tablebasehandler.hpp"
#include "Tablebase/tablebasehandler_1.hpp"
#include "Tablebase/tablebasehandler_2.hpp"
#include "Tablebase/tablebasehandler_3.hpp"
#include "Tablebase/tablebaseutil.hpp"
#include "Tablebase/tablebasemanager.hpp"
#include "Tablebase/Tablebase_Xv0.hpp"
#include "Tablebase/Tablebase_0vX.hpp"
#include "Tablebase/tablebase_1v1.hpp"
#include "Tablebase/tablebase_2v1.hpp"

#endif
