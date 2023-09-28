/*
 * StatisticsProblemClass.h
 *
 *  Created on: 01.07.2023
 *      Author: Lijia Yuan
 */

#include <vector>

#ifndef STATISTICS_PROBLEM_CLASS_H
#define STATISTICS_PROBLEM_CLASS_H

#include "../Model.h"
#include "NodeProperties.h"

namespace progression {

    void findProblemClass(searchNode *n, Model *htn, NodeProperties *node);

    bool isTotalOrder(searchNode *n, Model *htn, NodeProperties *node);

    bool isNodePrimitive(searchNode *n, Model *htn);

    bool isRegular(searchNode *n, Model *htn, NodeProperties *node);

    bool isAcyclic(searchNode *n, Model *htn, NodeProperties *node);

    bool isTailRecursive(searchNode *n, Model *htn, NodeProperties *node);

    void calStatistics();

    void getAllReachableMethod(Model *htn, int method);

    void getAllTasks(planStep *task);

    extern int numPrimitive;

    extern int numAcyclic;

    extern int numRegular;

    extern int numTotalOrder;

    extern int numExploredNode;

    extern int numTailRecursive;

    extern vector<int> visitedMethod;
    
    extern vector<planStep*> tasksContain;
} 
#endif