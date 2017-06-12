#pragma once
//=================================================================================================
//                    Copyright (C) 2017 Alain Lanthier - All Rights Reserved                      
//=================================================================================================
//
//
#ifndef _AL_CHESS_CHESS_H
#define _AL_CHESS_CHESS_H

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

namespace chess
{
    template <typename PieceID, typename uint8_t _BoardSize> class Board;
    template <typename PieceID, typename uint8_t _BoardSize> class Piece;
    template <typename PieceID> struct Move;
    enum class ExactScore :int8_t;
    enum class PieceName;
    enum class PieceColor;

    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Domain;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class Partition;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class PartitionManager;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionValuationNode;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class FeatureManager;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ValuationFeature_numberMoveForPiece;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionFeature_isOppositeKinCheck;

    template <typename T, int PARAM_NBIT> class ChessGeneticAlgorithm;
    template <typename PieceID, typename uint8_t _BoardSize, typename TYPE_PARAM, int PARAM_NBIT> class ConditionValuationNode;
}

#include "core/move.hpp"
#include "core/piece.hpp"
#include "core/board.hpp"
#include "core/unittest.hpp"
#include "core/testboard.hpp"
#include "player/player.hpp"
#include "domain/domain.hpp"
#include "domain/partition.hpp"
#include "domain/partitionmanager.hpp"
#include "persistence/persist.hpp"
#include "feature/basefeature.hpp"
#include "feature/feature.hpp"
#include "feature/condvalunode.hpp"
#include "feature/featuremanager.hpp"
#include "game/game.hpp"
#include "ga/galgo_example.hpp"
#include "ChessGA/ChessGenAlgo.hpp"
#include "ChessGA/ChessCoEvolveGA.hpp"

#endif
